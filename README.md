# MotaPluginShowcase

**Showcase Interactivo de Patrones de Diseño - Plugin EzSaveGame para Unreal Engine 5.6**

## Descripción

Este proyecto es un showcase interactivo que demuestra la implementación práctica de ocho patrones de diseño clásicos aplicados al desarrollo de videojuegos. Desarrollado en Unreal Engine 5.6 con C++, el proyecto presenta un hub central desde el cual el jugador puede acceder a siete niveles independientes, cada uno demostrando un patrón de diseño específico en el contexto de un sistema de persistencia de datos.

## Características Principales

- **8 Patrones de Diseño Implementados**: Singleton, Strategy, Facade, Factory Method, Template Method, Adapter, Composite y Decorator
- **7 Niveles Interactivos**: Cada nivel demuestra un patrón específico con ejemplos visuales y funcionales
- **Plugin EzSaveGame**: Sistema completo de guardado con soporte para múltiples formatos de serialización
- **Arquitectura Modular**: Código encapsulado y reutilizable siguiendo principios SOLID
- **Documentación Completa**: Memoria técnica exhaustiva y documentación del plugin incluidas

## Tecnologías Utilizadas

- **Motor**: Unreal Engine 5.6
- **Lenguaje**: C++ con Unreal Engine API
- **Formatos de Serialización**: JSON (RapidJSON), TOML (toml++), YAML (yaml-cpp), XML (tinyxml2)
- **Encriptación**: AES-256 (disponible en builds finales)

## Estructura del Proyecto

```
MotaPluginShowcase/
├── Content/
│   ├── ThirdPerson/           # Main Hall (hub central)
│   └── Levels/                # 7 niveles de demostración
├── Source/
│   └── MotaPluginShowcase/    # Código C++ de los actores de demostración
├── Plugins/
│   └── EzSaveGame/            # Plugin de persistencia de datos
├── Saved/
│   └── SaveGames/             # Archivos de guardado generados
└── Memoria_MotaPluginShowcase.md  # Documentación técnica completa
```

## Patrones de Diseño Demostrados

### 1. Singleton
Gestor global de estadísticas con acceso único y compartido.
- **Nivel**: 1
- **Demostración**: Contador compartido entre múltiples instancias
- **Slot de Guardado**: Slot10.json

### 2. Strategy
Cambio dinámico entre algoritmos de serialización.
- **Nivel**: 2
- **Demostración**: Selección entre JSON, TOML, YAML y XML
- **Slot de Guardado**: Slot2.json/toml/yaml/xml

### 3. Facade
Simplificación de operaciones complejas mediante interfaz unificada.
- **Nivel**: 3
- **Demostración**: API simplificada para operaciones de guardado
- **Slot de Guardado**: Slot3.json

### 4. Factory Method
Generación automática de schemas mediante introspección.
- **Nivel**: 4
- **Demostración**: Creación de JSON Schema y XML Schema
- **Slot de Guardado**: Slot4.json

### 5. Template Method
Algoritmo de validación con pasos fijos y personalizables.
- **Nivel**: 5
- **Demostración**: Validación de datos en 5 pasos
- **Slot de Guardado**: Slot5.json

### 6. Adapter
Integración de múltiples bibliotecas bajo interfaz común.
- **Nivel**: 6
- **Demostración**: 4 bibliotecas de serialización adaptadas
- **Slot de Guardado**: Slot6.json

### 7. Composite
Gestión jerárquica de estructuras de guardado.
- **Nivel**: 7
- **Demostración**: Slots compuestos (Datos + Metadata + Screenshot)
- **Slots de Guardado**: Slot0.json y Slot1.json

### 8. Decorator
Añadir funcionalidad de encriptación sin modificar código base.
- **Implementación**: Solo en plugin (sin nivel dedicado)
- **Demostración**: Encriptación AES-256 (disponible en builds finales)
- **Nota**: No funcional en el editor de Unreal Engine

## Requisitos

