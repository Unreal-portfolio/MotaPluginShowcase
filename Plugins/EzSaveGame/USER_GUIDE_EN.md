﻿﻿# COMPLETE USER GUIDE - EzSaveGame Plugin
Version: 1.0.0
Engine: Unreal Engine 5.6+
Date: 4 December 2025
---
## TABLE OF CONTENTS
1. [INTRODUCTION](#1-introduction)
2. [SYSTEM ARCHITECTURE](#2-system-architecture)
3. [INSTALLATION AND CONFIGURATION](#3-installation-and-configuration)
4. [FUNDAMENTAL CONCEPTS](#4-fundamental-concepts)
5. [COMPLETE API REFERENCE](#5-complete-api-reference)
6. [BLUEPRINT GUIDE](#6-blueprint-guide)
7. [C++ GUIDE](#7-c-guide)
8. [SCHEMA SYSTEM](#8-schema-system)
9. [SLOT SYSTEM](#9-slot-system)
10. [DATA VALIDATION](#10-data-validation)
11. [ENCRYPTION](#11-encryption)
12. [FILE FORMATS](#12-file-formats)
13. [PRACTICAL EXAMPLES](#13-practical-examples)
14. [USE CASES](#14-use-cases)
15. [OPTIMIZATION AND PERFORMANCE](#15-optimization-and-performance)
16. [CONSOLE COMMANDS](#16-console-commands)
17. [MIGRATION AND UPGRADES](#17-migration-and-upgrades)
18. [TROUBLESHOOTING](#18-troubleshooting)
19. [BEST PRACTICES](#19-best-practices)
20. [FAQ](#20-faq)
---
## 1. INTRODUCTION
### 1.1 What is EzSaveGame
EzSaveGame is a comprehensive data persistence plugin for Unreal Engine designed to provide reliable, flexible and secure save/load capabilities for games and interactive experiences. It combines human-readable configuration formats (TOML, JSON, YAML, XML) with Unreal Engine's native save infrastructure to deliver a workflow suitable for designers and programmers alike.

### 1.2 Design Philosophy
EzSaveGame is built around these core principles:
- Ease of Use: A minimal and consistent API available for Blueprints and C++.
- Flexibility: Support for multiple serialization formats and swappable storage backends.
- Security: Optional AES-256 encryption and HMAC integrity checks for production builds.
- Scalability: Slot-based save system enabling multiple save states and per-user policies.
- Validation: Schema-first validation to ensure data integrity and facilitate safe migrations.
- Debuggability: Editor-visible, human-readable artifacts and clear diagnostics for rapid troubleshooting.

### 1.3 Key Features
- Multi-format serialization: TOML, JSON, YAML, XML.
- Automatic schema generation from `USaveGame`-derived classes.
- Integrated visual schema editor in the Unreal Editor.
- Slot management with metadata (timestamp, session playtime, level, custom tags).
- Validation rules: required fields, min/max, regex patterns and custom validators.
- Optional AES-256 encryption pipeline and HMAC verification.
- Full Blueprint nodes and C++ API surface.
- Support for primitives, engine types (Vector, Rotator), arrays and nested structures.
- Auto-save, QuickSave/QuickLoad, backups and atomic writes.
- Console commands for debugging and automation.

### 1.4 Audience and Scope
This guide targets game developers, technical designers and engineers using Unreal Engine 5.6 or later who need a robust, extensible save/load system. It covers:
- Installation and configuration for editor and runtime.
- Both Blueprint and C++ usage patterns with code samples.
- The schema system and validation rules.
- Encryption and security considerations.
- Troubleshooting, optimization and migration strategies.

---

## 2. SYSTEM ARCHITECTURE
### 2.1 Overview
EzSaveGame is organized as a set of modular layers that separate responsibilities for application integration, data serialization and physical storage. This separation makes the plugin easy to extend, test and maintain while providing clear extension points for custom serialization formats, storage backends and validation logic.

The architecture is intentionally simple and follows these layers:
- Application Layer: public API consumed by gameplay code and Blueprints.
- Serialization Layer: format-specific adapters that convert in-memory objects to human-readable representations and back.
- Storage Layer: providers that handle physical persistence and platform-specific I/O.

### 2.2 Application Layer (Core Responsibilities)
The Application Layer exposes the high-level API used by game code and designers. Its responsibilities include:
- Managing named save slots and associated metadata (timestamp, playtime, custom tags).
- Exposing synchronous and asynchronous save/load operations for Blueprints and C++.
- Coordinating schema generation, validation and migration before persisting or after loading data.
- Raising events and callbacks (success/failure, progress, validation reports).

Key components:
- Save Manager: central service responsible for save/load orchestration and slot lifecycle.
- Slot Manager: thin helper that tracks per-slot metadata and retention policies.
- Config Subsystem: runtime settings, default options and runtime registration hooks.

### 2.3 Serialization Layer
The Serialization Layer contains adapters that translate in-memory objects into a format (JSON, TOML, YAML, XML) and back again. Each adapter implements a common interface that guarantees:
- Deterministic serialization order where necessary for stable diffs.
- Respect for schema constraints and version metadata.
- Clear error reporting on malformed payloads or missing required fields.

Responsibilities:
- Marshalling engine types (FVector, FRotator, FTransform), arrays and nested structures.
- Emitting and consuming schema metadata (version, field descriptors, custom attributes).
- Providing extension points for custom type serializers (e.g., game-specific structs).

Adapters provided by default:
- JSONAdapter
- TOMLAdapter
- YAMLAdapter
- XMLAdapter

Each adapter is implemented as a pluggable module. Third-party or project-specific adapters can be registered at startup via the Config Subsystem.

### 2.4 Storage Layer
The Storage Layer abstracts the physical persistence mechanism and ensures operations are safe and atomic.

Responsibilities:
- Performing atomic writes (write to temporary file then rename) to avoid partial saves.
- Implementing platform-aware paths and permission checks (per-user directories, platform sandboxes).
- Optional compression and encryption hooks invoked before writing and after reading.
- Maintaining a small set of built-in providers: LocalFilesystemProvider, InMemoryProvider (for tests), and an optional CloudProvider interface for platform services.

Providers expose a simple contract: Save(name, payload, options) and Load(name) -> payload. They are responsible for low-level retries, file locking policies and basic diagnostics.

### 2.5 Component Flow
Typical operation flow:
1. Game code calls SaveManager->SaveSlot(slotName, UObject* payload, options).
2. Application Layer validates the payload against the active schema and runs migration hooks if necessary.
3. The Serialization Layer adapter converts the in-memory payload to the chosen format and attaches schema/version metadata.
4. If encryption is enabled, the Encryption pipeline transforms the serialized bytes.
5. The Storage Provider performs an atomic write to disk or cloud.
6. On success/failure the SaveManager raises callbacks and updates slot metadata.

Load flow is the inverse: Storage Provider -> Decrypt -> Deserialize -> Validate -> Migrate -> Deliver to caller.

### 2.6 Versioning and Migration
Every saved artifact includes metadata with a schema identifier and version. EzSaveGame supports:
- Automatic schema version checks during load.
- Developer-defined migration callbacks or transformations to upgrade older saves to the current schema.
- Fallback strategies when migration fails (automatic backup, read-only legacy mode and clear diagnostic messages).

Migration hooks are located in the Schema Generator module and can be generated automatically for common changes or implemented by hand for complex transforms.

### 2.7 Security Model
Security is opt-in and designed to protect packaged builds and user data. Core ideas:
- Encryption pipeline (recommended AES-256-GCM) with optional HMAC signing for tamper detection.
- Key provisioning via a secure provider interface that can read from platform keystores or injected secrets at runtime.
- Editor mode defaults to non-production keys with explicit warnings; production builds should use real provisioning.

### 2.8 Extensibility Points
EzSaveGame exposes multiple extension points:
- Adapter registration: register custom serializers for new formats.
- Provider plugins: implement new storage backends (cloud, platform SDKs).
- Custom validators: add domain-specific validation rules invoked in the Application Layer.
- Migration handlers: register version upgrade handlers for complex schema changes.

### 2.9 Module Layout (high-level)
The codebase is organized into two runtime modules and one editor module:
- EzSaveGameRuntime: runtime serialization, storage, schema generator and core APIs.
- EzSaveGameEditor: editor-only widgets, schema editor and tooling.
- ThirdParty: included libraries (tinyxml2, yaml-cpp, tomlplusplus) used by adapters.

---

## 3. INSTALLATION AND CONFIGURATION
### 3.1 Prerequisites
Before installing EzSaveGame, ensure your development environment meets the following requirements:
- Unreal Engine 5.6 or later.
- Visual Studio 2019/2022 (or equivalent toolchain) for compiling editor/runtime modules on Windows.
- Optional: platform SDKs for cloud or platform-specific storage targets.
- Optional (encryption): OpenSSL or platform crypto APIs if you plan to build custom encryption providers.

### 3.2 Editor Installation (Project-local)
To add EzSaveGame to a project:
1. Copy the `EzSaveGame` plugin folder into your project's `Plugins/` directory: `YourProject/Plugins/EzSaveGame`.
2. Open the project in the Unreal Editor. The editor will detect the new plugin and may compile modules on first load.
3. Open `Edit -> Plugins` and locate `EzSaveGame` (category: Utilities). Enable the plugin if it is not enabled.
4. Restart the editor when prompted to complete module initialization.

Notes:
- If using source control, add the plugin folder to source control after validating it in your project.
- For engine-wide installation (all projects), install the plugin into the Engine's `Plugins/` directory instead, but prefer project-local installations for project-specific behavior and versioning.

### 3.3 Binary vs. Source Distribution
- Binary distribution: contains precompiled modules in `Binaries/` and is quicker to drop into a project. Use this when engine/visual studio toolchain differences might complicate a local build.
- Source distribution: includes the `Source/` directory and requires compilation on first load but allows modification of plugin code.

### 3.4 Plugin Settings (Project Settings)
EzSaveGame exposes configurable options in the editor under `Edit -> Project Settings -> EzSaveGame` or a dedicated settings asset. Key settings include:
- Default Serialization Format: JSON | TOML | YAML | XML.
- Default Save Path Policy: Per-User | Per-Install | Custom Path.
- Auto-Save Interval (seconds): 0 = disabled.
- Auto-Save on Exit: Automatically saves when the game closes.
- QuickSave Slot Name: default slot used by QuickSave nodes.
- Encryption: Disabled | Editor (debug key) | Production (key provider required).
- Backup Retention Count: number of automatic backups to keep per slot.

**Auto-Save on Exit behavior:**
- When enabled, the system automatically saves all current data when the game is closing.
- Uses the subsystem's `SaveToDisk()` method to persist data synchronously before shutdown.
- **Important limitations:**
  - In PIE (Play In Editor): May not trigger if the editor stops the game abruptly (Ctrl+Alt+F11 or forced stop).
  - In packaged builds: Works reliably when the player closes the game normally (Quit button, window X).
  - Does NOT work: On crashes, task manager kills, or power loss.
- **Recommended usage:** Enable as a safety net, but always provide manual save options for players.
- For development testing, implement manual save in `EndPlay()` to ensure data is saved when stopping PIE.

Example recommended settings for development:
- Default Serialization Format: JSON
- Encryption: Disabled or Editor
- Auto-Save Interval: 300 (5 minutes)
- Auto-Save on Exit: Enabled (with manual save backup in code)

Example recommended settings for shipping builds:
- Default Serialization Format: TOML or JSON (whichever your toolchain uses)
- Encryption: Production (use a keyed provider)
- Auto-Save Interval: tuned per game requirements
- Auto-Save on Exit: Enabled (as safety net)

#### 3.4.1 Utility Buttons in Project Settings
The EzSaveGame settings panel in the Unreal Editor includes utility buttons for common development tasks. These buttons are located under `Edit -> Project Settings -> Plugins -> EzSaveGame` in the **Utilities** category:

**Open Save Folder**
- Opens the save game directory in the system file explorer (Windows Explorer, Finder, etc.).
- Useful for quickly inspecting, backing up or manually editing save files during development.
- Creates the directory if it doesn't exist yet.

**Delete All Save Data**
- Permanently deletes ALL save data including:
  - All save slots (Slot0, Slot1, ..., AutoSave)
  - Slot metadata files
  - Screenshot files associated with slots
  - Configuration files (EzSaveConfig.*)
  - Schema files (EzSaveSchema.*)
- **WARNING**: This action cannot be undone!
- A confirmation dialog will appear before deletion, requiring explicit user confirmation.
- The dialog clearly lists what will be deleted and warns that the operation is irreversible.
- After successful deletion, a confirmation message displays the number of files deleted.
- If some files cannot be deleted (e.g., due to permissions), an error message will be shown and details will be logged to the Output Log.

**Usage recommendations**:
- Use "Open Save Folder" frequently during development to verify save file contents and structure.
- Use "Delete All Save Data" when:
  - Resetting the project to a clean state for testing.
  - Clearing corrupt or invalid save files.
  - Testing first-time user experience without existing saves.
  - Preparing for a fresh playthrough or demo.
- Always backup important saves before using "Delete All Save Data" if you need to preserve them.
- These buttons are editor-only and do not appear in packaged builds.

**Safety features**:
- The delete operation requires explicit confirmation through a modal dialog.
- The confirmation dialog uses clear warning text and orange/red color indicators.
- All deletion operations are logged to the Output Log for audit purposes.
- Failed deletions are reported with specific error messages indicating which files could not be removed.

### 3.5 Runtime Configuration
Runtime configuration can be done through C++ code, Blueprints or configuration files. Typical runtime setup steps:
- Register the secure key provider early in startup (for example, in GameInstance::Init or a custom subsystem).
- Set the active serialization adapter if your project requires a non-default format.
- Configure paths and retention policies programmatically when the user profile or platform demands it.

C++ example (conceptual):
- In GameInstance::Init:
  - Initialize EzSaveGame subsystem
  - EzSave::RegisterKeyProvider(MakeShared<MyKeyProvider>());
  - EzSave::SetDefaultFormat(EEzFormat::JSON);

Blueprint example:
- Use the `EzSave Init` node provided in the plugin to set format and basic options on game start.

### 3.6 Key Provisioning and Encryption
If enabling encryption in production, follow these guidelines:
- Implement a secure key provider that implements the plugin's key provider interface. The provider should load keys from a secure location (platform keystore, environment variables injected by your CI/CD, or a secrets manager).
- Do not hard-code production keys in the source or shipping builds.
- Prefer platform-native keystores (Keychain on macOS, DPAPI/Windows Credential Store on Windows, Android Keystore, iOS Keychain) for secure storage.
- If using AES-256-GCM, ensure the provider can return both key material and an identifier used for key rotation and validation.

Editor mode: the plugin ships with a debug key provider suitable for development only. The editor provider logs explicit warnings when used in packaged builds.

### 3.7 Packaging and Platform Notes
- When packaging for consoles or closed platforms, verify save paths and sandboxing rules for each platform. Use the Storage Provider appropriate to the platform.
- Cloud providers often require platform SDK initialization (e.g., PlayStation/Live/Azure). Register and initialize those SDKs prior to EzSaveGame usage.
- On Windows, ensure the chosen save path has write permissions for the user. For per-user saves, use `%LOCALAPPDATA%/YourCompany/YourGame/Saves`.

### 3.8 Upgrading the Plugin
When upgrading EzSaveGame to a new plugin version:
1. Read the release notes for breaking changes.
2. Backup your existing saves (copy the saves folder to a safe location).
3. Update the plugin folder in `Plugins/` and restart the editor.
4. Run the validation utility included with the plugin to scan existing saves for schema/version mismatches.

### 3.9 Troubleshooting Installation Issues
- The editor fails to compile the plugin modules: confirm your Visual Studio toolchain and Unreal Engine version match the plugin's expected compiler settings.
- Plugin not visible in the Plugins browser: ensure the plugin folder contains a valid `.uplugin` file and that the path is `Project/Plugins/EzSaveGame`.
- Permissions errors when saving during play-in-editor: check that the path configured for saves is writable by the editor process.

---

## 4. FUNDAMENTAL CONCEPTS
This section defines the core concepts used throughout EzSaveGame and clarifies the terminology used in the rest of this guide.

### 4.1 Slot
A Slot is a named container representing one save state. Each slot stores:
- Payload: the serialized game data.
- Metadata: human-readable properties such as timestamp, playtime, level name, version and custom tags.
- Backups: zero or more historical copies kept by automatic or manual backup policies.

Common operations: Create, Save, Load, Delete, Enumerate, and Backup/Restore.

Example slot metadata (conceptual):
- Name: "Player_01"
- Timestamp: 2025-11-28T12:34:56Z
- SchemaVersion: 1.2.0
- PlaytimeSeconds: 4567
- LevelName: "MainLevel"
- CustomTags: {"difficulty":"hard","campaign":"alpha"}

### 4.2 Schema
A Schema is a machine-readable contract describing the shape and validation rules of saved data. Schemas enable:
- Validation during save and load to prevent corrupt or incompatible data.
- Automatic UI generation in the editor (schema editor).
- Deterministic migration paths between versions.

Schemas typically include field definitions (name, type, required flag), constraints (min/max, regex), and metadata (version, description).

### 4.3 Adapter
An Adapter is a serialization implementation for a specific format (JSON, TOML, YAML, XML). Responsibilities:
- Serialize/deserialize payloads while preserving schema metadata.
- Provide stable formatting options for readable diffs.
- Expose clear error messages when deserialization fails.

### 4.4 Provider (Storage Provider)
A Provider is responsible for physical persistence. Examples:
- LocalFilesystemProvider: saves files to disk.
- InMemoryProvider: keeps saves in memory for testing.
- CloudProvider: an interface for cloud or platform-backed storage.

Providers implement a minimal contract (Save, Load, Delete, List) and handle platform specifics like path resolution, atomic writes and permission checks.

### 4.5 Save Manager
The Save Manager orchestrates the save/load lifecycle. It coordinates schema validation, serialization, encryption, and storage. It also exposes Blueprint nodes and C++ APIs for common tasks such as QuickSave, QuickLoad, and enumerating slots.

### 4.6 Slot Manager
A helper responsible for maintaining slot metadata, retention policies (how many backups to keep), and scheduling background maintenance (cleanup, compaction).

### 4.7 Schema Generator
A utility that inspects `USaveGame`-derived classes (or annotated structures) and produces an initial schema. The generator attempts to map common Unreal types to schema types and emits validation stubs which the designer can refine in the schema editor.

### 4.8 Validation
Validation ensures payloads conform to the active schema. Validation phases:
- Pre-save validation: stop invalid data from being written.
- Post-load validation: detect and report schema mismatches or missing fields.

Validators can be: built-in (required, min/max, pattern), or custom (project-specific rules implemented via a plugin interface).

### 4.9 Migration
Migration transforms legacy payloads to the current schema version. Migration strategies:
- Automatic: simple renames, default value injection, type widening (int -> float).
- Custom: developer-provided migration functions for complex transforms.

Migration runs during load and may produce warnings, errors, or automated fixes depending on configuration.

### 4.10 Metadata and Versioning
Every save includes metadata: schema ID, schema version, serialization format, creation timestamp, and optional provider-specific tags (e.g., cloud sync ids). Use metadata to implement compatibility checks, display save thumbnails in UI, and drive migration decisions.

### 4.11 Atomic Writes and Backups
EzSaveGame uses atomic write patterns (write-then-rename) to reduce the risk of partial or corrupted saves. Additionally, it can keep a configurable number of backups per slot to allow rollback in case of corruption.

### 4.12 QuickSave / QuickLoad and Auto-Save
- QuickSave/QuickLoad: single-action operations bound to a configured default slot intended for fast user-driven saves.
- Auto-Save: periodic saves driven by an interval; designed to be non-blocking and yield-friendly. Auto-save respects validation and backup policies.

### 4.13 Limitations and Best Practices (short)
- Avoid storing large binary blobs in the primary save payload; prefer external assets or references.
- Keep save frequency reasonable to reduce I/O pressure on the storage provider.
- Always test migrations in integration environments before shipping updates that change schema versions.

---

## 5. COMPLETE API REFERENCE
This section documents the public API surface provided by EzSaveGame. It covers both the C++ runtime API and Blueprint-exposed nodes, describes asynchronous behaviour, error handling, and includes concise usage examples.

### 5.1 Overview
The API is organized around a small set of core types and subsystems:
- Save Manager (runtime service)
- Slot Manager (metadata and retention)
- Serialization Adapters (format implementations)
- Storage Providers (persistence backends)
- Schema Generator & Validators
- Key Provider (encryption)

The plugin exposes a combination of UObject-based subsystems, static helper functions and Blueprint library nodes. Most long-running operations provide asynchronous variants with progress callbacks.

### 5.2 Naming and Conventions
- C++ classes follow the `UEz` or `MotaEz` prefix patterns used in the codebase.
- Blueprint nodes are placed under the `EzSave` category and use clear verbs: `EzSave_SaveSlot`, `EzSave_LoadSlot`, `EzSave_Init`.
- Error handling: functions that can fail either return a boolean + out-parameter for details, or accept a delegate/callback that provides a `FEzSaveResult` struct including success flag and error code.

### 5.3 Core C++ Types (summary)
- UEzSaveManager (UObject subsystem)
  - SaveSlot(FName SlotName, UObject* Payload, const FEzSaveOptions& Options) -> bool
  - SaveSlotAsync(FName SlotName, UObject* Payload, const FEzSaveOptions& Options, FOnEzSaveComplete Delegate)
  - LoadSlot(FName SlotName, UObject* OutTarget, const FEzLoadOptions& Options) -> bool
  - LoadSlotAsync(FName SlotName, UObject* OutTarget, const FEzLoadOptions& Options, FOnEzLoadComplete Delegate)
  - EnumerateSlots(TArray<FEzSlotInfo>& OutSlots) -> void
  - DeleteSlot(FName SlotName) -> bool

- FEzSaveOptions (struct)
  - FormatOverride (optional) : EEzFormat
  - bEncrypt : bool
  - bCreateBackup : bool
  - MetadataOverrides: map<string,string>

- FEzLoadOptions (struct)
  - bValidate : bool
  - bRunMigrations : bool
  - DesiredFormat : optional

- FEzSlotInfo (struct)
  - SlotName : FName
  - Timestamp : FDateTime
  - SchemaVersion : FString
  - PlaytimeSeconds : int64
  - CustomTags : TMap<FString,FString>

- IEzStorageProvider (interface)
  - Save(const FString& Key, const TArray<uint8>& Payload, const FEzStorageOptions& Options)
  - Load(const FString& Key, TArray<uint8>& OutPayload)
  - Delete(const FString& Key)
  - List(const FString& Prefix, TArray<FString>& OutKeys)

- IEzSerializationAdapter (interface)
  - Serialize(UObject* Source, TArray<uint8>& OutBytes, FEzSerializationResult& OutResult)
  - Deserialize(const TArray<uint8>& Bytes, UObject* Target, FEzSerializationResult& OutResult)

- IEzKeyProvider (interface)
  - GetKey(FString& OutKeyId, TArray<uint8>& OutKeyMaterial)
  - RotateKey(...)

### 5.4 Blueprint API
The plugin exposes a Blueprint library with nodes grouped under `EzSave`:
- EzSave Init
  - Inputs: DefaultFormat (enum), EnableEncryption (bool)
  - Returns: success (bool)
- EzSave Save Slot (Sync)
  - Inputs: SlotName (string), SaveObject (object), Options (struct)
  - Outputs: Success (bool), ErrorMessage (string)
- EzSave Save Slot (Async)
  - Inputs: SlotName, SaveObject, Options
  - Outputs: Completed (event), Progress (float), Result (struct)
- EzSave Load Slot (Sync / Async)
- EzSave Enumerate Slots
- EzSave Delete Slot
- EzSave Create Backup / Restore Backup

Blueprint nodes accept the same options structs (exposed as Blueprintable) and return a FEzSaveResult-style struct that includes an error code and human-readable message.

### 5.5 Async Patterns and Delegates
Long-running operations provide two integration patterns:
- Delegate-based: pass a delegate (FOnEzSaveComplete) that receives a FEzSaveResult once complete.
- Latent/Node-based: Blueprint async nodes that use latent pins or async events to resume execution.

Progress reporting: async save/load nodes provide an optional progress delegate with a float [0..1] representing I/O + serialization progress.

### 5.6 Events and Notifications
- OnSaveCompleted(FName SlotName, FEzSaveResult Result)
- OnLoadCompleted(FName SlotName, FEzLoadResult Result)
- OnValidationReport(FName SlotName, FEzValidationReport Report)
- OnProviderError(FName ProviderName, FEzProviderError Error)

These events can be bound in C++ or Blueprints. Some are exposed via the Save Manager singleton for convenience.

### 5.7 Error Codes and Diagnostics
FEzErrorCode enumeration (examples):
- None
- InvalidPayload
- SchemaMismatch
- MigrationFailed
- SerializationError
- ProviderWriteFailed
- ProviderReadFailed
- DecryptionFailed
- PermissionDenied
- Unknown

FEzSaveResult contains:
- bool bSuccess
- FEzErrorCode ErrorCode
- FString Message
- TArray<FEzValidationProblem> Problems (optional)

Logging: the plugin uses UE_LOG with category `LogEzSave` and includes detailed context for failures (slot name, provider, file path, schema version).

### 5.8 Examples
C++ synchronous save (conceptual):
- UEzSaveManager* Manager = GetGameInstance()->GetSubsystem<UEzSaveManager>();
- FEzSaveOptions Options; Options.bEncrypt = false; Options.bCreateBackup = true;
- bool bOk = Manager->SaveSlot(TEXT("Player_01"), MySaveGameObject, Options);

C++ asynchronous save (conceptual):
- Manager->SaveSlotAsync(TEXT("Player_01"), MySaveGameObject, Options, FOnEzSaveComplete::CreateLambda([](const FEzSaveResult& Result){ if(Result.bSuccess) { /*...*/ } }));

Blueprint quick example:
- Node: EzSave Save Slot (Async)
  - Connect SaveObject and SlotName
  - Bind Completed event: check Result.Success pin and show UI message

### 5.9 Extending the API
To add a new storage provider:
1. Implement `IEzStorageProvider` in a new module.
2. Register the provider during startup via `EzSave::RegisterStorageProvider("MyProvider", MakeShared<FMyProvider>())` or through the plugin settings.
3. Configure the provider in Project Settings and select it at runtime.

To add a custom serializer for a new format:
1. Implement `IEzSerializationAdapter`.
2. Register adapter with `EzSave::RegisterAdapter(EEzFormat::MyFormat, MakeShared<FMyAdapter>())`.

### 5.10 Backwards Compatibility and Deprecations
Public API changes follow semantic versioning. Deprecated methods remain available for at least one minor release with `UE_DEPRECATED` annotations and clear migration notes in the changelog.

---

## 6. BLUEPRINT GUIDE
This section explains how to use EzSaveGame from Blueprints. It covers setup, the available nodes, typical patterns (QuickSave/AutoSave), error handling, debugging tips and recommended best practices for Blueprints-first projects.

### 6.1 Getting Started (Initialization)
- Place an initialization call early in your game startup (recommended: GameInstance -> Event Init).
- Use the `EzSave Init` node to set the default serialization format and enable/disable encryption.
- If you rely on a custom key provider or storage provider, register or configure it in C++ before calling Blueprint init, or expose a thin Blueprint wrapper that calls into your C++ initialization.

Recommended pattern:
1. In `GameInstance::Event Init` call `EzSave Init` with the project's defaults.
2. Optionally read player preferences (auto-save enabled, save slot name) and set runtime options via the exposed Blueprint nodes.

### 6.2 Core Blueprint Nodes (summary)
All nodes are grouped under the `EzSave` category in the Palette.
- EzSave Init
- EzSave Save Slot (Sync)
- EzSave Save Slot (Async)
- EzSave Load Slot (Sync)
- EzSave Load Slot (Async)
- EzSave Enumerate Slots
- EzSave Delete Slot
- EzSave Create Backup
- EzSave Restore Backup
- EzSave Validate Slot

Nodes accept/return the equivalent options and result structs exposed to Blueprints (e.g., FEzSaveOptions_BP, FEzSaveResult_BP).

### 6.3 Saving from Blueprints
Quick example (Async):
1. Hook a UI button `OnClicked` to start a save.
2. Call `EzSave Save Slot (Async)` with:
   - SlotName: "Player_01" (use a variable or setting)
   - SaveObject: reference to the `SaveGame`/data object (or an exposed UObject that implements the save contract)
   - Options: set `CreateBackup=true` for safety
3. Bind the `Completed` event; check the Result.Success pin and show feedback to the player.
4. Optionally bind `Progress` to a loading indicator.

Notes:
- Prefer the Async node for UI-thread safety. The Sync node can block the game thread and should be used only for small payloads or in non-interactive contexts.
- If you need deterministic ordering (multiple saves in a row), await the `Completed` delegate before starting the next save.

### 6.4 Loading from Blueprints
Quick example (Async):
1. Call `EzSave Load Slot (Async)` with the SlotName and a target `SaveObject` (or let the node create and return an object depending on node variant).
2. Bind the `Completed` event and inspect Result.Success and any validation issues provided in the result struct.
3. If migrations are required, the load node will run them (if enabled in options) before returning success.

Handling failures:
- If Result.Success is false, read Result.ErrorCode and Result.Message to display an appropriate message or attempt fallback logic (load backup, notify user, revert to default state).

### 6.5 Enumerating and Managing Slots
- Use `EzSave Enumerate Slots` to populate a save/load UI list. The node returns an array of slot info (name, timestamp, playtime, custom tags).
- Provide `Load` and `Delete` actions per entry in your UI.
- Implement thumbnail support by storing an optional preview image path or base64-encoded thumbnail in slot metadata.

### 6.6 Backup and Restore
- Call `EzSave Create Backup` to create an immediate backup for the current slot.
- Use `EzSave Restore Backup` to revert to a previous backup. The restore node returns the backup metadata and the result of the restore operation.

### 6.7 Validation and Reporting
- Use `EzSave Validate Slot` prior to important saves (e.g., before shipping a checkpoint) to ensure the payload conforms to the active schema.
- Validation nodes return detailed problem lists (field path, problem type, message) which you can display in an editor-only UI or log during development.

### 6.8 Auto-Save and QuickSave Patterns
Auto-Save recommended pattern:
- Run a timer (e.g., every 300s) in a central manager (GameState or GameInstance) that calls `EzSave Save Slot (Async)`.
- Ensure the auto-save options set `CreateBackup=true` and `bEncrypt` as needed.
- Avoid auto-saving during heavy I/O operations or cutscenes — provide a small debounce window.

QuickSave pattern:
- Bind a key (e.g., F5) to call the QuickSave node which invokes `EzSave Save Slot (Async)` on a preconfigured default slot.
- On completion, show a short confirmation toast and optionally store the quicksave timestamp in local UI state.

### 6.9 Exposing Data to Blueprints
- Expose a `USaveGame`-derived UObject or a lightweight `USTRUCT(BlueprintType)` container as the payload for saves.
- Prefer small, explicit containers (primitive types, arrays of simple structs) rather than referencing large actor graphs.
- If the game needs to persist actor references, store soft references (FSoftObjectPath) or GUIDs and resolve them at load time.

### 6.10 Debugging Blueprints Saves
- Enable verbose logging by setting the `LogEzSave` category to `Verbose` in `DefaultEngine.ini` during development.
- Inspect generated save files in the project's save path. Files are human-readable for JSON/TOML/YAML/XML formats unless encrypted.
- Use the `EzSave Validate Slot` node to surface schema problems before trying to load in-game.
- For malformed payloads, the deserialization error includes a detailed path to the failing field — log that in your UI or console.

### 6.11 Error Handling Best Practices
- Always check the `Result` struct on completion for `Success` and `ErrorCode`.
- Implement retry logic for transient provider failures (e.g., temporary cloud unavailability).
- Provide a graceful fallback: load the most recent backup, or restore a default player state and continue.

### 6.12 Multiplayer Considerations (Blueprints)
- For multiplayer games, do not rely on client-side authoritative saves for shared world state. Instead:
  - Save per-player progress client-side (private slots) and let the server own shared persistent state.
  - Use server RPCs to request server-side persistence where necessary.
  - Synchronize slot metadata (timestamps, tags) between client and server via secure RPCs to avoid tampering.

### 6.13 Editor-Only Tools and Blueprint Utility Widgets
- The plugin includes editor widgets (schema editor) that integrate with the Blueprint workflow. Use editor utilities to preview schema-driven forms and validate payloads before runtime use.

### 6.14 Examples (Blueprint flow snippets)
QuickSave Button (Async) flow:
- OnClicked -> EzSave Save Slot (Async) -> Bind Completed -> If Success: ShowToast("Saved") Else: ShowToast("Save failed: " + Result.Message)

Auto-Save Timer flow:
- BeginPlay (GameInstance) -> Set Timer (300s repeating) -> Timer Callback -> EzSave Save Slot (Async) with Debounce

Slot List UI flow:
- On Open Menu -> EzSave Enumerate Slots -> ForEach Slot: Create Row (SlotName, Timestamp) -> Bind Load Button to EzSave Load Slot (Async)

---

## 7. C++ GUIDE
This section explains how to use EzSaveGame from C++. It covers module setup, the available classes and functions, typical usage patterns, error handling, debugging tips and recommended best practices for C++ developers.

### 7.1 Module Setup
To use EzSaveGame in C++:
- Ensure your project includes the `EzSaveGameRuntime` module in its `.uproject` or `CMakeLists.txt`.
- Add `EzSaveGameRuntime` to the PublicDependencyModuleNames or PrivateDependencyModuleNames section depending on usage.

Example `YourProject.Build.cs`:
```csharp
PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "EzSaveGameRuntime" });
```

### 7.2 Core C++ Classes (summary)
- UEzSaveManager (UObject-derived)
  - SaveSlot(FName SlotName, UObject* Payload, const FEzSaveOptions& Options) -> bool
  - SaveSlotAsync(FName SlotName, UObject* Payload, const FEzSaveOptions& Options, FOnEzSaveComplete Delegate)
  - LoadSlot(FName SlotName, UObject* OutTarget, const FEzLoadOptions& Options) -> bool
  - LoadSlotAsync(FName SlotName, UObject* OutTarget, const FEzLoadOptions& Options, FOnEzLoadComplete Delegate)
  - EnumerateSlots(TArray<FEzSlotInfo>& OutSlots) -> void
  - DeleteSlot(FName SlotName) -> bool

- FEzSaveOptions (struct)
  - FormatOverride (optional) : EEzFormat
  - bEncrypt : bool
  - bCreateBackup : bool
  - MetadataOverrides: map<string,string>

- FEzLoadOptions (struct)
  - bValidate : bool
  - bRunMigrations : bool
  - DesiredFormat : optional

- FEzSlotInfo (struct)
  - SlotName : FName
  - Timestamp : FDateTime
  - SchemaVersion : FString
  - PlaytimeSeconds : int64
  - CustomTags : TMap<FString,FString>

### 7.3 Saving and Loading (C++)
Typical save/load pattern in C++:
1. Obtain the SaveManager instance:
   - `UEzSaveManager* Manager = GetGameInstance()->GetSubsystem<UEzSaveManager>();`
2. Configure options:
   - `FEzSaveOptions Options; Options.bEncrypt = true; Options.bCreateBackup = true;`
3. Call SaveSlot or LoadSlot:
   - `Manager->SaveSlot(TEXT("Player_01"), MySaveGameObject, Options);`
   - `Manager->LoadSlot(TEXT("Player_01"), MySaveGameObject, Options);`

Async variants are available with the `Async` suffix and require a completion delegate.

### 7.4 Error Handling (C++)
Error handling in C++:
- Check the return value of SaveSlot/LoadSlot for success.
- On failure, inspect the error code and message in the result struct.
- Implement retry logic for transient errors (e.g., network issues).

Example:
```cpp
FEzSaveResult Result;
if(!Manager->SaveSlot(TEXT("Player_01"), MySaveGameObject, Options, Result))
{
    UE_LOG(LogEzSave, Error, TEXT("Save failed: %s"), *Result.Message);
    if(Result.ErrorCode == FEzErrorCode::ProviderWriteFailed)
    {
        // Retry logic or fallback
    }
}
```

### 7.5 Debugging Tips (C++)
- Enable verbose logging for EzSaveGame by setting `LogEzSave` to `Verbose` in `DefaultEngine.ini`.
- Inspect generated save files in the project's save directory. Files are human-readable for JSON/TOML/YAML/XML formats unless encrypted.
- Use the schema editor utility to validate and preview schemas outside of gameplay.

### 7.6 Best Practices (C++)
- Prefer asynchronous save/load calls in gameplay code to avoid frame drops.
- Use the delegate-based async pattern for complex workflows that need to chain actions on save/load completion.
- Regularly test save/load functionality throughout development, especially after schema or major gameplay changes.

---

## 8. SCHEMA SYSTEM
The schema system enables structured, versioned and validated save data. This section describes how schemas work in EzSaveGame, how to define and edit them, and best practices for using the schema system.

### 8.1 Overview
Schemas are machine-readable definitions that describe the structure and validation rules for saved data. They enable:
- Validation of save data against the schema to catch errors early.
- Automatic generation of UI elements in the editor for schema fields.
- Versioning of schemas to support evolution of save data structures over time.

### 8.2 Schema Definition Language (SDL)
Schemas are defined using a simple schema definition language (SDL) that resembles JSON. Example:
```json
{
  "SchemaVersion": "1.0.0",
  "Fields": [
    {"Name": "PlayerName", "Type": "string", "Required": true},
    {"Name": "PlayerStats", "Type": "struct", "Fields": [
      {"Name": "Health", "Type": "float", "Min": 0.0, "Max": 100.0},
      {"Name": "Experience", "Type": "int", "Min": 0}
    ]}
  ]
}
```

### 8.3 Editing Schemas
- Use the integrated schema editor in the Unreal Editor to create and modify schemas.
- The editor provides a visual interface to define fields, types, constraints and metadata.
- Schemas are automatically associated with `USaveGame`-derived classes at runtime.

### 8.4 Schema Validation
Validation ensures that saved data conforms to the expected schema. Validation occurs:
- Automatically during save and load operations.
- Manually via the `EzSave Validate Slot` node or equivalent C++ function.

Validation results are reported in the `FEzValidationReport` struct, detailing any issues found.

### 8.5 Schema Migration
Migration transforms saved data from an old schema version to the current version. Migration strategies:
- Automatic: simple renames, type changes, default value injections.
- Custom: developer-defined migration functions for complex changes.

Migration runs as part of the load process if the saved data schema version is older than the current schema version.

### 8.6 Best Practices (Schema System)
- Keep schemas simple and focused; avoid unnecessary complexity in field types or validation rules.
- Regularly review and test migrations to ensure saved data remains valid across schema changes.
- Use descriptive field and schema names to make schemas self-documenting.

---

## 9. SLOT SYSTEM
The slot system provides a way to manage multiple save states for a game. This section explains how slots work in EzSaveGame, how to manage them, and best practices for using the slot system.

### 9.1 Overview
Slots are named containers for saved data, allowing multiple save states per game. Each slot has:
- A unique name.
- Associated metadata (timestamp, playtime, custom tags).
- A payload: the serialized game data.

Slots enable features like:
- QuickSave/QuickLoad: fast save/load operations on a default slot.
- Auto-Save: periodic saving to a specific slot.
- Manual save/load: explicit operations on named slots.

### 9.2 Slot Metadata
Slot metadata provides additional context about a save slot. Common metadata fields:
- `Timestamp`: when the slot was last modified.
- `PlaytimeSeconds`: how long the game was played since this save.
- `CustomTags`: user-defined tags for categorizing or filtering slots.

Metadata is automatically managed by the plugin but can be overridden or set by the user.

### 9.3 Managing Slots
Slots are managed via the `EzSaveManager` API:
- `SaveSlot`: save data to a slot.
- `LoadSlot`: load data from a slot.
- `DeleteSlot`: remove a slot.
- `EnumerateSlots`: list all slots and their metadata.

### 9.4 Best Practices (Slot System)
- Use descriptive and consistent naming for slots to avoid confusion.
- Regularly enumerate and clean up old or unused slots to reduce clutter.
- Consider user privacy and data security when managing save slots, especially on shared or public systems.

---

## 10. DATA VALIDATION
Data validation ensures that saved data meets the expected format and constraints. This section describes the validation features in EzSaveGame, how to define validation rules, and best practices for validating data.

### 10.1 Overview
Validation checks that saved data conforms to the defined schema and meets any additional application-specific rules. Validation occurs:
- Automatically during save and load operations.
- Manually via the `EzSave Validate Slot` node or equivalent C++ function.

### 10.2 Built-in Validation Rules
EzSaveGame provides several built-in validation rules:
- Required fields: check that specified fields are present.
- Type checks: ensure fields are of the expected type (string, int, float, etc.).
- Range checks: verify numeric values fall within a specified range.
- Pattern matching: use regular expressions to validate string formats.

### 10.3 Custom Validation
Custom validation rules can be defined by implementing the `IEzValidator` interface in a plugin or game module. Custom validators can be used to enforce domain-specific rules or complex invariants.

### 10.4 Best Practices (Data Validation)
- Define clear and concise validation rules for all saved data.
- Regularly review and test validation logic to ensure data integrity.
- Use validation warnings and errors to guide users towards fixing data issues.

---

## 11. ENCRYPTION
Encryption secures saved data against unauthorized access or tampering. This section explains the encryption features in EzSaveGame, how to configure encryption, and best practices for using encryption.

### 11.1 Overview
EzSaveGame supports optional encryption of saved data using AES-256-GCM. Encryption is applied at the byte level to the serialized payload.

### 11.2 Configuring Encryption
Encryption can be configured via project settings or at runtime:
- Project settings: set the default encryption mode (disabled, editor, production).
- Runtime: specify encryption options in the `FEzSaveOptions` struct when calling save functions.

### 11.3 Key Management
Encryption keys are managed via a key provider implementation of the `IEzKeyProvider` interface. The key provider is responsible for:
- Providing the encryption key material.
- Key rotation and invalidation.

### 11.4 Best Practices (Encryption)
- Always use encryption in production builds to protect user data.
- Regularly rotate encryption keys and update any dependent systems or configurations.
- Monitor for and respond to any encryption or key management errors.

---

## 12. FILE FORMATS
EzSaveGame supports multiple file formats for serialized data. This section describes the available file formats, their characteristics, and best practices for choosing a file format.

### 12.1 Supported File Formats
- JSON: human-readable text format, easy to debug and edit. Suitable for development and configuration data.
- TOML: minimal configuration file format, more readable than JSON for complex data. Good for settings and structured data.
- YAML: human-friendly data serialization, supports comments and complex structures. Use for configuration or data that benefits from readability.
- XML: markup language that defines rules for encoding documents. Use for interoperability with other systems or when XML is a project requirement.

### 12.2 Choosing a File Format
Consider the following when choosing a file format:
- Readability: JSON, TOML, and YAML are more human-readable than XML.
- Complexity: For simple key-value pairs, any format will do. For nested or complex data, TOML or YAML may be easier to work with.
- Interoperability: If the data needs to be shared with other systems or tools, consider using XML or JSON.
- Performance: For large data sets, binary formats (not covered here) may be more efficient. Text formats are generally slower to parse.

### 12.3 Best Practices (File Formats)
- Use the default JSON format for development and debugging due to its readability.
- For production, consider using TOML or YAML for configuration data that benefits from comments and structure.
- Avoid using XML unless required by external systems or for specific project needs.

---

## 13. PRACTICAL EXAMPLES
This section provides practical examples and code snippets for common tasks using EzSaveGame. It covers saving and loading data, managing slots, handling errors, and using Blueprint nodes.

### 13.1 Saving and Loading (Blueprints)
To save data:
1. Create or obtain a `SaveGame` object containing the data to save.
2. Call the `EzSave Save Slot (Async)` node with:
   - SlotName: the name of the slot to save to.
   - SaveObject: the `SaveGame` object.
   - Options: any save options (e.g., encryption, backup).
3. Bind to the Completed event to handle success or failure.

To load data:
1. Call the `EzSave Load Slot (Async)` node with:
   - SlotName: the name of the slot to load from.
   - Target: the `SaveGame` object to populate.
   - Options: any load options (e.g., validation, migration).
2. Bind to the Completed event to handle success or failure.

### 13.2 Saving and Loading (C++)
To save data:
```cpp
UEzSaveManager* Manager = GetGameInstance()->GetSubsystem<UEzSaveManager>();
FEzSaveOptions Options; Options.bEncrypt = true; Options.bCreateBackup = true;
Manager->SaveSlot(TEXT("Player_01"), MySaveGameObject, Options);
```
To load data:
```cpp
UEzSaveManager* Manager = GetGameInstance()->GetSubsystem<UEzSaveManager>();
FEzLoadOptions Options; Options.bValidate = true; Options.bRunMigrations = true;
Manager->LoadSlot(TEXT("Player_01"), MySaveGameObject, Options);
```

### 13.3 Managing Slots
To enumerate slots:
- Call `EzSave Enumerate Slots` to get a list of all slots and their metadata.

To delete a slot:
- Call `EzSave Delete Slot` with the slot name.

To create a backup:
- Call `EzSave Create Backup` to create an immediate backup of the current slot.

To restore a backup:
- Call `EzSave Restore Backup` with the backup metadata.

### 13.4 Error Handling
In Blueprints:
- Check the Result struct on completion of save/load nodes.
- Implement retry logic or fallback actions based on the error code.

In C++:
- Check the return value of save/load functions.
- Inspect the result struct for error details and handle accordingly.

### 13.5 Debugging Tips
- Enable verbose logging for EzSaveGame in `DefaultEngine.ini`.
- Inspect generated save files for readability and correctness.
- Use the schema editor utility to validate schemas and preview data.

---

## 14. USE CASES
This section explores common use cases for EzSaveGame and provides guidance on implementing these scenarios.

### 14.1 Simple Save/Load
For basic save/load functionality:
- Create a `USaveGame`-derived class to hold the data.
- Use `EzSave Save Slot` and `EzSave Load Slot` nodes or functions to save and load the data.

### 14.2 QuickSave/QuickLoad
To implement QuickSave/QuickLoad:
- Bind a key (e.g., F5) to a Blueprint or C++ function that calls `EzSave Save Slot (Async)` on a default slot.
- Use `EzSave Load Slot (Async)` to load the quicksave.

### 14.3 Auto-Save
For auto-save functionality:
- Set up a timer in your game instance or state that calls `EzSave Save Slot (Async)` at regular intervals.
- Ensure the auto-save slot is configured and that backups are enabled.

### 14.4 Slot Management
To manage multiple save slots:
- Use `EzSave Enumerate Slots` to list all available slots.
- Provide UI for the player to select a slot to load or delete.
- Use metadata (timestamp, playtime) to display slot information in the UI.

### 14.5 Data Migration
For migrating data between schema versions:
- Define migration functions in your schema generator module.
- Implement version checks and automatic migration during the load process.

---

## 15. OPTIMIZATION AND PERFORMANCE
This section provides guidance on optimizing save/load performance and reducing the impact of EzSaveGame on game runtime.

### 15.1 General Optimization Tips
- Prefer asynchronous save/load operations to avoid blocking the game thread.
- Use the delegate-based async pattern for complex workflows to minimize latency.
- Regularly profile save/load performance and optimize based on findings.

### 15.2 Reducing Save File Size
To reduce the size of save files:
- Exclude unnecessary data from the save payload. Only save what's needed.
- Use compression options in the storage provider if available.
- Regularly review and clean up old or unused save slots and backups.

### 15.3 Performance Monitoring
To monitor performance:
- Use Unreal's built-in profiling tools (e.g., Session Frontend, Profiler) to measure the impact of save/load operations.
- Monitor disk I/O, memory usage, and CPU usage during save/load to identify bottlenecks.

---

## 16. CONSOLE COMMANDS
This section documents the console commands provided by EzSaveGame for debugging and automation.

### 16.1 List of Console Commands
- `EzSave.ListSlots`: List all save slots and their metadata.
- `EzSave.DeleteSlot <SlotName>`: Delete the specified save slot.
- `EzSave.QuickSave`: Perform a QuickSave to the default slot.
- `EzSave.QuickLoad`: Perform a QuickLoad from the default slot.
- `EzSave.ValidateSlot <SlotName>`: Validate the specified save slot against the schema.

---

## 17. MIGRATION AND UPGRADES
This section provides guidance on migrating between EzSaveGame versions and upgrading your project to use new plugin releases.

### 17.1 Migrating Between Plugin Versions
To migrate between plugin versions:
1. Backup your project and saves.
2. Update the plugin folder in `Plugins/` to the new version.
3. Restart the Unreal Editor.
4. Run the validation utility to check for schema/version mismatches.

### 17.2 Upgrading the Plugin
When upgrading the plugin:
1. Read the release notes for breaking changes.
2. Backup your existing saves (copy the saves folder to a safe location).
3. Update the plugin folder in `Plugins/` and restart the editor.
4. Run the validation utility included with the plugin to scan existing saves for schema/version mismatches.

---

## 18. TROUBLESHOOTING
This section provides troubleshooting tips for common issues encountered when using EzSaveGame.

### 18.1 Common Issues and Solutions
- Issue: Plugin not visible in the Plugins browser.
  - Solution: Ensure the plugin folder contains a valid `.uplugin` file and that the path is `Project/Plugins/EzSaveGame`.
- Issue: The editor fails to compile the plugin modules.
  - Solution: Confirm your Visual Studio toolchain and Unreal Engine version match the plugin's expected compiler settings.
- Issue: Permissions errors when saving during play-in-editor.
  - Solution: Check that the path configured for saves is writable by the editor process.

### 18.2 Debugging Tips
- Enable verbose logging by setting the `LogEzSave` category to `Verbose` in `DefaultEngine.ini` during development.
- Inspect generated save files in the project's save path. Files are human-readable for JSON/TOML/YAML/XML formats unless encrypted.
- Use the `EzSave Validate Slot` node to surface schema problems before trying to load in-game.
- For malformed payloads, the deserialization error includes a detailed path to the failing field — log that in your UI or console.

---

## 19. BEST PRACTICES
This section provides best practices for using EzSaveGame effectively and avoiding common pitfalls.

### 19.1 General Best Practices
- Regularly test save/load functionality throughout development, especially after schema or major gameplay changes.
- Use version control for your project and include the saves directory to track changes to saved data.
- Document your schema and save game structure to aid in debugging and future development.

### 19.2 Performance Best Practices
- Prefer asynchronous save/load operations to avoid blocking the game thread.
- Use the delegate-based async pattern for complex workflows to minimize latency.
- Regularly profile save/load performance and optimize based on findings.

### 19.3 Security Best Practices
- Always use encryption in production builds to protect user data.
- Regularly rotate encryption keys and update any dependent systems or configurations.
- Monitor for and respond to any encryption or key management errors.

---

## 20. FAQ
This section provides answers to frequently asked questions about EzSaveGame.

### 20.1 What platforms does EzSaveGame support?
EzSaveGame supports all platforms that Unreal Engine 5.6+ supports, including Windows, macOS, Linux, iOS, Android, and consoles.

### 20.2 Can I use EzSaveGame with Blueprints only?
Yes, EzSaveGame is designed to be fully usable from Blueprints. All core functionality is exposed via Blueprint nodes.

### 20.3 Do I need to know C++ to use EzSaveGame?
No, you can use EzSaveGame entirely from Blueprints. However, knowing C++ can help you extend the plugin or integrate it more deeply with your game.

### 20.4 Is there a limit to the number of save slots?
No, but managing a very large number of slots may impact performance. Regularly enumerate and clean up old or unused slots to maintain performance.

### 20.5 Can I change the serialization format later?
Yes, you can change the serialization format by updating the project settings or at runtime via the `FEzSaveOptions` struct. Existing data will be migrated to the new format if necessary.

### 20.6 How does encryption work?
Encryption is optional and uses AES-256-GCM to encrypt the serialized payload. A key provider implementation of the `IEzKeyProvider` interface supplies the encryption key.

### 20.7 What should I do if I encounter a bug?
First, check the plugin's documentation and FAQ for solutions. If the issue persists, contact the plugin author with detailed information about the bug, including steps to reproduce it.

### 20.8 Where can I get support?
For support, contact the plugin author or visit the plugin's support forum/page. Provide detailed information about your issue, including Unreal Engine version, plugin version, and steps to reproduce the problem.

---

## AUTHOR & CREDITS

This user guide and the EzSaveGame plugin were created by José Antonio Mota Lucas.

- Author: José Antonio Mota Lucas
- LinkedIn: https://www.linkedin.com/in/joseantoniomotalucas/

### Third-Party Libraries and Credits
EzSaveGame includes and uses several third-party libraries for parsing and serialization. We gratefully acknowledge the work of their authors and maintainers:

- tinyxml2 — Lightweight XML parser. Repository: https://github.com/leethomason/tinyxml2
  - License: See `Source/ThirdParty/tinyxml2/LICENSE.txt` in this repository (permissive zlib/libpng-style license).

- yaml-cpp — YAML parser and emitter for C++. Created by Jesse Beder. Repository: https://github.com/jbeder/yaml-cpp
  - License: MIT (see `Source/ThirdParty/yaml-cpp/LICENSE`).

- tomlplusplus (toml++) — TOML parser for modern C++. Repository: https://github.com/marzer/tomlplusplus
  - License: MIT (see `Source/ThirdParty/tomlplusplus/toml.hpp` header and project license).

These libraries remain the property of their respective authors. Their licenses and copyright notices are included in the `Source/ThirdParty/` folder of this plugin — please consult those files for full license terms.

---

