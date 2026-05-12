# Arquitectura

Este documento cubre la estructura interna de Raysim: el ciclo de vida de la aplicación, el resumen de módulos y cómo encajan las piezas.

---

## Bucle de Aplicación

Cada frame, el motor recorre los callbacks del ciclo de vida de la escena activa en un orden fijo. Solo sobreescribe los que necesites — los no implementados son no-ops.

| Callback                 | Cuándo se llama                                                                                                                      |
| ------------------------ | ------------------------------------------------------------------------------------------------------------------------------------ |
| `OnAttach`               | Una vez, cuando la escena se añade al stack. Carga assets, agrega layers, suscríbete al EventBus.                                    |
| `OnStart`                | Cada vez que la escena se activa (tras `OnAttach` o al reanudar desde pausa). Reinicia el estado de la simulación aquí.              |
| `OnUpdate(dt)`           | Cada frame. Consulta de entrada, lógica del juego, actualizaciones de estado a tasa variable.                                        |
| `OnFixedUpdate(fixedDt)` | Con un paso de tiempo fijo, independiente del frame rate. La integración de física y los pasos deterministas van aquí.               |
| `OnDraw(alpha)`          | Cada frame, tras `OnUpdate`. Todos los comandos de renderizado van aquí. `alpha` es un valor en [0, 1) para interpolación de física. |
| `OnPause`                | Cuando el `SceneManager` pausa la escena.                                                                                            |
| `OnResume`               | Cuando la escena se reanuda desde un estado de pausa.                                                                                |
| `OnDetach`               | Cuando la escena se elimina del stack. Libera recursos y cancela suscripciones al EventBus.                                          |

> [!TIP]
> `OnDraw` recibe un factor de interpolación `alpha` para interpolar entre el snapshot de física anterior y el actual, consiguiendo renderizado suave a cualquier frame rate. Si pones todo el renderizado en `OnUpdate`, `alpha` simplemente valdrá 0 — ambas formas son válidas.

<!-- -->

> [!CAUTION]
> Nunca reserves memoria ni cargues assets dentro de `OnDraw`. Usa `OnAttach` o `OnStart` para la inicialización. `OnDraw` se llama cada frame y cualquier alocación ahí causará microtartamudeos.

---

## Módulos

### Core

