<p align="center">
  <img src="docs/images/banner.png" alt="Raysim Banner" width="720" />
</p>

[![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/Build-CMake-064F8C?style=flat&logo=cmake&logoColor=white)](https://cmake.org/)
![Status](https://img.shields.io/badge/Status-Alpha-orange?style=flat)
[![Version](https://img.shields.io/badge/Version-0.93.0-brightgreen?style=flat)](https://github.com/DMsuDev/Raysim/releases)
[![License](https://img.shields.io/badge/Licencia-Apache%202.0-lightgrey?style=flat)](LICENSE)

[English Readme](https://github.com/DMsuDev/Raysim/blob/main/README.md)
• [Readme Español](https://github.com/DMsuDev/Raysim/blob/main/README.es.md)
• [Changelog](docs/CHANGELOG.md)

Raysim es un framework de C++ para gráficos 2D y aplicaciones interactivas, construido sobre [raylib](https://www.raylib.com/).

Inspirado arquitectónicamente en el [**Hazel Engine de The Cherno**](https://github.com/TheCherno/Hazel), ofrece una API limpia basada en clases para dibujar formas, manejar entrada, gestionar el tiempo y ejecutar simulaciones con paso de tiempo fijo.

Útil para aprender programación gráfica, prototipar ideas o construir pequeños juegos y simulaciones.

Comentarios y contribuciones son bienvenidos.

> **Nota:** Este proyecto está en Alpha. La API puede cambiar sin previo aviso y algunas funcionalidades aún están en implementación.

## Demos Rápidos

<p align="center">
  <img src="docs/gif/Mouse2D.gif" alt="Mouse2D" width="480" /><br />
  <b>Mouse2D</b>
</p>

<p align="center">
  <img src="docs/gif/NoiseLandscape.gif" alt="NoiseLandscape" width="480" /><br />
  <b>NoiseLandscape</b>
</p>

<p align="center">
  <img src="docs/gif/SceneShowcase.gif" alt="SceneShowcase" width="480" /><br />
  <b>SceneShowcase</b>
</p>

| Ejemplo           | Descripción                                                                                                                                                                                                           |
| ----------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `BouncingBalls`   | Simulación de física con gravedad, atracción/repulsión del mouse y generación de bolas. Genera bolas con clic izquierdo, activa o desactiva la gravedad con G y observa cómo colisionan y rebotan contra las paredes. |
| `LissajousCurves` | Visualizador de curvas paramétricas con cambio de fase animado y presets de frecuencia. La curva evoluciona en tiempo real; prueba cambiar presets desde el panel ImGui para ver cómo se transforman los patrones.    |
| `Mouse2D`         | Sigue la posición del mouse con interpolación exponencial suave y dibuja un círculo que lo persigue. Un buen punto de partida para entender el pipeline de entrada y renderizado.                                     |
| `NoiseLandscape`  | Terreno con desplazamiento generado proceduralmente en capas usando ruido Perlin, Simplex, Cellular y Value. Pulsa R para regenerar todas las capas y obtener un paisaje completamente nuevo.                         |
| `SceneShowcase`   | Tres escenas visuales independientes (StarField, PlasmaArt, ClockMandala) accesibles desde un overlay de navegación ImGui compartido. Muestra cómo cambiar escenas en tiempo de ejecución usando el SceneManager.     |

Todos los ejemplos se compilan junto con la librería. Consulta la sección [Compilación](#compilación) para las instrucciones de configuración.

## Bucle de Aplicación

Cada ciclo de la aplicación ejecuta los callbacks del ciclo de vida de la escena activa en orden. Solo sobreescribe los que necesites.

| Callback                 | Cuándo se llama                                                                                                                           |
| ------------------------ | ----------------------------------------------------------------------------------------------------------------------------------------- |
| `OnAttach`               | Una vez, cuando la escena se añade al stack. Carga assets, agrega layers, suscríbete al EventBus.                                         |
| `OnStart`                | Cada vez que la escena se activa (tras `OnAttach` o al reanudar). Reinicia el estado de la simulación.                                    |
| `OnUpdate(dt)`           | Cada frame. Consulta de entrada, lógica del juego, actualizaciones de estado a tasa variable.                                             |
| `OnFixedUpdate(fixedDt)` | Con un paso de tiempo fijo, independiente del frame rate. Integración de física y pasos deterministas.                                    |
| `OnDraw(alpha)`          | Cada frame tras `OnUpdate`. Todos los comandos de renderizado van aquí. `alpha` es un valor en [0, 1) usado para interpolación de física. |
| `OnPause`                | Cuando el SceneManager pausa la escena.                                                                                                   |
| `OnResume`               | Cuando la escena se reanuda desde un estado de pausa.                                                                                     |
| `OnDetach`               | Cuando la escena se elimina. Libera recursos y cancela suscripciones al EventBus.                                                         |

`OnDraw` recibe un factor de interpolación `alpha` para interpolar entre el snapshot de física anterior y el actual, consiguiendo renderizado suave a cualquier frame rate. Poner todo el renderizado en `OnUpdate` también es válido; `alpha` simplemente valdrá 0.

## Módulos

<details>
<summary>Core</summary>

| Símbolo             | Propósito                                                                                                                                                                     |
| ------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Application`       | Clase raíz. Gestiona el bucle principal, el backend, el `SceneManager` y el `EngineContext`. Registra escenas con `RegisterScene<T>()` y actívalas con `ChangeScene<T>()`.    |
| `ApplicationConfig` | Configuración declarativa (título, ancho, alto, nivel de log, máximo de pasos fijos). Se pasa al constructor de `Application`; todos los campos tienen valores por defecto.   |
| `EngineContext`     | Struct plano pasado a cada escena. Contiene punteros no propietarios a `Window`, `Input`, `SceneManager`, `ApplicationConfig` y `EventBus`.                                   |
| `Time`              | Utilidad estática: delta time, paso de tiempo fijo, escala de tiempo, pausa/reanudar, FPS suavizado y `GetRealtimeSinceStartup()`.                                            |
| `Log`               | Wrapper ligero sobre **spdlog**. Escribe en consola y en un archivo de log opcional. Macros: `RS_LOG_TRACE`, `RS_LOG_INFO`, `RS_LOG_WARN`, `RS_LOG_ERROR`, `RS_LOG_CRITICAL`. |
| `FontManager`       | Carga una fuente TTF/OTF una vez con una clave y la referencia globalmente para renderizado de texto. La clave `"default"` se usa cuando no se especifica ninguna.            |
| `BackendFactory`    | Crea instancias concretas de `RendererAPI`, `Window` e `Input` para el backend activo.                                                                                        |

</details>

<details>
<summary>Sistema de Escenas</summary>

Organiza la aplicación en pantallas independientes y autocontenidas (menú principal, juego, pausa, etc.), cada una con su propio ciclo de vida, layer stack y acceso a los subsistemas.

| Símbolo        | Propósito                                                                                                                  |
| -------------- | -------------------------------------------------------------------------------------------------------------------------- |
| `Scene`        | Clase base. Sobreescribe los callbacks del ciclo de vida. Usa `RS_SCENE(MiEscena)` para registrar el tipo automáticamente. |
| `SceneManager` | Gestiona el stack de escenas. Soporta `ChangeScene`, `PushScene`, `PopScene`, pausa/reanudar y búsqueda por ID o nombre.   |

</details>

<details>
<summary>Eventos</summary>

Se proporcionan dos mecanismos de eventos complementarios:

Propagación por layers: los eventos del `Window` se reenvían a través del `LayerStack` en orden inverso (el layer más alto primero). Un layer puede detener la propagación poniendo `e.Handled = true`. Ideal para overlays de UI que deben consumir la entrada antes de que llegue a los layers de juego.

EventBus: canal de publicación/suscripción de tipo seguro, propiedad de la `Application` y accesible a través de `EngineContext::Bus`. Todos los suscriptores reciben el evento en orden de suscripción, independientemente del flag `Handled`. Útil para sistemas desacoplados como audio, logros o atajos de teclado de depuración. Suscríbete siempre en `OnAttach` y cancela la suscripción en `OnDetach`.

| Categoría de evento | Eventos                                                                                        |
| ------------------- | ---------------------------------------------------------------------------------------------- |
| Window              | `WindowCloseEvent`, `WindowResizeEvent`, `WindowFocusEvent`                                    |
| Teclado             | `KeyPressedEvent`, `KeyReleasedEvent`, `KeyTypedEvent`                                         |
| Mouse               | `MouseButtonPressedEvent`, `MouseButtonReleasedEvent`, `MouseMovedEvent`, `MouseScrolledEvent` |

</details>

<details>
<summary>Layers</summary>

Un `Layer` es una unidad modular dentro del `LayerStack` de una escena. Recibe los mismos callbacks que una escena más `OnUIRender()` para widgets de ImGui. El stack se divide en dos grupos: layers (procesados de abajo hacia arriba) y overlays (siempre procesados después de todos los layers). Agrégalos desde `OnAttach` con `PushLayer` / `PushOverlay`.

| Método             | Efecto                                     |
| ------------------ | ------------------------------------------ |
| `PushLayer(layer)` | Inserta un layer y llama a `OnAttach()`.   |
| `PushOverlay(ov)`  | Agrega un overlay y llama a `OnAttach()`.  |
| `PopLayer(layer)`  | Elimina un layer y llama a `OnDetach()`.   |
| `PopOverlay(ov)`   | Elimina un overlay y llama a `OnDetach()`. |

</details>

<details>
<summary>Gráficos</summary>

| Símbolo      | Propósito                                                                                                                                                                              |
| ------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Shapes`     | Primitivas rellenas y delineadas: rectángulos, círculos, líneas, triángulos. Cada función acepta un `OriginMode` opcional para anclar la forma en su centro o cualquier esquina/borde. |
| `Texts`      | Dibuja cadenas de texto en una posición de pantalla usando la fuente cargada actualmente.                                                                                              |
| `Color`      | Struct de color RGBA `{r, g, b, a}` con `Lerp`, `FromHSV` y sobrecargas de operadores.                                                                                                 |
| `Colors`     | Namespace de presets `constexpr`: `Colors::White`, `Colors::Black`, `Colors::Cyan`, `Colors::DarkBlue`, `Colors::RayWhite`, y más.                                                     |
| `OriginMode` | Enum que controla el punto de anclaje de una primitiva de `Shapes` (`TopLeft`, `Center`, `BottomRight`, etc.).                                                                         |

</details>

<details>
<summary>Math</summary>

Todos los tipos matemáticos viven en el namespace `RS::Math`.

| Símbolo        | Propósito                                                                                                                                                                                                                     |
| -------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Vec2f`        | `Vector2<float>`. Vector 2D flotante con operadores aritméticos, `Length`, `Normalize`, `Distance`, `Dot`, `Reflect`, `Project` y `RotatedBy(Angle)`.                                                                         |
| `Vec2i`        | `Vector2<int>`. Vector 2D entero con el mismo conjunto de operadores.                                                                                                                                                         |
| `Vec3f`        | `Vector3<float>`. Vector 3D flotante, usado internamente y para matemáticas generales.                                                                                                                                        |
| `Vec3i`        | `Vector3<int>`. Vector 3D entero.                                                                                                                                                                                             |
| `Angle`        | Wrapper de ángulo fuertemente tipado, almacenado internamente en radianes. Construye via `Angle::Degrees(90.0f)`, `Angle::Radians(x)` o el literal `90.0_deg`. Previene la mezcla accidental de unidades.                     |
| `Math` (utils) | `Lerp`, `InverseLerp`, `Remap`, `Clamp`, `Clamp01`, `SmoothStep`, `SmoothDamp`, `MoveTowards`, `PingPong`, `Repeat`, `EpsilonEquals`, `Saturate`, `Fract`, y más.                                                             |
| `Random`       | RNG Mersenne Twister con semilla. Rangos de enteros/flotantes, booleanos, muestreo de contenedores. Ruido coherente: Perlin 2D/3D, Simplex, Cellular, Value y FBM. Semilla auto-aleatoria al inicio via `std::random_device`. |

</details>

<details>
<summary>Input</summary>

La interfaz `Input` proporciona acceso independiente del backend al estado de teclado, mouse y gamepad. Accede a ella con `GetInput()` desde cualquier escena o layer.

| Categoría | Métodos principales                                                                             |
| --------- | ----------------------------------------------------------------------------------------------- |
| Teclado   | `IsKeyDown`, `IsKeyPressed`, `IsKeyReleased`, `IsKeyRepeating`                                  |
| Mouse     | `IsMouseButtonDown/Pressed/Released`, `GetMousePosition`, `GetMouseDelta`, `GetMouseWheelDelta` |
| Gamepad   | `IsGamepadConnected`, `IsGamepadButtonDown/Pressed`, `GetGamepadAxis`                           |

Los botones y ejes del gamepad usan los enums `GamepadButton` y `GamepadAxis` con distribución Xbox: botones de cara A/B/X/Y, LB/RB para los hombros, LT/RT para gatillos, D-Pad y palancas analógicas.

Los backends orientados a eventos (GLFW, SDL2, etc.) extienden `EventDrivenInput` en lugar de `Input` directamente. `EventDrivenInput` mantiene arrays de estado por frame Down/Pressed/Released/Repeating poblados mediante `OnEvent()` y limpiados en cada `Update()`.

</details>

<details>
<summary>Integración con ImGui</summary>

`ImGuiLayer` está integrado para overlays de depuración y paneles de UI. Agrégalo como overlay en `OnAttach()`, luego implementa `OnUIRender()` en cualquier subclase de `Layer` para dibujar widgets de ImGui. El contexto se inicializa y destruye automáticamente.

**SceneShowcase** demuestra un overlay `ImGui` compartido (`SceneNavLayer`) que cambia entre tres sub-escenas en tiempo de ejecución.

</details>

<details>
<summary>Interfaces y Backend</summary>

Cinco interfaces abstractas desacoplan el código del usuario de la librería gráfica subyacente:

| Interfaz       | Responsabilidad                                              |
| -------------- | ------------------------------------------------------------ |
| `RendererAPI`  | Inicio/fin de frame, limpieza de pantalla, control VSync.    |
| `Window`       | Título, tamaño, fullscreen, relación de aspecto.             |
| `Input`        | Estado de teclado, mouse y gamepad.                          |
| `ImGuiBackend` | Inicialización, apagado y ciclo de vida por frame de ImGui.  |
| `FontRenderer` | Carga de fuentes, medición de glifos y renderizado de texto. |

El backend de `Raylib` es la única implementación incluida actualmente. `RaylibRendererAPI`, `RaylibWindow`, `RaylibInput`, `RaylibImGuiBackend` y `RaylibFontRenderer` satisfacen cada interfaz. Todos los headers específicos del backend están confinados a esta capa y nunca se filtran al código del usuario.

El backend activo se selecciona en tiempo de configuración mediante la opción CMake `RS_BACKEND` (`raylib` por defecto).

</details>

## Compilación

Requisitos mínimos: CMake 3.28, C++20 y Ninja. Las dependencias se gestionan mediante [vcpkg](https://vcpkg.io/), incluido como submódulo de Git.

El soporte para macOS no ha sido probado oficialmente. El sistema de compilación y las dependencias deberían funcionar en teoría, pero la compatibilidad no está garantizada.

### 1. Clonar con submódulos

vcpkg está incluido como submódulo de Git, por lo que es necesario inicializarlo después de clonar el repositorio. Si lo clonaste sin `--recurse-submodules`, ejecuta:

```bash
git submodule update --init --recursive
```

### 2. Configuración inicial

Con el submódulo listo, el script de configuración inicializa vcpkg e instala automáticamente todas las dependencias requeridas:

```bash
./tools/setup_all.sh       # Linux / macOS
.\tools\setup_all.ps1      # Windows (PowerShell)
```

### 3. Compilar

Usar CMake presets es la forma recomendada y más rápida de compilar:

```bash
cmake --preset debug              # Configurar Debug (Ninja)
cmake --build --preset debug      # Compilar Debug

cmake --preset release            # Configurar Release (Ninja)
cmake --build --preset release    # Compilar Release
```

El preset `debug` habilita ASan y UBSan por defecto. El soporte de sanitizadores es experimental y puede no funcionar en todas las cadenas de herramientas; en MinGW se deshabilitan automáticamente.

### CMake Manual

Al no usar presets, pasa el archivo toolchain de vcpkg manualmente para que CMake encuentre los paquetes instalados.

#### Con Ninja (recomendado, todas las plataformas)

```bash
cmake -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DRS_BUILD_EXAMPLES=ON
cmake --build build
```

#### Con Visual Studio (solo Windows)

```bash
cmake -B build -G "Visual Studio 17 2022" -A x64 \
  -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DRS_BUILD_EXAMPLES=ON
cmake --build build --config Release
```

Visual Studio es un generador multi-config. Ignora `-DCMAKE_BUILD_TYPE` en el tiempo de configuración y requiere `--config` en el tiempo de compilación.

## Inicio Rápido

```cpp
#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"

using namespace RS;

class MyScene : public Scene {
    RS_SCENE(MyScene)

private:
    Math::Vec2f position = {400.0f, 300.0f};
    Math::Vec2f velocity = {150.0f, 100.0f};

    void OnFixedUpdate(float fixedDt) override {
        position += velocity * fixedDt;

        float width  = static_cast<float>(GetWindow().GetWidth());
        float height = static_cast<float>(GetWindow().GetHeight());

        if (position.x < 20 || position.x > width  - 20) velocity.x *= -1;
        if (position.y < 20 || position.y > height - 20) velocity.y *= -1;
    }

    void OnDraw(float /*alpha*/) override {
        GetRenderer().ClearScreen(Colors::DarkBlue);
        Shapes::DrawCircle(position.x, position.y, 20.0f, Colors::RayWhite);
    }
};

RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args)
{
    RS::ApplicationConfig config;
    config.Window.Title  = "Mi Primera App Raysim";
    config.Window.Width  = 800;
    config.Window.Height = 600;

    auto* app = new RS::Application(config);
    app->RegisterScene<MyScene>();
    app->ChangeScene<MyScene>();
    return app;
}
```

La semilla es auto-aleatoria al inicio. Llama a `Math::Random::Seed(valor)` en `OnAttach()` solo si necesitas reproducibilidad.

## Licencia

Este proyecto está licenciado bajo la **Apache License 2.0**.
Consulta el archivo [LICENSE](LICENSE) para más detalles.

<p align="center">
  <img src="docs/images/footer.png" alt="Raysim Footer" width="600" />
</p>