- Unreal Engine 5.6
- Visual Studio 2022 (para compilación de C++)
- Windows 10/11 (64-bit)

## Instalación

1. Clonar el repositorio:
   ```bash
   git clone https://github.com/usuario/MotaPluginShowcase.git
   ```

2. Abrir `MotaPluginShowcase.uproject` con Unreal Engine 5.6

3. Compilar el proyecto (Unreal generará automáticamente los archivos de Visual Studio)

4. Presionar Play para iniciar el showcase

## Uso

1. **Inicio**: El juego inicia en el Main Hall con un panel de estadísticas central
2. **Navegación**: Caminar hacia los portales de teletransporte para acceder a cada nivel
3. **Demostración**: Entrar en los triggers (cajas transparentes) para activar las funcionalidades
4. **Feedback**: Observar los mensajes en pantalla y el texto 3D para ver los resultados
5. **Verificación**: Los datos guardados se encuentran en `Saved/SaveGames/`

## Controles

- **W/A/S/D**: Movimiento
- **Ratón**: Cámara
- **Espacio**: Saltar
- **Entrar en trigger**: Activar funcionalidad del patrón

## Documentación

- **Memoria Técnica**: `Memoria_MotaPluginShowcase.md` - Documentación completa del proyecto (aprox. 35,000 palabras)
- **Documentación del Plugin**: `Plugins/EzSaveGame/Documentation/` - Referencia técnica del plugin

## Archivos de Guardado

Los archivos se guardan en `Saved/SaveGames/`:

- `SlotMain.json` - Estadísticas globales del showcase
- `Slot10.json` - Singleton (contador)
- `Slot2.json/toml/yaml/xml` - Strategy (formato variable)
- `Slot3.json` - Facade (puntuación)
- `Slot4.json` - Factory Method (schema generado)
- `Slot5.json` - Template Method (resultados de validación)
- `Slot6.json` - Adapter (contador de tests)
- `Slot0.json` + `Slot0_Metadata.json` - Composite (principal)
- `Slot1.json` + `Slot1_Metadata.json` - Composite (backup)

## Características Técnicas

### Plugin EzSaveGame

El plugin proporciona:

- Guardado/carga de datos con API simplificada
- Soporte para 4 formatos de serialización (JSON, TOML, YAML, XML)
- Sistema de slots con metadata
- Encriptación AES-256 (builds finales)
- Validación de datos
- Generación automática de schemas

### Arquitectura

El proyecto sigue una arquitectura en capas:

1. **Capa de Presentación**: Actores de demostración en cada nivel
2. **Capa de Fachada**: API simplificada del plugin
3. **Capa de Lógica de Negocio**: Subsistemas y managers
4. **Capa de Adaptadores**: Integración de bibliotecas externas
5. **Capa de Persistencia**: Sistema de archivos

## Autor

**Jose Antonio Mota Lucas**

Proyecto desarrollado como parte de la asignatura de Patrones de Diseño en Desarrollo de Videojuegos.

## Licencias

### Proyecto
Este proyecto es de uso educativo y académico.

### Bibliotecas de Terceros
- **RapidJSON**: MIT License
- **toml++**: MIT License
- **yaml-cpp**: MIT License
- **tinyxml2**: Zlib License

## Notas Importantes

- **Logs de Depuración**: El proyecto incluye intencionalmente logs detallados (UE_LOG y mensajes en pantalla) para facilitar la evaluación y comprensión del flujo de ejecución
- **Decorator**: El patrón Decorator (encriptación) solo funciona en builds empaquetados, no en el editor
- **Rendimiento**: El proyecto prioriza claridad y demostración sobre optimización extrema

## Contribuciones

Este es un proyecto académico presentado como trabajo final. No se aceptan contribuciones externas.

## Contacto

Para consultas sobre el proyecto, referirse a la documentación técnica incluida o contactar al autor a través de los canales institucionales.

---

**Fecha**: Enero 2026  
**Motor**: Unreal Engine 5.6  
**Lenguaje**: C++  
**Autor**: Jose Antonio Mota Lucas