| Símbolo             | Propósito                                                                                                                                                                     |
| ------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Application`       | Clase raíz. Gestiona el bucle principal, el backend, el `SceneManager` y el `EngineContext`. Registra escenas con `RegisterScene<T>()` y actívalas con `ChangeScene<T>()`.    |
| `ApplicationConfig` | Configuración declarativa (título, ancho, alto, nivel de log, máximo de pasos fijos). Se pasa al constructor de `Application`; todos los campos tienen valores por defecto.   |
| `EngineContext`     | Struct plano pasado a cada escena y layer. Contiene punteros no propietarios a `Window`, `Input`, `SceneManager`, `ApplicationConfig` y `EventBus`.                           |
| `Time`              | Utilidad estática: delta time, paso de tiempo fijo, escala de tiempo, pausa/reanudar, FPS suavizado y `GetRealtimeSinceStartup()`.                                            |
| `Log`               | Wrapper ligero sobre **spdlog**. Escribe en consola y en un archivo de log opcional. Macros: `RS_LOG_TRACE`, `RS_LOG_INFO`, `RS_LOG_WARN`, `RS_LOG_ERROR`, `RS_LOG_CRITICAL`. |
| `FontManager`       | Carga una fuente TTF/OTF una vez con una clave y la referencia globalmente para renderizado de texto. La clave `"default"` se usa cuando no se especifica ninguna.            |
| `BackendFactory`    | Crea instancias concretas de `RendererAPI`, `Window` e `Input` para el backend activo seleccionado en tiempo de compilación.                                                  |

> [!NOTE]
> Las macros `RS_LOG_*` tienen coste cero en builds Release cuando el nivel de log está por encima del umbral — la expresión nunca se evalúa. Úsalas libremente sin preocuparte por el rendimiento.

---

### Sistema de Escenas

Organiza la aplicación en pantallas independientes y autocontenidas (menú principal, juego, pausa, etc.), cada una con su propio ciclo de vida, layer stack y acceso a los subsistemas.

| Símbolo        | Propósito                                                                                                                  |
| -------------- | -------------------------------------------------------------------------------------------------------------------------- |
| `Scene`        | Clase base. Sobreescribe los callbacks del ciclo de vida. Usa `RS_SCENE(MiEscena)` para registrar el tipo automáticamente. |
| `SceneManager` | Gestiona el stack de escenas. Soporta `ChangeScene`, `PushScene`, `PopScene`, pausa/reanudar y búsqueda por ID o nombre.   |

`ChangeScene<T>()` reemplaza la escena actual inmediatamente. `PushScene<T>()` suspende (pero conserva) la escena actual y apila una nueva encima — ideal para menús de pausa o sub-flujos que necesitan volver a la pantalla anterior.

---

### Eventos

Se proporcionan dos mecanismos complementarios. Usa el que mejor se adapte a tu arquitectura — no son excluyentes.

**Propagación por layers (bloqueante, ordenada)**
Los eventos del `Window` se reenvían a través del `LayerStack` de arriba hacia abajo. Un layer detiene la propagación poniendo `e.Handled = true`. Ideal para overlays de UI que deben consumir la entrada antes de que llegue a los layers de juego.

**EventBus (broadcast, no ordenado)**
Canal de publicación/suscripción de tipo seguro, propiedad de la `Application` y accesible a través de `EngineContext::Bus`. Todos los suscriptores reciben el evento independientemente del flag `Handled`. Útil para sistemas desacoplados: audio, logros, atajos de teclado de depuración.

> [!WARNING]
> Suscríbete siempre en `OnAttach` y **cancela la suscripción en `OnDetach`**. El `EventBus` sobrevive a cualquier layer individual. No hacerlo deja un callback colgante que se llamará después de que el layer sea destruido.

| Categoría de evento | Eventos                                                                                        |
| ------------------- | ---------------------------------------------------------------------------------------------- |
| Window              | `WindowCloseEvent`, `WindowResizeEvent`, `WindowFocusEvent`                                    |
| Teclado             | `KeyPressedEvent`, `KeyReleasedEvent`, `KeyTypedEvent`                                         |
| Mouse               | `MouseButtonPressedEvent`, `MouseButtonReleasedEvent`, `MouseMovedEvent`, `MouseScrolledEvent` |

---

### Layers

Un `Layer` es una unidad modular dentro del `LayerStack` de una escena. Recibe los mismos callbacks del ciclo de vida que una escena, más `OnUIRender()` para widgets de Dear ImGui. El stack tiene dos grupos: layers (procesados de abajo hacia arriba en renderizado, de arriba hacia abajo en eventos) y overlays (siempre sobre todos los layers).

| Método             | Efecto                                     |
| ------------------ | ------------------------------------------ |
| `PushLayer(layer)` | Inserta un layer y llama a `OnAttach()`.   |
| `PushOverlay(ov)`  | Agrega un overlay y llama a `OnAttach()`.  |
| `PopLayer(layer)`  | Elimina un layer y llama a `OnDetach()`.   |
| `PopOverlay(ov)`   | Elimina un overlay y llama a `OnDetach()`. |

---

### Gráficos

| Símbolo      | Propósito                                                                                                                                                                              |
| ------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Shapes`     | Primitivas rellenas y delineadas: rectángulos, círculos, líneas, triángulos. Cada función acepta un `OriginMode` opcional para anclar la forma en su centro o cualquier esquina/borde. |
| `Texts`      | Dibuja cadenas de texto en una posición de pantalla usando un handle de fuente cargada.                                                                                                |
| `Color`      | Struct de color RGBA `{r, g, b, a}` con `Lerp`, `FromHSV` y sobrecargas de operadores.                                                                                                 |
| `Colors`     | Namespace de presets `constexpr`: `Colors::White`, `Colors::Black`, `Colors::Cyan`, `Colors::DarkBlue`, `Colors::RayWhite`, y más.                                                     |
| `OriginMode` | Enum que controla el punto de anclaje de una primitiva de `Shapes` (`TopLeft`, `Center`, `BottomRight`, etc.).                                                                         |

