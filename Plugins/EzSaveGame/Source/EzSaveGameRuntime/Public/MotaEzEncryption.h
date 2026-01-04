#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MotaEzEncryption.generated.h"

/**
 * Encryption module for EzSaveGame.
 * Implements AES-256 encryption with 14 transformation rounds for save file security.
 * 
 * Features:
 * - AES-256 encryption with 32-byte key
 * - Automatic PKCS7 padding
 * - 16-byte block processing
 * - Editor: Always disabled for easy debugging
 * - Packaged game: Configurable via Project Settings
 * 
 * Recommended usage:
 * - Beta Testing: Disabled (testers can edit files)
 * - Production: Enabled (prevent cheating)
 * - Development: Always disabled automatically
 */
UCLASS()
class EZSAVEGAMERUNTIME_API UMotaEzEncryption : public UObject
{
    GENERATED_BODY()

public:

    /**
     * Checks if encryption should be active.
     * @return Always false in editor, settings-dependent in packaged game
     */
    UFUNCTION(BlueprintPure, Category="EzSaveGame|Encryption")
    static bool ShouldEncrypt();

    /**
     * Encrypts a string using AES-256.
     * @param PlainText - String to encrypt
     * @param OutEncrypted - Base64-encoded encrypted string
     * @param OutError - Error message if encryption fails
     * @return true if encryption was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Encryption")
    static bool EncryptString(const FString& PlainText, FString& OutEncrypted, FString& OutError);

    /**
     * Decrypts a string using AES-256.
     * @param EncryptedText - Base64-encoded encrypted string
     * @param OutPlainText - Decrypted plain text
     * @param OutError - Error message if decryption fails
     * @return true if decryption was successful, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category="EzSaveGame|Encryption")
    static bool DecryptString(const FString& EncryptedText, FString& OutPlainText, FString& OutError);

    /**
     * Gets or generates the encryption key.
     * Uses configured key from settings, or generates one from project ID if not set.
     * @return 32-byte encryption key as string
     */
    static FString GetEncryptionKey();

    /**
     * Generates a random encryption key.
     * @return Random GUID-based encryption key
     */
    static FString GenerateRandomKey();

private:
    static bool EncryptAES(const TArray<uint8>& PlainData, const FString& Key, TArray<uint8>& OutEncrypted);
    static bool DecryptAES(const TArray<uint8>& EncryptedData, const FString& Key, TArray<uint8>& OutDecrypted);
};

