﻿﻿# GUIA DE USUARIO COMPLETA - EZSAVEGAME PLUGIN
Version: 1.0.0
Motor: Unreal Engine 5.6+
Fecha: 4 de Diciembre 2025
---
## TABLA DE CONTENIDOS
1. [INTRODUCCION](#1-introduccion)
2. [ARQUITECTURA DEL SISTEMA](#2-arquitectura-del-sistema)
3. [INSTALACION Y CONFIGURACION](#3-instalacion-y-configuracion)
4. [CONCEPTOS FUNDAMENTALES](#4-conceptos-fundamentales)
5. [REFERENCIA COMPLETA DE API](#5-referencia-completa-de-api)
6. [GUIA DE BLUEPRINTS](#6-guia-de-blueprints)
7. [GUIA DE C++](#7-guia-de-c)
8. [SISTEMA DE ESQUEMAS](#8-sistema-de-esquemas)
9. [SISTEMA DE SLOTS](#9-sistema-de-slots)
10. [VALIDACION DE DATOS](#10-validacion-de-datos)
11. [ENCRIPTACION](#11-encriptacion)
12. [FORMATOS DE ARCHIVO](#12-formatos-de-archivo)
13. [EJEMPLOS PRACTICOS](#13-ejemplos-practicos)
14. [CASOS DE USO](#14-casos-de-uso)
15. [OPTIMIZACION Y RENDIMIENTO](#15-optimizacion-y-rendimiento)
16. [COMANDOS DE CONSOLA](#16-comandos-de-consola)
17. [MIGRACION Y ACTUALIZACION](#17-migracion-y-actualizacion)
18. [SOLUCION DE PROBLEMAS](#18-solucion-de-problemas)
19. [MEJORES PRACTICAS](#19-mejores-practicas)
20. [PREGUNTAS FRECUENTES](#20-preguntas-frecuentes)
---
## 1. INTRODUCCION
### 1.1 Que es EzSaveGame
EzSaveGame es un plugin integral de persistencia para Unreal Engine que ofrece flujos de guardado/carga fiables, flexibles y seguros tanto para juegos como para experiencias interactivas. Combina formatos de configuracion legibles por humanos (TOML, JSON, YAML, XML) con la infraestructura nativa de guardado de Unreal para brindar un flujo de trabajo comodo para disenadores y programadores.

### 1.2 Filosofia de Diseno
EzSaveGame se basa en estos principios clave:
- Facilidad de Uso: API minimalista y consistente disponible en Blueprints y C++.
- Flexibilidad: Soporte para varios formatos de serializacion e intercambiabilidad de backends de almacenamiento.
- Seguridad: Encriptacion AES-256 opcional y verificaciones HMAC para builds de produccion.
- Escalabilidad: Sistema de slots que habilita multiples estados de guardado y politicas por usuario.
- Validacion: Enfoque schema-first para garantizar integridad de datos y facilitar migraciones seguras.
- Depurabilidad: Artefactos legibles en editor y diagnosticos claros para resolver incidencias rapido.

### 1.3 Caracteristicas Clave
- Serializacion multiformato: TOML, JSON, YAML, XML.
- Generacion automatica de esquemas a partir de clases derivadas de `USaveGame`.
- Editor visual de esquemas integrado en Unreal Editor.
- Gestion de slots con metadatos (marca de tiempo, tiempo de sesion, nivel, etiquetas personalizadas).
- Reglas de validacion: campos obligatorios, min/max, patrones regex y validadores personalizados.
- Pipeline opcional de encriptacion AES-256 y verificacion HMAC.
- Conjunto completo de nodos Blueprint y API C++.
- Soporte para primitivos, tipos del engine (Vector, Rotator), arrays y estructuras anidadas.
- Auto-save, QuickSave/QuickLoad, backups y escrituras atomicas.
- Comandos de consola para depuracion y automatizacion.

### 1.4 Audiencia y Alcance
Esta guia esta dirigida a desarrolladores, disenadores tecnicos e ingenieros que usan Unreal Engine 5.6+ y necesitan un sistema de guardado robusto y extensible. Cubre:
- Instalacion y configuracion para editor y runtime.
- Patrones de uso en Blueprints y C++ con ejemplos de codigo.
- El sistema de esquemas y reglas de validacion.
- Consideraciones de seguridad y encriptacion.
- Solucion de problemas, optimizacion y estrategias de migracion.

---

## 2. ARQUITECTURA DEL SISTEMA
### 2.1 Descripcion General
EzSaveGame se organiza en capas modulares que separan integracion de aplicacion, serializacion de datos y almacenamiento fisico. Esta separacion facilita extender, probar y mantener el plugin, ademas de ofrecer puntos de extension claros para formatos personalizados, backends propios y logica de validacion.

Capas principales:
- Capa de Aplicacion: API publica consumida por gameplay y Blueprints.
- Capa de Serializacion: adaptadores especificos por formato que convierten objetos en memoria a representaciones legibles y viceversa.
- Capa de Almacenamiento: proveedores que gestionan persistencia fisica e I/O especifico por plataforma.

### 2.2 Capa de Aplicacion (Responsabilidades)
Expone la API de alto nivel para codigo de juego y disenadores. Sus responsabilidades incluyen:
- Gestionar slots nombrados y sus metadatos (timestamp, playtime, etiquetas).
- Exponer operaciones de guardado/carga sincronas y asincronas para Blueprints y C++.
- Coordinar generacion de esquemas, validacion y migracion antes/despues de persistir datos.
- Disparar eventos y callbacks (exito/fallo, progreso, reportes de validacion).

Componentes clave:
- Save Manager: servicio central que orquesta guardado/carga y ciclo de vida de slots.
- Slot Manager: helper que rastrea metadatos por slot y politicas de retencion.
- Config Subsystem: ajustes en runtime, opciones por defecto y hooks de registro.

### 2.3 Capa de Serializacion
Contiene adaptadores que traducen objetos en memoria a un formato (JSON, TOML, YAML, XML) y viceversa. Cada adaptador implementa una interfaz comun que garantiza:
- Orden de serializacion determinista cuando se requieran diffs estables.
- Respeto por restricciones de esquema y metadatos de version.
- Reportes claros ante payloads malformados o campos faltantes.

Responsabilidades:
- Modelar tipos del engine (FVector, FRotator, FTransform), arrays y estructuras anidadas.
- Emitir/consumir metadatos de esquema (version, descriptores de campos, atributos custom).
- Ofrecer puntos de extension para serializadores de tipos personalizados.

Adaptadores incluidos:
- JSONAdapter
- TOMLAdapter
- YAMLAdapter
- XMLAdapter

Cada adaptador es un modulo enchufable; se pueden registrar adaptadores de terceros o especificos del proyecto durante el inicio via el Config Subsystem.

### 2.4 Capa de Almacenamiento
Abstrae la persistencia fisica y garantiza operaciones seguras y atomicas.

Responsabilidades:
- Realizar escrituras atomicas (escribir en archivo temporal y luego renombrar) para evitar saves parciales.
- Implementar rutas conscientes de plataforma y verificaciones de permisos (directorios por usuario, sandboxes).
- Hooks opcionales de compresion y encriptacion antes/despues de escribir.
- Mantener proveedores integrados: LocalFilesystemProvider, InMemoryProvider (tests) y una interfaz CloudProvider opcional.

Los proveedores exponen un contrato simple: Save(nombre, payload, opciones) y Load(nombre) -> payload. Manejan reintentos de bajo nivel, bloqueos de archivo y diagnosticos basicos.

### 2.5 Flujo de Componentes
Flujo tipico:
1. El juego invoca SaveManager->SaveSlot(nombreSlot, UObject* payload, opciones).
2. La Capa de Aplicacion valida el payload contra el esquema activo y ejecuta migraciones si es necesario.
3. El adaptador de Serializacion convierte el payload a formato elegido y adjunta metadatos de esquema/version.
4. Si hay encriptacion, el pipeline transforma los bytes serializados.
5. El Proveedor de Almacenamiento realiza la escritura atomica en disco o nube.
6. En exito/fallo el SaveManager lanza callbacks y actualiza metadatos del slot.

El flujo de carga es inverso: Storage Provider -> Desencriptar -> Deserializar -> Validar -> Migrar -> Entregar al solicitante.

### 2.6 Versionado y Migracion
Cada artefacto guardado incluye metadatos con identificador y version de esquema. EzSaveGame permite:
- Chequeos automaticos de version durante la carga.
- Callbacks o transformaciones definidas por el desarrollador para actualizar saves antiguos.
- Estrategias de respaldo cuando la migracion falla (backup automatico, modo legacy solo lectura, diagnosticos claros).

Las hooks de migracion viven en el modulo Schema Generator y pueden generarse automaticamente para cambios comunes o implementarse manualmente.

### 2.7 Modelo de Seguridad
La seguridad es opcional, orientada a proteger builds empaquetados y datos del usuario. Conceptos clave:
- Pipeline de encriptacion (recomendado AES-256-GCM) con HMAC opcional para deteccion de manipulaciones.
- Provision de llaves via proveedores seguros que leen de keystores de plataforma o secretos inyectados en runtime.
- En editor se usan llaves no productivas con advertencias explicitas; los builds de produccion deben usar provision real.

### 2.8 Puntos de Extension
EzSaveGame ofrece varios puntos de extension:
- Registro de adaptadores: serializadores personalizados para nuevos formatos.
- Plugins de proveedor: backends de almacenamiento (nube, SDKs de plataforma).
- Validadores personalizados: reglas especificas del dominio ejecutadas en la Capa de Aplicacion.
- Manejadores de migracion: actualizaciones complejas entre versiones de esquema.

### 2.9 Distribucion de Modulos
La base de codigo se divide en:
- EzSaveGameRuntime: serializacion en runtime, almacenamiento, generador de esquemas y API principal.
- EzSaveGameEditor: widgets de editor, editor de esquemas y tooling.
- ThirdParty: bibliotecas incluidas (tinyxml2, yaml-cpp, tomlplusplus) usadas por los adaptadores.

---

## 3. INSTALACION Y CONFIGURACION
### 3.1 Requisitos Previos
Antes de instalar EzSaveGame, confirma:
- Unreal Engine 5.6 o superior.
- Visual Studio 2019/2022 (u otra toolchain equivalente) para compilar modulos en Windows.
- Opcional: SDKs de plataforma para almacenamiento en la nube o backends especificos.
- Opcional (encriptacion): OpenSSL o APIs criptograficas de la plataforma si crearas proveedores personalizados.

### 3.2 Instalacion en el Editor (Proyecto local)
Para agregar EzSaveGame a un proyecto:
1. Copia la carpeta `EzSaveGame` dentro de `TuProyecto/Plugins/`.
2. Abre el proyecto en Unreal Editor; compilara modulos en el primer arranque si es necesario.
3. Ve a `Edit -> Plugins`, busca `EzSaveGame` (categoria Utilities) y habilitalo si aun no lo esta.
4. Reinicia el editor cuando se te solicite para finalizar la inicializacion.

Notas:
- Si usas control de versiones, agrega el plugin una vez validado en tu proyecto.
- Para instalacion a nivel engine, colocalo en `Engine/Plugins/`, aunque se recomienda mantenerlo por proyecto para personalizaciones.

### 3.3 Distribucion Binaria vs Codigo Fuente
- Distribucion binaria: incluye modulos precompilados en `Binaries/` y es mas rapida de integrar. Ideal si hay diferencias de toolchain.
- Distribucion con codigo fuente: incluye `Source/` y requiere compilacion en el primer arranque, pero te permite modificar el plugin.

### 3.4 Ajustes del Plugin (Project Settings)
En `Edit -> Project Settings -> EzSaveGame` encontraras opciones como:
- Formato de serializacion por defecto: JSON | TOML | YAML | XML.
- Politica de ruta de guardado: Per-User | Per-Install | Custom Path.
- Intervalo de Auto-Save (segundos): 0 = deshabilitado.
- Auto-Save on Exit (Guardado automático al salir): Guarda automáticamente cuando el juego se cierra.
- Slot de QuickSave: slot utilizado por los nodos de guardado rapido.
- Encriptacion: Disabled | Editor (debug key) | Production (requiere key provider).
- Backup Retention Count: numero de backups automaticos por slot.

**Comportamiento de Auto-Save on Exit:**
- Cuando está activado, el sistema guarda automáticamente todos los datos actuales al cerrar el juego.
- Usa el método `SaveToDisk()` del subsistema para persistir datos de forma síncrona antes del cierre.
- **Limitaciones importantes:**
  - En PIE (Play In Editor): Puede no ejecutarse si el editor detiene el juego abruptamente (Ctrl+Alt+F11 o stop forzado).
  - En builds empaquetados: Funciona de forma confiable cuando el jugador cierra el juego normalmente (botón Quit, X de ventana).
  - NO funciona: En crashes, cierres con Task Manager, o pérdida de energía.
- **Uso recomendado:** Activar como red de seguridad, pero siempre proporcionar opciones de guardado manual para los jugadores.
- Para pruebas en desarrollo, implementa guardado manual en `EndPlay()` para asegurar que los datos se guarden al detener PIE.

Recomendado para desarrollo:
- Formato por defecto: JSON
- Encriptacion: Disabled o Editor
- Auto-Save Interval: 300 (5 min)
- Auto-Save on Exit: Activado (con respaldo de guardado manual en código)

Recomendado para builds finales:
- Formato por defecto: TOML o JSON (segun toolchain)
- Encriptacion: Production (provider con llave real)
- Auto-Save Interval: ajustar a necesidades del juego
- Auto-Save on Exit: Activado (como red de seguridad)

#### 3.4.1 Botones de Utilidad en Project Settings
El panel de configuración de EzSaveGame en Unreal Editor incluye botones de utilidad para tareas comunes de desarrollo. Estos botones se encuentran en `Edit -> Project Settings -> Plugins -> EzSaveGame` en la categoría **Utilities**:

**Open Save Folder (Abrir Carpeta de Guardado)**
- Abre el directorio de guardado en el explorador de archivos del sistema (Windows Explorer, Finder, etc.).
- Útil para inspeccionar, respaldar o editar manualmente archivos de guardado durante el desarrollo.
- Crea el directorio si aún no existe.

**Delete All Save Data (Borrar Todos los Datos de Guardado)**
- Elimina permanentemente TODOS los datos de guardado incluyendo:
  - Todos los slots de guardado (Slot0, Slot1, ..., AutoSave)
  - Archivos de metadatos de slots
  - Archivos de capturas de pantalla asociados con slots
  - Archivos de configuración (EzSaveConfig.*)
  - Archivos de esquema (EzSaveSchema.*)
- **ADVERTENCIA**: ¡Esta acción no se puede deshacer!
- Aparecerá un diálogo de confirmación antes de la eliminación, requiriendo confirmación explícita del usuario.
- El diálogo lista claramente qué se eliminará y advierte que la operación es irreversible.
- Tras una eliminación exitosa, un mensaje de confirmación muestra el número de archivos eliminados.
- Si algunos archivos no pueden eliminarse (ej. por permisos), se mostrará un mensaje de error y los detalles se registrarán en el Output Log.

**Recomendaciones de uso**:
- Usa "Open Save Folder" frecuentemente durante el desarrollo para verificar el contenido y estructura de los archivos de guardado.
- Usa "Delete All Save Data" cuando:
  - Resetees el proyecto a un estado limpio para pruebas.
  - Limpies archivos de guardado corruptos o inválidos.
  - Pruebes la experiencia de usuario nuevo sin guardados existentes.
  - Prepares una sesión de juego fresca o demo.
- Siempre respalda guardados importantes antes de usar "Delete All Save Data" si necesitas preservarlos.
- Estos botones son solo para el editor y no aparecen en builds empaquetados.

**Características de seguridad**:
- La operación de borrado requiere confirmación explícita mediante un diálogo modal.
- El diálogo de confirmación usa texto de advertencia claro e indicadores de color naranja/rojo.
- Todas las operaciones de eliminación se registran en el Output Log con propósitos de auditoría.
- Los fallos de eliminación se reportan con mensajes de error específicos indicando qué archivos no pudieron eliminarse.

### 3.5 Configuracion en Runtime
Puedes configurar en runtime via C++, Blueprints o archivos de configuracion. Pasos tipicos:
- Registrar el key provider seguro temprano (GameInstance::Init o un subsystem custom).
- Definir el adaptador de serializacion activo si necesitas un formato no por defecto.
- Ajustar rutas y politicas de retencion segun perfil de usuario o plataforma.

Ejemplo C++ (conceptual):
- En GameInstance::Init:
  - Inicializa el subsystem de EzSaveGame
  - `EzSave::RegisterKeyProvider(MakeShared<MyKeyProvider>());`
  - `EzSave::SetDefaultFormat(EEzFormat::JSON);`

Ejemplo Blueprint:
- Usa el nodo `EzSave Init` para fijar formato y opciones basicas al iniciar el juego.

### 3.6 Provision de Llaves y Encriptacion
Si activas encriptacion en produccion:
- Implementa un key provider que cumpla la interfaz del plugin y cargue llaves de un lugar seguro (keystore, variables de entorno, secret manager).
- No incluyas llaves productivas en el codigo fuente o builds finales.
- Prefiere keystores nativos (Keychain, DPAPI, Android Keystore, iOS Keychain).
- Si usas AES-256-GCM, el provider debe devolver material de llave y un identificador para rotacion/validacion.

En editor se usa un provider de debug incluido; emitira advertencias si se detecta en builds empaquetados.

### 3.7 Packaging y Notas por Plataforma
- Consolas/plataformas cerradas: verifica rutas de guardado y reglas de sandbox; usa el Storage Provider adecuado.
- Proveedores en la nube suelen requerir inicializar SDKs de plataforma (PlayStation, Xbox, Azure). Hazlo antes de usar EzSaveGame.
- En Windows, asegura que la ruta elegida tenga permisos de escritura; para saves por usuario usa `%LOCALAPPDATA%/TuCompania/TuJuego/Saves`.

### 3.8 Actualizar el Plugin
Para actualizar EzSaveGame:
1. Lee las release notes en busca de breaking changes.
2. Respaldar tus saves (copia la carpeta de guardados).
3. Sustituye la carpeta del plugin en `Plugins/` y reinicia el editor.
4. Ejecuta la utilidad de validacion incluida para detectar desfaces de esquema/version.

### 3.9 Problemas de Instalacion (Troubleshooting)
- El editor no compila los modulos: confirma que tu toolchain (VS + UE) coincide con la requerida por el plugin.
- El plugin no aparece en Plugins: comprueba que exista un `.uplugin` valido en `Project/Plugins/EzSaveGame`.
- Errores de permisos al guardar en PIE: revisa que la ruta configurada permita escritura al proceso del editor.

---

## 4. CONCEPTOS FUNDAMENTALES
Esta seccion define los conceptos basicos utilizados en EzSaveGame y establece la terminologia empleada en el resto de la guia.

### 4.1 Slot
Un slot es un contenedor nombrado que representa un estado de guardado. Cada slot incluye:
- **Payload**: los datos del juego serializados.
- **Metadatos**: propiedades legibles como timestamp, tiempo de juego, nivel, version y etiquetas personalizadas.
- **Backups**: copias historicas generadas automaticamente o manualmente segun la politica de retencion.

Operaciones habituales: crear, guardar, cargar, borrar, enumerar y restaurar/respaldar.

### 4.2 Schema
Un esquema es un contrato legible por maquinas que describe la estructura y las reglas de validacion del payload. Permite validar datos antes de guardarlos o despues de cargarlos, genera formularios automaticamente en el editor y define trayectorias deterministas de migracion entre versiones.

### 4.3 Adapter
Un adaptador es la implementacion de serializacion para un formato concreto (JSON, TOML, YAML, XML). Debe conservar metadatos de esquema, emitir errores claros y mantener un orden determinista cuando se necesitan diffs estables.

### 4.4 Provider (Storage Provider)
Encapsula la persistencia fisica. Ejemplos: `LocalFilesystemProvider`, `InMemoryProvider`, `CloudProvider`. Expone operaciones `Save/Load/Delete/List`, gestiona rutas seguras por plataforma y garantiza escrituras atomicas.

### 4.5 Save Manager
Servicio central que coordina validacion, serializacion, encriptacion y almacenamiento. Exponer APIs y nodos Blueprint para QuickSave/QuickLoad, enumerar slots y manipular backups.

### 4.6 Slot Manager
Helper que mantiene metadatos por slot, aplica politicas de retencion (numero de backups, caducidad) y programa mantenimiento en background (limpieza, compactacion).

### 4.7 Schema Generator
Herramienta que inspecciona clases derivadas de `USaveGame` y genera un esquema inicial con tipos, valores por defecto y stubs de validacion listos para refinar en el editor.

### 4.8 Validacion
Garantiza que el payload respete el esquema activo. Incluye fases de pre-guardado (evitar corruptelas) y post-carga (detectar incompatibilidades o campos faltantes). Soporta validadores nativos (requerido, min/max, regex) o personalizados.

### 4.9 Migracion
Transforma payloads antiguos al esquema actual durante la carga. Puede ser automatica (renombres simples, valores por defecto, ampliacion de tipos) o personalizada mediante callbacks registrados en el Schema Generator.

### 4.10 Metadatos y Versionado
Cada save almacena ID de esquema, version, formato, timestamp y tags opcionales (p.ej., identificadores de nube). Estos datos habilitan comprobaciones de compatibilidad, UI rica (miniaturas, etiquetas) y decisiones de migracion.

### 4.11 Escrituras Atomicas y Backups
EzSaveGame escribe a archivos temporales y renombra para evitar guardados parciales. Ademas, conserva un numero configurable de backups por slot para revertir corrupciones.

### 4.12 QuickSave / QuickLoad y Auto-Save
- **QuickSave/QuickLoad**: acciones rapidas ligadas a un slot por defecto para el jugador.
- **Auto-Save**: guardados periodicos no bloqueantes que respetan validacion y politicas de backup.

### 4.13 Limitaciones y Buenas Practicas
- Evitar blobs binarios grandes en el payload; almacenar referencias externas.
- Mantener una frecuencia de guardado razonable para reducir I/O.
- Probar migraciones en entornos de integracion antes de lanzar actualizaciones de esquema.

---

## 5. REFERENCIA COMPLETA DE API
Esta seccion documenta la superficie publica disponible en C++ y Blueprints, describe patrones asincronos, manejo de errores y ejemplos rapidos.

### 5.1 Panorama General
Componentes clave:
- Save Manager (servicio runtime)
- Slot Manager (metadatos y retencion)
- Serialization Adapters (implementaciones por formato)
- Storage Providers (persistencia fisica)
- Schema Generator y Validators
- Key Provider (encriptacion)

### 5.2 Convenciones
- Clases C++ usan prefijos `UEz` o `MotaEz`.
- Nodos Blueprint se agrupan en la categoria `EzSave` con verbos claros (`EzSave_SaveSlot`, `EzSave_LoadSlot`, etc.).
- Errores: funciones retornan `bool` + parametro de salida o delegados con `FEzSaveResult` (incluye codigo y mensaje).

### 5.3 Clases y Structs Principales
- `UEzSaveManager`
  - `SaveSlot / SaveSlotAsync`
  - `LoadSlot / LoadSlotAsync`
  - `EnumerateSlots`
  - `DeleteSlot`
- `FEzSaveOptions`
  - `FormatOverride`, `bEncrypt`, `bCreateBackup`, `MetadataOverrides`
- `FEzLoadOptions`
  - `bValidate`, `bRunMigrations`, `DesiredFormat`
- `FEzSlotInfo`
  - `SlotName`, `Timestamp`, `SchemaVersion`, `PlaytimeSeconds`, `CustomTags`
- `IEzStorageProvider`
  - `Save/Load/Delete/List`
- `IEzSerializationAdapter`
  - `Serialize/Deserialize`
- `IEzKeyProvider`
  - `GetKey`, `RotateKey`

### 5.4 API Blueprint
Nodos disponibles:
- `EzSave Init`
- `EzSave Save Slot (Sync/Async)`
- `EzSave Load Slot (Sync/Async)`
- `EzSave Enumerate Slots`
- `EzSave Delete Slot`
- `EzSave Create Backup` / `EzSave Restore Backup`
- `EzSave Validate Slot`

Los structs `FEzSaveOptions_BP` y `FEzSaveResult_BP` reflejan sus equivalentes en C++.

### 5.5 Patrones Asincronos
- Delegados como `FOnEzSaveComplete` notifican al finalizar y entregan `FEzSaveResult`.
- Nodos asincronos ofrecen pins `Completed` y `Progress` (float 0..1) para mostrar avances.

### 5.6 Eventos y Notificaciones
- `OnSaveCompleted`
- `OnLoadCompleted`
- `OnValidationReport`
- `OnProviderError`

Se pueden enlazar desde C++ o Blueprints para monitorizar el sistema.

### 5.7 Codigos de Error
La enumeracion `FEzErrorCode` incluye valores como `None`, `InvalidPayload`, `SchemaMismatch`, `MigrationFailed`, `SerializationError`, `ProviderWriteFailed`, `ProviderReadFailed`, `DecryptionFailed`, `PermissionDenied`, `Unknown`.

`FEzSaveResult` expone `bSuccess`, `ErrorCode`, `Message` y, opcionalmente, problemas de validacion por campo.

### 5.8 Ejemplos Rapidos
```cpp
UEzSaveManager* Manager = GetGameInstance()->GetSubsystem<UEzSaveManager>();
FEzSaveOptions Options; Options.bEncrypt = false; Options.bCreateBackup = true;
Manager->SaveSlot(TEXT("Player_01"), SaveObject, Options);
```

```cpp
Manager->SaveSlotAsync(TEXT("Player_01"), SaveObject, Options,
    FOnEzSaveComplete::CreateLambda([](const FEzSaveResult& Result)
    {
        if (Result.bSuccess)
        {
            UE_LOG(LogTemp, Log, TEXT("Guardado completado"));
        }
    }));
```

Blueprint (QuickSave): usar `EzSave Save Slot (Async)`, enlazar `Completed` y mostrar feedback segun `Result.Success`.

### 5.9 Extension del API
- **Storage Provider**: implementar `IEzStorageProvider`, registrar con `EzSave::RegisterStorageProvider`, seleccionar en Project Settings.
- **Adapter personalizado**: implementar `IEzSerializationAdapter` y registrar via `EzSave::RegisterAdapter()` para nuevos formatos.

### 5.10 Compatibilidad y Deprecaciones
EzSaveGame sigue versionado semantico. APIs marcadas como obsoletas permanecen al menos una version menor con `UE_DEPRECATED` y notas en el changelog.

---

## 6. GUIA DE BLUEPRINTS
Describe como integrar EzSaveGame desde Blueprints, incluyendo inicializacion, nodos disponibles, patrones de uso y depuracion.

### 6.1 Puesta en Marcha
- Llama a `EzSave Init` durante el arranque (GameInstance -> Event Init recomendado).
- Configura formato por defecto, encriptacion y otras opciones globales.
- Registra key/storage providers personalizados en C++ antes o expone wrappers Blueprint.

Patron sugerido:
1. `EzSave Init` al iniciar el juego.
2. Leer preferencias del jugador (slot actual, auto-save) y aplicarlas via nodos de configuracion.

### 6.2 Nodos Disponibles
En la categoria `EzSave`:
- `EzSave Init`
- `EzSave Save Slot (Sync/Async)`
- `EzSave Load Slot (Sync/Async)`
- `EzSave Enumerate Slots`
- `EzSave Delete Slot`
- `EzSave Create Backup`
- `EzSave Restore Backup`
- `EzSave Validate Slot`

### 6.3 Guardar desde Blueprints
Flujo recomendado (Async):
1. Evento (boton, trigger) -> `EzSave Save Slot (Async)`.
2. Parametros: `SlotName`, `SaveObject`, `Options` (activar `CreateBackup` si procede).
3. `Completed`: revisar `Result.Success`, mostrar UI de confirmacion o error.
4. `Progress`: opcional para barras de progreso.

### 6.4 Cargar desde Blueprints
1. `EzSave Load Slot (Async)` con `SlotName` y objeto destino.
2. `Completed`: manejar `Result.Success`, validar problemas reportados y actualizar UI/jugabilidad.

### 6.5 Enumerar y Administrar Slots
- `EzSave Enumerate Slots` devuelve `FEzSlotInfo` para poblar listas.
- Agrega acciones de Cargar/Borrar por cada fila y muestra metadatos relevantes.

### 6.6 Backups
- `EzSave Create Backup` para snapshots manuales o tras hitos.
- `EzSave Restore Backup` para revertir a un backup especifico.

### 6.7 Validacion y Reportes
- Usa `EzSave Validate Slot` antes de guardados criticos o en herramientas de QA.
- Presenta la lista de problemas (campo, tipo, mensaje) en UI o logs.

### 6.8 Auto-Save y QuickSave
- **Auto-Save**: timer recurrente (ej. cada 300 s) que ejecuta `EzSave Save Slot (Async)`; deshabilitar durante cinematicas.
- **QuickSave**: asignar tecla (F5) a `EzSave Save Slot (Async)` contra el slot predeterminado; mostrar notificacion breve.

### 6.9 Exponer Datos
- Utiliza clases `USaveGame` o `USTRUCT(BlueprintType)` como contenedores de datos.
- Evita guardar grafos de actores enteros; usa referencias suaves (`FSoftObjectPath`) o GUIDs.

### 6.10 Depuracion
- Activar `LogEzSave` en Verbose en `DefaultEngine.ini`.
- Inspeccionar archivos resultantes (mientras no haya encriptacion) para verificar contenido.
- Invocar `EzSave Validate Slot` en editor para detectar problemas antes de runtime.

### 6.11 Manejo de Errores
- Siempre revisar el struct `Result`.
- Implementar reintentos para fallos de I/O temporales.
- Aplicar fallbacks: cargar el ultimo backup o restablecer valores por defecto.

### 6.12 Consideraciones Multijugador
- No depender de saves cliente para estado compartido.
  - Guardar progreso personal en el cliente.
  - Persistir estado global en el servidor.
  - Sincronizar metadatos mediante RPCs seguros para evitar manipulaciones.

### 6.13 Herramientas de Editor
- El editor de esquemas brinda paneles UI para preparar formularios basados en schema.
- Usa editor utilities para validar datos sin necesidad de ejecutar el juego.

### 6.14 Flujos Ejemplo
- **QuickSave**: boton -> `EzSave Save Slot (Async)` -> `Completed` -> notificacion.
- **Auto-Save**: timer -> `EzSave Save Slot (Async)` en slot designado.
- **Menu de Slots**: `EzSave Enumerate Slots` -> construir UI -> botones para cargar/borrar/copia.

---

## 7. GUIA DE C++
### 7.1 Configuracion del Modulo
Para acceder a EzSaveGame desde C++:
- Agrega `EzSaveGameRuntime` a `PublicDependencyModuleNames` o `PrivateDependencyModuleNames` en tu archivo `.Build.cs`.
- Incluye los headers correspondientes (por ejemplo, `MotaEzSaveGameLibrary.h`, `MotaEzSlotManager.h`).
- Inicializa el subsystem cuando el juego arranque (GameInstance o modulo dedicado).

Ejemplo en `YourProject.Build.cs`:
```csharp
PublicDependencyModuleNames.AddRange(
    new string[] { "Core", "CoreUObject", "Engine", "EzSaveGameRuntime" }
);
```

### 7.2 Clases Clave
- `UEzSaveManager`: subsystem principal con `SaveSlot`, `LoadSlot`, versiones asincronas, enumeracion y borrado.
- `UMotaEzSaveGameLibrary`: helpers estaticos para operaciones rapidas desde cualquier contexto.
- `UMotaEzSlotManager`: manejo de slots por indice (Save/Load/Delete/QuickSave).
- `FEzSaveOptions` y `FEzLoadOptions`: opciones de formato, encriptacion, backups y validacion.
- `FEzSlotInfo`: metadatos consumibles por UI.

### 7.3 Guardado/Carga en C++
```cpp
UEzSaveManager* Manager = GetGameInstance()->GetSubsystem<UEzSaveManager>();
FEzSaveOptions Options;
Options.bEncrypt = true;
Options.bCreateBackup = true;

Manager->SaveSlot(TEXT("Player_01"), SaveObject, Options);
```

```cpp
FEzLoadOptions LoadOptions;
LoadOptions.bValidate = true;
LoadOptions.bRunMigrations = true;

Manager->LoadSlot(TEXT("Player_01"), SaveObject, LoadOptions);
```

Usa `SaveSlotAsync`/`LoadSlotAsync` para evitar bloqueos del game thread.

### 7.4 Manejo de Errores
- Verifica el resultado booleano y el `FEzSaveResult` asociado.
- Registra `Result.ErrorCode` y `Result.Message` para diagnosticos.
- Implementa reintentos para fallos transitorios del proveedor.

### 7.5 Consejos de Depuracion (C++)
- Habilita logging verbose para EzSaveGame estableciendo `LogEzSave` a `Verbose` en `DefaultEngine.ini`.
- Inspecciona los archivos de guardado generados en el directorio de guardados del proyecto. Los archivos son legibles para formatos JSON/TOML/YAML/XML a menos que esten encriptados.
- Usa la utilidad del editor de esquemas para validar y previsualizar esquemas fuera del gameplay.

### 7.6 Mejores Practicas (C++)
- Prefiere llamadas asincronas de guardado/carga en codigo de gameplay para evitar caidas de frames.
- Usa el patron asincrono basado en delegados para flujos de trabajo complejos que necesitan encadenar acciones al completar guardado/carga.
- Prueba regularmente la funcionalidad de guardado/carga durante todo el desarrollo, especialmente despues de cambios en el esquema o en el gameplay principal.

---

## 8. SISTEMA DE ESQUEMAS
### 8.1 Descripcion General
Los esquemas definen la estructura y reglas de validacion de los datos guardados. Permiten validaciones tempranas, documentacion automatica y migraciones controladas.

### 8.2 Lenguaje de Definicion
Ejemplo JSON:
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

### 8.3 Edicion y Herramientas
- Editor visual: crear/editar campos, restricciones y metadatos.
- Generador: inspecciona clases `USaveGame` y produce un esquema base listo para refinar.

### 8.4 Validacion
- Automatizada en cada guardado/carga.
- Ejecutable manualmente via `EzSave Validate Slot`.
- Los reportes listan campo, tipo de error y mensaje sugerido.

### 8.5 Migracion
- Detecta versiones antiguas y aplica transformaciones.
- Configura callbacks en el generador o implementalas manualmente cuando haya cambios importantes.

### 8.6 Buenas Practicas
- Versiona y documenta esquemas.
- Testea migraciones en staging antes de publicar.
- Usa descripciones claras para ayudar a futuros contribuyentes.

---

## 9. SISTEMA DE SLOTS
### 9.1 Descripcion
Permite multiples estados de guardado por usuario. Cada slot combina datos y metadatos para mostrarlos en interfaces.

### 9.2 Metadatos
Campos habituales: nombre, timestamp, tiempo de juego, nivel, `SchemaVersion`, `CustomTags`. Puedes anadir thumbnails o banderas personalizadas.

### 9.3 API de Slots
- `UMotaEzSlotManager::GetAllSlots` -> listado con `FMotaEzSlotInfo`.
- `SaveToSlot/LoadFromSlot/DeleteSlot` -> indices numericos.
- `QuickSaveToSlot/QuickLoadFromSlot` -> operaciones sobre el ultimo slot usado.

### 9.4 Buenas Practicas
- Confirmar sobrescrituras.
- Proveer metadatos utiles (nivel, dificultad, progreso).
- Limpiar slots antiguos o backups obsoletos para reducir espacio.

---

## 10. VALIDACION DE DATOS
### 10.1 Objetivo
Garantizar integridad de los datos, detectar discrepancias de esquema y evitar que payloads corruptos lleguen a disco.

### 10.2 Reglas
- Requerido
- Tipo
- Rango (Min/Max)
- Regex
- Longitud maxima/minima

### 10.3 Validadores Personalizados
Implementa la interfaz de validacion para reglas especificas (coherencia de inventario, misiones activas vs completadas, etc.).

### 10.4 Recomendaciones
- Ejecuta validaciones exhaustivas en QA.
- Registra mensajes con instrucciones de remediacion.
- Clasifica errores en recuperables vs fatales para reaccionar adecuadamente.

---

## 11. ENCRIPTACION
### 11.1 Vision General
EzSaveGame puede cifrar payloads con AES-256-GCM y, opcionalmente, firmarlos con HMAC para detectar manipulaciones.

### 11.2 Activacion
- Ajusta el modo en Project Settings (Disabled/Editor/Production).
- Usa `FEzSaveOptions.bEncrypt` para forzar encriptacion por operacion.

### 11.3 Key Providers
- Implementa `IEzKeyProvider` para obtener llaves de keystores o secretos seguros.
- Soporta rotacion e invalidacion de llaves.

### 11.4 Buenas Practicas
- No empaquetes llaves reales en builds publicas.
- Usa el provider de debug solo en editor.
- Documenta procedimientos de rotacion y respuesta ante brechas.

---

## 12. FORMATOS DE ARCHIVO
### 12.1 Formatos disponibles
- JSON
- TOML
- YAML
- XML

### 12.2 Como elegir
Basate en legibilidad, tooling externo y requisitos de interoperabilidad. JSON/TOML para la mayoria de los casos, YAML si requieres comentarios, XML si lo exige un pipeline heredado.

### 12.3 Recomendaciones
- Mantener un formato consistente por ambiente.
- Documentar ubicacion y extension de los archivos generados.
- Evitar cambiar de formato una vez lanzado el juego salvo necesidad.

---

## 13. EJEMPLOS PRACTICOS
Esta seccion proporciona ejemplos practicos y fragmentos de codigo para tareas comunes usando EzSaveGame. Cubre el guardado y carga de datos, gestion de slots, manejo de errores y uso de nodos Blueprint.

### 13.1 Guardado y Carga (Blueprints)
Para guardar datos:
1. Crea u obten un objeto `SaveGame` que contenga los datos a guardar.
2. Llama al nodo `EzSave Save Slot (Async)` con:
   - SlotName: el nombre del slot donde guardar.
   - SaveObject: el objeto `SaveGame`.
   - Options: opciones de guardado (ej: encriptacion, backup).
3. Conecta al evento Completed para manejar el exito o fallo.

Para cargar datos:
1. Llama al nodo `EzSave Load Slot (Async)` con:
   - SlotName: el nombre del slot desde donde cargar.
   - Target: el objeto `SaveGame` a poblar.
   - Options: opciones de carga (ej: validacion, migracion).
2. Conecta al evento Completed para manejar el exito o fallo.

### 13.2 Guardado y Carga (C++)
Para guardar datos:
```cpp
UEzSaveManager* Manager = GetGameInstance()->GetSubsystem<UEzSaveManager>();
FEzSaveOptions Options; Options.bEncrypt = true; Options.bCreateBackup = true;
Manager->SaveSlot(TEXT("Player_01"), MySaveGameObject, Options);
```
Para cargar datos:
```cpp
UEzSaveManager* Manager = GetGameInstance()->GetSubsystem<UEzSaveManager>();
FEzLoadOptions Options; Options.bValidate = true; Options.bRunMigrations = true;
Manager->LoadSlot(TEXT("Player_01"), MySaveGameObject, Options);
```

### 13.3 Gestion de Slots
Para enumerar slots:
- Llama a `EzSave Enumerate Slots` para obtener una lista de todos los slots y sus metadatos.

Para eliminar un slot:
- Llama a `EzSave Delete Slot` con el nombre del slot.

Para crear un backup:
- Llama a `EzSave Create Backup` para crear un backup inmediato del slot actual.

Para restaurar un backup:
- Llama a `EzSave Restore Backup` con los metadatos del backup.

### 13.4 Manejo de Errores
En Blueprints:
- Verifica la estructura Result al completar los nodos de guardado/carga.
- Implementa logica de reintento o acciones alternativas basadas en el codigo de error.

En C++:
- Verifica el valor de retorno de las funciones de guardado/carga.
- Inspecciona la estructura result para detalles del error y maneja en consecuencia.

### 13.5 Consejos de Depuracion
- Habilita logging verbose para EzSaveGame en `DefaultEngine.ini`.
- Inspecciona los archivos de guardado generados para verificar legibilidad y correccion.
- Usa la utilidad del editor de esquemas para validar esquemas y previsualizar datos.

---

## 14. CASOS DE USO
Esta seccion explora casos de uso comunes para EzSaveGame y proporciona orientacion sobre como implementar estos escenarios.

### 14.1 Guardado/Carga Simple
Para funcionalidad basica de guardado/carga:
- Crea una clase derivada de `USaveGame` para contener los datos.
- Usa los nodos o funciones `EzSave Save Slot` y `EzSave Load Slot` para guardar y cargar los datos.

### 14.2 QuickSave/QuickLoad
Para implementar QuickSave/QuickLoad:
- Vincula una tecla (ej: F5) a una funcion Blueprint o C++ que llame a `EzSave Save Slot (Async)` en un slot por defecto.
- Usa `EzSave Load Slot (Async)` para cargar el guardado rapido.

### 14.3 Auto-Guardado
Para funcionalidad de auto-guardado:
- Configura un temporizador en tu instancia de juego o estado que llame a `EzSave Save Slot (Async)` a intervalos regulares.
- Asegurate de que el slot de auto-guardado este configurado y que los backups esten habilitados.

### 14.4 Gestion de Slots
Para gestionar multiples slots de guardado:
- Usa `EzSave Enumerate Slots` para listar todos los slots disponibles.
- Proporciona una UI para que el jugador seleccione un slot para cargar o eliminar.
- Usa metadatos (timestamp, playtime) para mostrar informacion del slot en la UI.

### 14.5 Migracion de Datos
Para migrar datos entre versiones de esquema:
- Define funciones de migracion en tu modulo generador de esquemas.
- Implementa verificaciones de version y migracion automatica durante el proceso de carga.

---

## 15. OPTIMIZACION Y RENDIMIENTO
Esta seccion proporciona orientacion sobre como optimizar el rendimiento de guardado/carga y reducir el impacto de EzSaveGame en el tiempo de ejecucion del juego.

### 15.1 Consejos Generales de Optimizacion
- Prefiere operaciones de guardado/carga asincronas para evitar bloquear el hilo del juego.
- Usa el patron asincrono basado en delegados para flujos de trabajo complejos y minimizar latencia.
- Perfilar regularmente el rendimiento de guardado/carga y optimiza basandote en los hallazgos.

### 15.2 Reducir el Tamaño de Archivos de Guardado
Para reducir el tamaño de los archivos de guardado:
- Excluye datos innecesarios del payload de guardado. Solo guarda lo que es necesario.
- Usa opciones de compresion en el proveedor de almacenamiento si estan disponibles.
- Revisa y limpia regularmente slots de guardado antiguos o no usados y backups.

### 15.3 Monitoreo de Rendimiento
Para monitorear el rendimiento:
- Usa las herramientas de perfilado integradas de Unreal (ej: Session Frontend, Profiler) para medir el impacto de las operaciones de guardado/carga.
- Monitorea I/O de disco, uso de memoria y uso de CPU durante guardado/carga para identificar cuellos de botella.

---

## 16. COMANDOS DE CONSOLA
Esta seccion documenta los comandos de consola proporcionados por EzSaveGame para depuracion y automatizacion.

### 16.1 Lista de Comandos de Consola
- `EzSave.ListSlots`: Lista todos los slots de guardado y sus metadatos.
- `EzSave.DeleteSlot <SlotName>`: Elimina el slot de guardado especificado.
- `EzSave.QuickSave`: Realiza un guardado rapido en el slot por defecto.
- `EzSave.QuickLoad`: Realiza una carga rapida desde el slot por defecto.
- `EzSave.ValidateSlot <SlotName>`: Valida el slot de guardado especificado contra el esquema.

---

## 17. MIGRACION Y ACTUALIZACION
Esta seccion proporciona orientacion sobre la migracion entre versiones de EzSaveGame y la actualizacion de tu proyecto para usar nuevas versiones del plugin.

### 17.1 Migrar entre Versiones del Plugin
Para migrar entre versiones del plugin:
1. Respalda tu proyecto y tus guardados.
2. Actualiza la carpeta del plugin en `Plugins/` a la nueva version.
3. Reinicia el Unreal Editor.
4. Ejecuta la utilidad de validacion para verificar discrepancias de esquema/version.

### 17.2 Actualizar el Plugin
Al actualizar el plugin:
1. Lee las notas de version para cambios incompatibles.
2. Respalda tus guardados existentes (copia la carpeta de guardados a una ubicacion segura).
3. Actualiza la carpeta del plugin en `Plugins/` y reinicia el editor.
4. Ejecuta la utilidad de validacion incluida con el plugin para escanear guardados existentes en busca de discrepancias de esquema/version.

---

## 18. SOLUCION DE PROBLEMAS
Esta seccion proporciona consejos de solucion de problemas para incidencias comunes encontradas al usar EzSaveGame.

### 18.1 Problemas Comunes y Soluciones
- Problema: El plugin no es visible en el navegador de Plugins.
  - Solucion: Asegurate de que la carpeta del plugin contenga un archivo `.uplugin` valido y que la ruta sea `Proyecto/Plugins/EzSaveGame`.
- Problema: El editor falla al compilar los modulos del plugin.
  - Solucion: Confirma que tu cadena de herramientas de Visual Studio y la version de Unreal Engine coincidan con la configuracion esperada del compilador del plugin.
- Problema: Errores de permisos al guardar durante play-in-editor.
  - Solucion: Verifica que la ruta configurada para guardados tenga permisos de escritura para el proceso del editor.

### 18.2 Consejos de Depuracion
- Habilita logging verbose estableciendo la categoria `LogEzSave` a `Verbose` en `DefaultEngine.ini` durante el desarrollo.
- Inspecciona los archivos de guardado generados en la ruta de guardados del proyecto. Los archivos son legibles para formatos JSON/TOML/YAML/XML a menos que esten encriptados.
- Usa el nodo `EzSave Validate Slot` para detectar problemas de esquema antes de intentar cargar en juego.
- Para payloads malformados, el error de deserializacion incluye una ruta detallada al campo que falla — registra eso en tu UI o consola.

---

## 19. MEJORES PRACTICAS
Esta seccion proporciona mejores practicas para usar EzSaveGame de manera efectiva y evitar problemas comunes.

### 19.1 Mejores Practicas Generales
- Prueba regularmente la funcionalidad de guardar/cargar durante todo el desarrollo, especialmente despues de cambios en el esquema o en el gameplay principal.
- Usa control de versiones para tu proyecto e incluye el directorio de guardados para rastrear cambios en los datos guardados.
- Documenta tu esquema y la estructura del save game para facilitar la depuracion y el desarrollo futuro.

### 19.2 Mejores Practicas de Rendimiento
- Prefiere operaciones asincronas de guardado/carga para evitar bloquear el hilo del juego.
- Utiliza el patron asincrono basado en delegados para flujos de trabajo complejos y minimizar la latencia.
- Perfila regularmente el rendimiento de guardado/carga y optimiza segun los hallazgos.

### 19.3 Mejores Practicas de Seguridad
- Usa siempre encriptacion en builds de produccion para proteger los datos del usuario.
- Rota regularmente las claves de encriptacion y actualiza cualquier sistema o configuracion dependiente.
- Monitorea y responde a cualquier error de encriptacion o gestion de claves.

---

## 20. PREGUNTAS FRECUENTES
Esta seccion proporciona respuestas a las preguntas mas frecuentes sobre EzSaveGame.

### 20.1 ¿Que plataformas soporta EzSaveGame?
EzSaveGame soporta todas las plataformas que Unreal Engine 5.6+ soporta, incluyendo Windows, macOS, Linux, iOS, Android y consolas.

### 20.2 ¿Puedo usar EzSaveGame solo con Blueprints?
Si, EzSaveGame esta diseñado para ser completamente usable desde Blueprints. Toda la funcionalidad principal esta expuesta mediante nodos Blueprint.

### 20.3 ¿Necesito saber C++ para usar EzSaveGame?
No, puedes usar EzSaveGame completamente desde Blueprints. Sin embargo, conocer C++ puede ayudarte a extender el plugin o integrarlo mas profundamente con tu juego.

### 20.4 ¿Hay un limite en el numero de slots de guardado?
No, pero gestionar un numero muy grande de slots puede impactar el rendimiento. Enumera y limpia regularmente los slots antiguos o no utilizados para mantener el rendimiento.

### 20.5 ¿Puedo cambiar el formato de serializacion mas adelante?
Si, puedes cambiar el formato de serializacion actualizando la configuracion del proyecto o en tiempo de ejecucion mediante la estructura `FEzSaveOptions`. Los datos existentes se migraran al nuevo formato si es necesario.

### 20.6 ¿Como funciona la encriptacion?
La encriptacion es opcional y utiliza AES-256-GCM para cifrar el payload serializado. Una implementacion de proveedor de claves de la interfaz `IEzKeyProvider` suministra la clave de encriptacion.

### 20.7 ¿Que debo hacer si encuentro un error?
Primero, consulta la documentacion y las FAQ del plugin para encontrar soluciones. Si el problema persiste, contacta al autor del plugin con informacion detallada sobre el error, incluyendo los pasos para reproducirlo.

### 20.8 ¿Donde puedo obtener soporte?
Para soporte, contacta al autor del plugin o visita el foro/pagina de soporte del plugin. Proporciona informacion detallada sobre tu problema, incluyendo la version de Unreal Engine, version del plugin y pasos para reproducir el problema.

---

## AUTOR Y CREDITOS

Esta guia de usuario y el plugin EzSaveGame fueron creados por José Antonio Mota Lucas.

- Autor: José Antonio Mota Lucas
- LinkedIn: https://www.linkedin.com/in/joseantoniomotalucas/

### Bibliotecas de Terceros y Creditos
EzSaveGame incluye y utiliza varias bibliotecas de terceros para el parseo y la serializacion. Agradecemos el trabajo de sus autores y mantenedores:

- tinyxml2 — Parser XML ligero. Repositorio: https://github.com/leethomason/tinyxml2
  - Licencia: Ver `Source/ThirdParty/tinyxml2/LICENSE.txt` en este repositorio (licencia permisiva estilo zlib/libpng).

- yaml-cpp — Parser y emisor YAML para C++. Creado por Jesse Beder. Repositorio: https://github.com/jbeder/yaml-cpp
  - Licencia: MIT (ver `Source/ThirdParty/yaml-cpp/LICENSE`).

- tomlplusplus (toml++) — Parser TOML para C++ moderno. Repositorio: https://github.com/marzer/tomlplusplus
  - Licencia: MIT (ver el encabezado `Source/ThirdParty/tomlplusplus/toml.hpp` y la licencia del proyecto).

Estas bibliotecas siguen siendo propiedad de sus respectivos autores. Sus licencias y avisos de copyright estan incluidos en la carpeta `Source/ThirdParty/` de este plugin — por favor consulta esos archivos para los terminos completos de licencia.

---

