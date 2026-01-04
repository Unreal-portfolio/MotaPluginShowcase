#include "MotaEzEncryption.h"
#include "MotaEzSaveGameSettings.h"
#include "Misc/Base64.h"
#include "Misc/SecureHash.h"
#include "HAL/PlatformMisc.h"
#include "Misc/App.h"
#include "Misc/Paths.h"

bool UMotaEzEncryption::ShouldEncrypt()
{
    #if WITH_EDITOR
        return false;
    #else
        const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
        return Settings && Settings->bEnableEncryption;
    #endif
}

bool UMotaEzEncryption::EncryptString(const FString& PlainText, FString& OutEncrypted, FString& OutError)
{
    if (!ShouldEncrypt())
    {
        OutEncrypted = PlainText;
        return true;
    }

    FTCHARToUTF8 Converter(*PlainText);
    TArray<uint8> PlainData;
    PlainData.Append((uint8*)Converter.Get(), Converter.Length());

    TArray<uint8> EncryptedData;
    FString Key = GetEncryptionKey();
    
    if (!EncryptAES(PlainData, Key, EncryptedData))
    {
        OutError = TEXT("Encryption failed");
        return false;
    }

    OutEncrypted = FBase64::Encode(EncryptedData);
    return true;
}

bool UMotaEzEncryption::DecryptString(const FString& EncryptedText, FString& OutPlainText, FString& OutError)
{
    if (!ShouldEncrypt())
    {
        OutPlainText = EncryptedText;
        return true;
    }

    TArray<uint8> EncryptedData;
    if (!FBase64::Decode(EncryptedText, EncryptedData))
    {
        OutError = TEXT("Base64 decode failed");
        return false;
    }

    TArray<uint8> PlainData;
    FString Key = GetEncryptionKey();
    
    if (!DecryptAES(EncryptedData, Key, PlainData))
    {
        OutError = TEXT("Decryption failed");
        return false;
    }

    PlainData.Add(0);
    OutPlainText = FString(UTF8_TO_TCHAR(PlainData.GetData()));
    return true;
}

FString UMotaEzEncryption::GetEncryptionKey()
{
    const UMotaEzSaveGameSettings* Settings = GetDefault<UMotaEzSaveGameSettings>();
    if (!Settings)
    {
        return GenerateRandomKey();
    }

    if (Settings->EncryptionKey.IsEmpty())
    {
        FString ProjectName = FApp::GetProjectName();
        FString MachineId = FPlatformMisc::GetLoginId();
        FString Combined = ProjectName + TEXT("_") + MachineId;
        
        FString Hash = FMD5::HashAnsiString(*Combined);
        return Hash;
    }

    return Settings->EncryptionKey;
}

FString UMotaEzEncryption::GenerateRandomKey()
{
    FGuid Guid = FGuid::NewGuid();
    return Guid.ToString();
}

bool UMotaEzEncryption::EncryptAES(const TArray<uint8>& PlainData, const FString& Key, TArray<uint8>& OutEncrypted)
{
    ANSICHAR KeyAnsi[32] = {0};
    FString KeyHash = FMD5::HashAnsiString(*Key);
    
    for (int32 i = 0; i < 32; ++i)
    {
        int32 HashIndex = (i / 2) % 16;
        uint8 HexValue = 0;
        
        if (HashIndex * 2 < KeyHash.Len())
        {
            TCHAR HighChar = KeyHash[HashIndex * 2];
            TCHAR LowChar = (HashIndex * 2 + 1 < KeyHash.Len()) ? KeyHash[HashIndex * 2 + 1] : '0';
            HexValue = (FParse::HexDigit(HighChar) << 4) | FParse::HexDigit(LowChar);
        }
        
        KeyAnsi[i] = (ANSICHAR)(HexValue ^ (i * 7));
    }

    int32 PaddedSize = ((PlainData.Num() + 15) / 16) * 16;
    
    TArray<uint8> PaddedData;
    PaddedData.SetNum(PaddedSize);
    
    FMemory::Memcpy(PaddedData.GetData(), PlainData.GetData(), PlainData.Num());
    
    uint8 PaddingValue = PaddedSize - PlainData.Num();
    for (int32 i = PlainData.Num(); i < PaddedSize; ++i)
    {
        PaddedData[i] = PaddingValue;
    }

    OutEncrypted.SetNum(PaddedSize);
    
    for (int32 BlockStart = 0; BlockStart < PaddedSize; BlockStart += 16)
    {
        uint8 Block[16];
        FMemory::Memcpy(Block, &PaddedData[BlockStart], 16);
        
        for (int32 Round = 0; Round < 14; ++Round)
        {
            for (int32 i = 0; i < 16; ++i)
            {
                uint8 KeyByte = KeyAnsi[(Round * 16 + i) % 32];
                Block[i] = (Block[i] + KeyByte + Round) ^ ((Block[i] << 1) | (Block[i] >> 7));
            }
            
            for (int32 i = 0; i < 4; ++i)
            {
                uint8 a = Block[i * 4 + 0];
                uint8 b = Block[i * 4 + 1];
                uint8 c = Block[i * 4 + 2];
                uint8 d = Block[i * 4 + 3];
                
                Block[i * 4 + 0] = a ^ b ^ c ^ d ^ KeyAnsi[(Round + i * 4 + 0) % 32];
                Block[i * 4 + 1] = b ^ c ^ d ^ a ^ KeyAnsi[(Round + i * 4 + 1) % 32];
                Block[i * 4 + 2] = c ^ d ^ a ^ b ^ KeyAnsi[(Round + i * 4 + 2) % 32];
                Block[i * 4 + 3] = d ^ a ^ b ^ c ^ KeyAnsi[(Round + i * 4 + 3) % 32];
            }
        }
        
        FMemory::Memcpy(&OutEncrypted[BlockStart], Block, 16);
    }

    return true;
}

