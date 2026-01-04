# EzSaveGame Plugin for Unreal Engine

**Version**: 1.0.0  
**Engine Compatibility**: Unreal Engine 5.6+  
**License**: MIT (or your chosen license)

---

## Overview

EzSaveGame is a comprehensive save game plugin for Unreal Engine that provides a flexible, multi-format data persistence system. It bridges the gap between human-readable configuration files (TOML, JSON, YAML, XML) and Unreal Engine's efficient binary save game system.

### Key Features

- **Multi-Format Support**: Choose between TOML, JSON, YAML, or XML for your save files
- **Schema-Driven**: Define data structure and default values through schemas
- **Auto-Generation**: Generate schemas automatically from USaveGame classes using reflection
- **Visual Editor**: Built-in schema editor widget in Unreal Editor
- **Multiple Save Slots**: Full slot management system with metadata (date, playtime, level, etc.)
- **Data Validation**: Min/max ranges for numbers, regex patterns for strings
- **AES-256 Encryption**: Optional robust encryption for packaged games
- **Blueprint Ready**: Complete Blueprint function library with simplified API
- **Console Commands**: Debug commands for testing and development
- **Type Support**: Int, Float, Bool, String, Vector, Rotator (scalars and arrays)

---

## Quick Start

### Installation

1. Copy the `EzSaveGame` folder to your project's `Plugins` directory
2. Install required third-party libraries (see [Third-Party Libraries](#third-party-libraries))
3. Regenerate project files
4. Compile your project

### Basic Usage

**C++ Example:**
```cpp
// Get a value
int32 PlayerLevel = UMotaEzSaveGameLibrary::EzGetInt(this, TEXT("PlayerLevel"), 1);

// Set a value
UMotaEzSaveGameLibrary::EzSetInt(this, TEXT("PlayerLevel"), 50);

// Save to disk
UMotaEzSaveGameLibrary::QuickSave(this);
```

**Blueprint Example:**
```
1. Call "Ez Get Int" with Key "PlayerLevel"
2. Call "Ez Set Int" with Key "PlayerLevel" and Value 50
3. Call "Quick Save"
```

---

## Table of Contents

1. [Architecture](#architecture)
2. [Installation Guide](#installation-guide)
3. [Configuration](#configuration)
4. [Core Concepts](#core-concepts)
5. [API Reference](#api-reference)
6. [Blueprint Usage](#blueprint-usage)
7. [Advanced Features](#advanced-features)
8. [Console Commands](#console-commands)
9. [Best Practices](#best-practices)
10. [Troubleshooting](#troubleshooting)

---

## Architecture

### System Overview

```
USaveGame Class (User-Defined)
       |
       v
Schema Generator (Reflection) --> Schema File (TOML/JSON/YAML/XML)
       |                                |
       v                                v
   Runtime Data  <---------------  Config File (TOML/JSON/YAML/XML)
       |
       v
UMotaEzSaveData (In-Memory)
       |
       v
Game Logic (Blueprints/C++)
```

### Core Components

**UMotaEzConfigSubsystem**
- Game Instance Subsystem (auto-initialized)
- Loads schema and config on startup
- Manages current save data in memory
- Provides save/load to disk functionality

**UMotaEzSaveData**
- Inherits from USaveGame
- Stores all data as Key-Value pairs
- Supports scalar and array types
- Blueprint-accessible getters/setters

**UMotaEzSlotManager**
- Manages multiple save slots (0-N)
- Handles metadata (date, playtime, level, screenshots)
- Provides QuickSave/QuickLoad functionality
- Auto-save support (slot -1)

**UMotaEzSchemaGenerator**
- Inspects USaveGame classes using Unreal's reflection system
- Generates schemas automatically
- Detects types and default values from CDO

**Format Serializers**
- UMotaEzTomlSerializer (toml++)
- UMotaEzJsonSerializer (Unreal's Json module)
- UMotaEzYamlSerializer (yaml-cpp)
- UMotaEzXmlSerializer (tinyxml2)

---

## Installation Guide

### Prerequisites

- Unreal Engine 5.6 or later
- Visual Studio 2022 (Windows)
- Git (for cloning third-party libraries)

### Step 1: Add Plugin to Project

Copy the entire `EzSaveGame` folder to:
```
YourProject/Plugins/EzSaveGame/
```

### Step 2: Third-Party Libraries

The plugin requires three external libraries:

**1. toml++ (header-only)**
- Already included in `Source/ThirdParty/tomlplusplus/`
- No additional installation needed

**2. yaml-cpp**
```bash
cd YourProject/Plugins/EzSaveGame/Source/ThirdParty/
git clone https://github.com/jbeder/yaml-cpp.git
```

**3. tinyxml2**
```bash
cd YourProject/Plugins/EzSaveGame/Source/ThirdParty/
git clone https://github.com/leethomason/tinyxml2.git
```

### Step 3: Compile

1. Right-click on `YourProject.uproject`
2. Select "Generate Visual Studio project files"
3. Open the solution in Visual Studio
4. Build the project (F7)

---

## Configuration

### Project Settings

Access plugin settings via:
```
Edit > Project Settings > Plugins > EzSaveGame
```

**Format Settings**
- **File Format**: TOML / JSON / YAML / XML

**Path Settings**
- **Save Directory**: Folder inside Saved/ (default: "EzSaveGame")
- **Config File Name**: Base name for config file (default: "EzSaveConfig")
- **Schema File Name**: Base name for schema file (default: "EzSaveSchema")

**Advanced Settings**
- **Create Examples Automatically**: Generate example files on startup
- **Auto-Save on Exit**: Save automatically when game closes
- **Verbose Logging**: Detailed logs in Output Log

**Security Settings**
- **Enable Encryption**: AES-256 encryption (packaged builds only)
- **Encryption Key**: Custom key (leave empty for auto-generation)

### File Locations

With default settings, files are saved to:
```
YourProject/Saved/EzSaveGame/
    EzSaveConfig.toml (or .json, .yaml, .xml)
    EzSaveSchema.toml (or .json, .yaml, .xml)
    Slot0.toml
    Slot0_Metadata.json
    Slot1.toml
    Slot1_Metadata.json
    ...
```

---

## Core Concepts

### Schema System

A schema defines:
- Available data fields
- Data types (Int, Float, Bool, String, Vector, Rotator)
- Whether fields are scalars or arrays
- Default values
- Validation rules (optional)

**Example Schema (TOML format):**
```toml
[[Fields]]
Name = "PlayerLevel"
Type = "Int"
IsArray = false
DefaultValue = "1"
EnableValidation = true
MinValue = 1.0
MaxValue = 100.0

[[Fields]]
Name = "PlayerName"
Type = "String"
IsArray = false
DefaultValue = "Hero"

[[Fields]]
Name = "Inventory"
Type = "String"
IsArray = true
DefaultArray = ["Sword", "Shield"]
```

### Data Flow

1. **Schema Definition**: Define or generate schema
2. **Config Loading**: System loads schema + config files
3. **Data Merging**: Config values merged with schema defaults
4. **Runtime Access**: Game code reads/writes data
5. **Persistence**: Data saved back to human-readable format

### Save Slots

The plugin supports multiple save slots:
- **Numbered Slots** (0, 1, 2, ...): Regular save slots
- **Auto-Save Slot** (-1): Special slot for auto-saves
- **Metadata**: Each slot stores date, playtime, level, character data
- **QuickSave/Load**: Automatically uses last accessed slot

---

## API Reference

### UMotaEzSaveGameLibrary (Blueprint Function Library)

#### Quick Save/Load

```cpp
// Save all current data to disk
static bool QuickSave(const UObject* WorldContextObject);

// Reload all data from disk
static bool QuickLoad(const UObject* WorldContextObject);
```

#### Get Functions (Scalars)

```cpp
static int32 EzGetInt(const UObject* WorldContextObject, const FString& Key, int32 DefaultValue = 0);
static float EzGetFloat(const UObject* WorldContextObject, const FString& Key, float DefaultValue = 0.0f);
static bool EzGetBool(const UObject* WorldContextObject, const FString& Key, bool DefaultValue = false);
static FString EzGetString(const UObject* WorldContextObject, const FString& Key, const FString& DefaultValue = "");
static FVector EzGetVector(const UObject* WorldContextObject, const FString& Key, FVector DefaultValue = FVector::ZeroVector);
static FRotator EzGetRotator(const UObject* WorldContextObject, const FString& Key, FRotator DefaultValue = FRotator::ZeroRotator);
```

#### Set Functions (Scalars)

```cpp
static void EzSetInt(const UObject* WorldContextObject, const FString& Key, int32 Value);
static void EzSetFloat(const UObject* WorldContextObject, const FString& Key, float Value);
static void EzSetBool(const UObject* WorldContextObject, const FString& Key, bool Value);
static void EzSetString(const UObject* WorldContextObject, const FString& Key, const FString& Value);
static void EzSetVector(const UObject* WorldContextObject, const FString& Key, FVector Value);
static void EzSetRotator(const UObject* WorldContextObject, const FString& Key, FRotator Value);
```

#### Utilities

```cpp
static bool HasKey(const UObject* WorldContextObject, const FString& Key);
static void RemoveKey(const UObject* WorldContextObject, const FString& Key);
static void ClearAllData(const UObject* WorldContextObject);
static FString GetConfigFilePath();
static void OpenSaveFolder();
```

#### Schema Generation

```cpp
static bool GenerateSchemaFromSaveGameClass(UClass* SaveGameClass, FString& OutError);
```

### UMotaEzSlotManager (Slot Management)

#### Save/Load to Specific Slots

```cpp
static bool SaveToSlot(const UObject* WorldContextObject, int32 SlotIndex, FString& OutError);
static bool LoadFromSlot(const UObject* WorldContextObject, int32 SlotIndex, FString& OutError);
static bool DeleteSlot(int32 SlotIndex);
static bool IsSlotOccupied(int32 SlotIndex);
```

#### Slot Metadata

```cpp
static FMotaEzSlotInfo GetSlotInfo(int32 SlotIndex);
static TArray<FMotaEzSlotInfo> GetAllSlots(int32 MaxSlots = 10);
static bool UpdateSlotMetadata(int32 SlotIndex, const FMotaEzSlotInfo& NewInfo);
```

#### Quick Operations

```cpp
static bool AutoSave(const UObject* WorldContextObject);
static bool QuickSaveToSlot(const UObject* WorldContextObject, FString& OutError);
static bool QuickLoadFromSlot(const UObject* WorldContextObject, FString& OutError);
```

#### Utilities

```cpp
static bool CopySlot(int32 FromSlot, int32 ToSlot);
static int32 GetLastUsedSlot();
static int32 FindFirstEmptySlot(int32 MaxSlots = 10);
```

---

## Blueprint Usage

### Example: RPG Save System

**Step 1: Define Your Save Game Class**
```cpp
UCLASS()
class URPGSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(SaveGame) int32 PlayerLevel = 1;
    UPROPERTY(SaveGame) FString PlayerName = "Hero";
    UPROPERTY(SaveGame) FVector PlayerPosition;
    UPROPERTY(SaveGame) TArray<FString> Inventory;
};
```

**Step 2: Generate Schema (First Time Only)**
```cpp
// In your GameMode or other initialization point
void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    FString Error;
    if (!UMotaEzSchemaGenerator::GenerateAndSaveSchema(URPGSaveGame::StaticClass(), Error))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to generate schema: %s"), *Error);
    }
}
```

**Step 3: Save/Load Data in Blueprints**

Blueprint nodes available:
- **Ez Get Int** / **Ez Set Int**
- **Ez Get String** / **Ez Set String**
- **Ez Get Vector** / **Ez Set Vector**
- **Quick Save** / **Quick Load**
- **Save To Slot** / **Load From Slot**

---

## Advanced Features

### 1. Auto-Generated Schemas

Generate schemas automatically from your USaveGame classes:

```cpp
UCLASS()
class UMyGameSave : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(SaveGame) int32 Score = 0;
    UPROPERTY(SaveGame) float PlayTime = 0.0f;
    UPROPERTY(SaveGame) TArray<FString> UnlockedLevels;
};

// Generate schema
FString Error;
UMotaEzSchemaGenerator::GenerateAndSaveSchema(UMyGameSave::StaticClass(), Error);
```

The plugin will:
1. Inspect all UPROPERTY fields with SaveGame specifier
2. Detect types automatically
3. Extract default values from CDO (Class Default Object)
4. Generate a complete schema file

### 2. Visual Schema Editor

Access via:
```
Window > EzSaveGame Schema Editor
```

Features:
- Load existing schemas
- Add/remove fields visually
- Edit field properties
- Generate from USaveGame classes
- Real-time preview
- Save to any supported format

### 3. Multiple Save Slots

Create a save/load menu:

```cpp
void UMainMenu::PopulateSlotList()
{
    TArray<FMotaEzSlotInfo> Slots = UMotaEzSlotManager::GetAllSlots(10);
    
    for (const FMotaEzSlotInfo& Slot : Slots)
    {
        if (Slot.bIsOccupied)
        {
            // Display slot info:
            // - Slot.SlotName
            // - Slot.LastModified
            // - Slot.PlayTime
            // - Slot.CharacterLevel
            // - Slot.PlayerLevel
        }
        else
        {
            // Display "Empty Slot"
        }
    }

    void UMainMenu::OnSaveToSlot(int32 SlotIndex)
    {
        FString Error;
        if (UMotaEzSlotManager::SaveToSlot(this, SlotIndex, Error))
        {
            // Success
        }
        else
        {
            // Show error message
        }
    }
}
```

### 4. Data Validation

Add validation rules to your schema:

```toml
[[Fields]]
Name = "PlayerHealth"
Type = "Float"
DefaultValue = "100.0"
EnableValidation = true
MinValue = 0.0
MaxValue = 100.0
ValidationErrorMessage = "Health must be between 0 and 100"

[[Fields]]
Name = "PlayerName"
Type = "String"
DefaultValue = "Player"
EnableValidation = true
RegexPattern = "^[A-Za-z0-9_]{3,20}$"
ValidationErrorMessage = "Name must be 3-20 alphanumeric characters"
```

Validate programmatically:
```cpp
FMotaEzValidationResult Result = UMotaEzValidator::ValidateAllData(Data, Schema);
if (!Result.bIsValid)
{
    UE_LOG(LogTemp, Error, TEXT("Validation failed: %s - %s"), 
           *Result.FieldName, *Result.ErrorMessage);
}
```

### 5. AES-256 Encryption

Enable encryption in Project Settings > Security.

**Behavior:**
- **In Editor**: Always disabled (for easy debugging)
- **In Packaged Game**: Enabled if checkbox is checked

**Configuration:**
```
Enable Encryption: [X]
Encryption Key: [leave empty for auto-generation]
```

The plugin will:
- Generate a unique key based on project + machine ID
- Encrypt all save files using AES-256
- Handle encryption/decryption automatically
- Keep files human-readable in editor

**Use Cases:**
- **Development**: Encryption OFF (easy file inspection)
- **Beta Testing**: Encryption OFF (testers can modify stats)
- **Production**: Encryption ON (prevent cheating)

---

## Console Commands

Available console commands (press ` key in-game):

```
EzSave.Save        - Save all data to disk
EzSave.Load        - Reload all data from disk
EzSave.GetInt <Key>      - Get integer value
EzSave.SetInt <Key> <Val> - Set integer value
EzSave.GetString <Key>    - Get string value
EzSave.Info        - Display current configuration
```

**Example Usage:**
```
> EzSave.SetInt PlayerLevel 99
> EzSave.Save
> EzSave.Info
```

---

## Best Practices

### 1. Schema First Development

Always create or generate a schema before using the save system:
```cpp
// First time setup
UMotaEzSchemaGenerator::GenerateAndSaveSchema(UMySaveGame::StaticClass(), Error);
```

### 2. Use Descriptive Keys

```cpp
// Good
EzSetInt("Player_Level", 50);
EzSetString("Player_Name", "Hero");

// Avoid
EzSetInt("pl", 50);
EzSetString("n", "Hero");
```

### 3. Initialize Defaults

Always provide default values in your schema or in code:
```cpp
int32 Level = EzGetInt("PlayerLevel", 1);  // Default to 1 if not found
```

### 4. Save Frequency

- **Don't save every frame** - Too expensive
- **Save on important events**: Level completion, checkpoint reached, etc.
- **Auto-save periodically**: Every 5-10 minutes
- **Save on exit**: Ensure no progress is lost

Example auto-save:
```cpp
GetWorld()->GetTimerManager().SetTimer(
    AutoSaveTimer,
    [this]() { UMotaEzSlotManager::AutoSave(this); },
    300.0f,  // 5 minutes
    true
);
```

### 5. Error Handling

Always check return values:
```cpp
FString Error;
if (!UMotaEzSlotManager::SaveToSlot(this, SlotIndex, Error))
{
    UE_LOG(LogTemp, Error, TEXT("Save failed: %s"), *Error);
    // Show error message to player
}
```

### 6. Slot Management

Implement a robust save menu:
- Show all available slots
- Display metadata (date, time, level)
- Allow slot deletion
- Confirm overwrite operations

---

## Troubleshooting

### Common Issues

**Issue: "Failed to load schema"**
- **Cause**: Schema file doesn't exist
- **Solution**: Generate schema using `GenerateAndSaveSchema()`

**Issue: "Failed to parse config"**
- **Cause**: Malformed config file
- **Solution**: Delete config file and let system recreate it

**Issue: "GetInt returns default value"**
- **Cause**: Key doesn't exist in data
- **Solution**: Use `SetInt()` to create the key first, or check schema

**Issue: "BOM detected in file"**
- **Cause**: File saved with UTF-8 BOM
- **Solution**: All plugin files use UTF-8 without BOM

**Issue: "Encryption not working in Editor"**
- **Cause**: By design - encryption is disabled in editor
- **Solution**: Test encryption in packaged build

### Debug Commands

Check current state:
```
EzSave.Info
```

Inspect saved files:
```cpp
FString Path = UMotaEzSaveGameLibrary::GetConfigFilePath();
UE_LOG(LogTemp, Log, TEXT("Config file: %s"), *Path);
```

Open save folder:
```cpp
UMotaEzSaveGameLibrary::OpenSaveFolder();
```

---

## Performance Considerations

- **File I/O**: Performed on game thread (consider async for large files)
- **Memory Usage**: All data kept in memory (suitable for small-medium sized saves)
- **Serialization**: TOML/JSON parsing is fast for typical save sizes (<1MB)
- **Encryption**: Minimal overhead (AES-256 is hardware-accelerated on modern CPUs)

**Optimization Tips:**
1. Keep save files small (< 1MB recommended)
2. Save only essential data
3. Use arrays sparingly
4. Don't call save operations in Tick()

---

## License

MIT License (requires attribution)

See the [`LICENSE`](LICENSE) file for full terms and attribution requirements.

---

## Support

For issues, feature requests, or contributions:
- GitHub: [your-repository-url]
- Email: [your-support-email]
- Discord: [your-discord-server]

---

## Author & Credits

EzSaveGame is authored and maintained by José Antonio Mota Lucas.

- Author: José Antonio Mota Lucas
- LinkedIn: https://www.linkedin.com/in/joseantoniomotalucas/

### Third-Party Libraries
We rely on the following excellent open-source projects for serialization support:

- tinyxml2 — XML parser (https://github.com/leethomason/tinyxml2) — see `Source/ThirdParty/tinyxml2/LICENSE.txt`
- yaml-cpp — YAML parser/emitter (https://github.com/jbeder/yaml-cpp) — see `Source/ThirdParty/yaml-cpp/LICENSE`
- tomlplusplus (toml++) — TOML parser (https://github.com/marzer/tomlplusplus) — see `Source/ThirdParty/tomlplusplus/toml.hpp`

Please respect their respective licenses when redistributing or modifying the plugin.

---

## Changelog

### Version 1.0.0 (2025-11-27)
- Initial release
- Multi-format support (TOML, JSON, YAML, XML)
- Auto-schema generation
- Visual schema editor
- Multiple save slots
- Data validation
- AES-256 encryption
- Complete Blueprint API
- Console commands

---

**For a comprehensive usage guide with detailed examples, see [USER_GUIDE.md](USER_GUIDE.md)**