---

### Math

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

> [!TIP]
> Llama a `Math::Random::Seed(valor)` en `OnAttach()` solo si necesitas una secuencia reproducible (por ejemplo para tests o replay). La semilla automática por defecto es suficiente para la mayoría de los casos.

---

### Input

La interfaz `Input` proporciona acceso independiente del backend al estado de teclado, mouse y gamepad. Accede a ella con `GetInput()` desde cualquier escena o layer.

| Categoría | Métodos principales                                                                             |
| --------- | ----------------------------------------------------------------------------------------------- |
| Teclado   | `IsKeyDown`, `IsKeyPressed`, `IsKeyReleased`, `IsKeyRepeating`                                  |
| Mouse     | `IsMouseButtonDown/Pressed/Released`, `GetMousePosition`, `GetMouseDelta`, `GetMouseWheelDelta` |
| Gamepad   | `IsGamepadConnected`, `IsGamepadButtonDown/Pressed`, `GetGamepadAxis`                           |

Los botones y ejes del gamepad usan los enums `GamepadButton` y `GamepadAxis` con distribución Xbox: botones de cara A/B/X/Y, hombros LB/RB, gatillos LT/RT, D-Pad y palancas analógicas.

Los backends orientados a eventos (GLFW, SDL2, etc.) extienden `EventDrivenInput` en lugar de `Input` directamente. Mantiene arrays de estado por frame Down/Pressed/Released/Repeating poblados mediante `OnEvent()` y limpiados en cada `Update()`.

---

### Integración con ImGui

`ImGuiLayer` está integrado para overlays de depuración y paneles de UI. Agrégalo como overlay en `OnAttach()`, luego implementa `OnUIRender()` en cualquier subclase de `Layer` para dibujar widgets de Dear ImGui. El contexto se inicializa y destruye automáticamente.

> [!IMPORTANT]
> Todas las llamadas a Dear ImGui deben ir dentro de `OnUIRender()`. Llamar a funciones ImGui en `OnDraw` o `OnUpdate` no producirá salida o provocará fallos de aserción dentro de la librería ImGui.

**SceneShowcase** demuestra un overlay ImGui compartido (`SceneNavLayer`) que cambia entre tres sub-escenas en tiempo de ejecución — una buena referencia para UI con múltiples escenas.

---

### Interfaces y Backend

Cinco interfaces abstractas desacoplan el código del usuario de la librería gráfica subyacente:

| Interfaz       | Responsabilidad                                              |
| -------------- | ------------------------------------------------------------ |
| `RendererAPI`  | Inicio/fin de frame, limpieza de pantalla, control VSync.    |
| `Window`       | Título, tamaño, fullscreen, relación de aspecto.             |
| `Input`        | Estado de teclado, mouse y gamepad.                          |
| `ImGuiBackend` | Inicialización, apagado y ciclo de vida por frame de ImGui.  |
| `FontRenderer` | Carga de fuentes, medición de glifos y renderizado de texto. |

El backend de `Raylib` es la única implementación incluida actualmente. `RaylibRendererAPI`, `RaylibWindow`, `RaylibInput`, `RaylibImGuiBackend` y `RaylibFontRenderer` satisfacen cada interfaz. Todos los headers específicos del backend están confinados a esta capa y nunca se filtran al código del usuario.

El backend activo se selecciona en tiempo de configuración mediante la opción CMake `RS_BACKEND` (`raylib` por defecto). Consulta [BACKENDS.md](BACKENDS.md) para instrucciones sobre cómo implementar un backend personalizado.

---

<p align="center">
  <img src="images/footer.png" alt="Raysim Footer" width="600" />
</p>