bool UMotaEzEncryption::DecryptAES(const TArray<uint8>& EncryptedData, const FString& Key, TArray<uint8>& OutDecrypted)
{
    if (EncryptedData.Num() == 0 || EncryptedData.Num() % 16 != 0)
    {
        return false;
    }

    ANSICHAR KeyAnsi[32] = {0};
    FString KeyHash = FMD5::HashAnsiString(*Key);
    
    for (int32 i = 0; i < 32; ++i)
    {
        int32 HashIndex = (i / 2) % 16;
        uint8 HexValue = 0;
        
        if (HashIndex * 2 < KeyHash.Len())
        {
            TCHAR HighChar = KeyHash[HashIndex * 2];
            TCHAR LowChar = (HashIndex * 2 + 1 < KeyHash.Len()) ? KeyHash[HashIndex * 2 + 1] : '0';
            HexValue = (FParse::HexDigit(HighChar) << 4) | FParse::HexDigit(LowChar);
        }
        
        KeyAnsi[i] = (ANSICHAR)(HexValue ^ (i * 7));
    }

    TArray<uint8> DecryptedPadded;
    DecryptedPadded.SetNum(EncryptedData.Num());

    for (int32 BlockStart = 0; BlockStart < EncryptedData.Num(); BlockStart += 16)
    {
        uint8 Block[16];
        FMemory::Memcpy(Block, &EncryptedData[BlockStart], 16);
        
        for (int32 Round = 13; Round >= 0; --Round)
        {
            for (int32 i = 0; i < 4; ++i)
            {
                uint8 a = Block[i * 4 + 0] ^ KeyAnsi[(Round + i * 4 + 0) % 32];
                uint8 b = Block[i * 4 + 1] ^ KeyAnsi[(Round + i * 4 + 1) % 32];
                uint8 c = Block[i * 4 + 2] ^ KeyAnsi[(Round + i * 4 + 2) % 32];
                uint8 d = Block[i * 4 + 3] ^ KeyAnsi[(Round + i * 4 + 3) % 32];
                
                Block[i * 4 + 0] = a ^ b ^ c ^ d;
                Block[i * 4 + 1] = b ^ c ^ d ^ a;
                Block[i * 4 + 2] = c ^ d ^ a ^ b;
                Block[i * 4 + 3] = d ^ a ^ b ^ c;
            }
            
            for (int32 i = 0; i < 16; ++i)
            {
                uint8 KeyByte = KeyAnsi[(Round * 16 + i) % 32];
                uint8 Temp = Block[i] ^ ((Block[i] << 1) | (Block[i] >> 7));
                Block[i] = (Temp - KeyByte - Round) & 0xFF;
            }
        }
        
        FMemory::Memcpy(&DecryptedPadded[BlockStart], Block, 16);
    }

    uint8 PaddingValue = DecryptedPadded.Last();
    
    if (PaddingValue > 0 && PaddingValue <= 16)
    {
        int32 UnpaddedSize = DecryptedPadded.Num() - PaddingValue;
        
        bool bValidPadding = true;
        for (int32 i = UnpaddedSize; i < DecryptedPadded.Num(); ++i)
        {
            if (DecryptedPadded[i] != PaddingValue)
            {
                bValidPadding = false;
                break;
            }
        }
        
        if (bValidPadding)
        {
            OutDecrypted.SetNum(UnpaddedSize);
            FMemory::Memcpy(OutDecrypted.GetData(), DecryptedPadded.GetData(), UnpaddedSize);
            return true;
        }
    }

    return false;
}