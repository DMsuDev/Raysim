# Raysim

[![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/Build-CMake-064F8C?style=flat&logo=cmake&logoColor=white)](https://cmake.org/)
![Status](https://img.shields.io/badge/Status-Early%20Development-yellow?style=flat)
[![Version](https://img.shields.io/badge/Version-0.6.1-brightgreen?style=flat)](https://github.com/DMsuDev/Raysim/releases)

[English Readme](https://github.com/DMsuDev/Raysim/blob/main/README.md)
• [Readme Español](https://github.com/DMsuDev/Raysim/blob/main/README.es.md)

Raysim es un framework de C++ para gráficos 2D y aplicaciones interactivas, construido sobre [raylib](https://www.raylib.com/).

Inspirado en **p5.js** y **Processing**, ofrece una API simple basada en clases para dibujar formas, manejar entrada, gestionar el tiempo y ejecutar simulaciones con paso de tiempo fijo. Los headers del backend nunca se exponen al código del usuario, todo el acceso pasa por interfaces abstractas limpias.

Útil para aprender programación gráfica, prototipar ideas o construir pequeños juegos y simulaciones.

> **Nota:** Este proyecto está en **desarrollo temprano**. La API puede cambiar sin previo aviso y algunas funcionalidades aún están en implementación. Es mi primera vez desarrollando un framework en C++. ¡Comentarios y contribuciones son bienvenidos!

## Demos Rápidos

<!-- Screenshots o GIF.
    TODO:
-->

<table>
  <tr>
    <td align="center">
      <img src="docs/images/bouncing-balls.png" width="400" alt="Bouncing Balls" /><br />
      <b>BouncingBalls</b>
    </td>
    <td align="center">
      <img src="docs/images/lissajous-curves.png" width="400" alt="Lissajous Curves" /><br />
      <b>LissajousCurves</b>
    </td>
  </tr>
  <tr>
    <td align="center">
      <img src="docs/images/mouse-2d.png" width="400" alt="Mouse 2D" /><br />
      <b>Mouse2D</b>
    </td>
    <td align="center">
      <img src="docs/images/noise-landscape.png" width="400" alt="Noise Landscape" /><br />
      <b>NoiseLandscape</b>
    </td>
  </tr>
</table>

| Ejemplo           | Descripción                                                                             |
| ----------------- | --------------------------------------------------------------------------------------- |
| `BouncingBalls`   | Simulación de física con gravedad, atracción/repulsión del mouse y generación de bolas. |
| `LissajousCurves` | Visualizador de curvas paramétricas con cambio de fase animado y presets de frecuencia. |
| `Mouse2D`         | Seguimiento del mouse e interacción 2D.                                                 |
| `NoiseLandscape`  | Terreno con desplazamiento generado proceduralmente usando varias funciones de ruido.   |

### Usando CMake Presets

Para compilar los ejemplos, habilita la opción `RS_BUILD_EXAMPLES` (ya habilitada en los presets):

```bash
cmake --preset debug
cmake --build --preset debug
```

> Cada ejemplo es un ejecutable independiente en `examples/` que demuestra diferentes funcionalidades del framework. Puedes ejecutarlos después de compilar el proyecto.

## Bucle de Aplicación

Cada ciclo de la aplicación ejecuta los métodos del ciclo de vida de la escena activa en orden.

<details>
<summary>OnAttach</summary>

Se llama una vez cuando la escena se adjunta por primera vez al SceneManager. Úsalo para
cargar assets, crear entidades e inicializar el estado de la escena.

```cpp
class MyScene : public Scene {
    RS_SCENE(MyScene) // Macro para generar ID y nombre de escena para lookup
protected:
    void OnAttach() override {
        GetWindow().SetTitle("Mi Escena");
    }
};
```

</details>

<details>
<summary>OnStart</summary>

Se llama cada vez que la escena comienza (después de `OnAttach` o cuando se reanuda). Úsalo para
reiniciar el estado del juego, reiniciar temporizadores o inicializar recursos dinámicos.

```cpp
void OnStart() override {
    position = {400, 300};
    velocity = {150, 100};
}
```

</details>

<details>
<summary>OnUpdate</summary>

Se llama cada frame. Úsalo para consultar la entrada, lógica del juego y todo lo que
lea o escriba estado de la simulación. Recibe el delta time escalado en segundos
para que el movimiento sea independiente del frame rate.

```cpp
void OnUpdate(float dt) override {
    if (GetInput().IsKeyPressed(KeyCode::Space)) foo();
    position += velocity * dt;
}
```

</details>

<details>
<summary>OnFixedUpdate</summary>

Se llama con un paso de tiempo fijo independientemente del frame rate real. Úsalo para
integración de física y pasos de simulación deterministas. El acumulador ejecuta
tantos pasos fijos como sea necesario para alcanzar el tiempo real, limitado por
`maxFixedSteps` de `ApplicationConfig` para evitar una espiral de la muerte.

```cpp
void OnFixedUpdate(float fixedDt) override {
    velocity += gravity * fixedDt;
    position += velocity * fixedDt;
}
```

</details>

<details>
<summary>OnDraw</summary>

Se llama cada frame después de `OnUpdate`. Emite todos los comandos de renderizado aquí.
Recibe un factor de interpolación `alpha` en `[0, 1)` que representa cuánto ha avanzado
la simulación hacia el siguiente paso fijo. Úsalo para interpolar entre la captura
de física anterior y la actual para que los visuales se mantengan suaves a cualquier
frame rate. No mutes estado dentro de `OnDraw`.

```cpp
void OnDraw(float alpha) override {
    GetRenderer().ClearScreen(Colors::DarkBlue);
    Vector2 renderPos = prevPosition + (position - prevPosition) * alpha;
    Shapes::DrawCircle(renderPos.x, renderPos.y, 20.0f, Colors::RayWhite);
}
```

> Usar un paso de Draw separado es ideal porque mantiene la estructura del código limpia y evita inconsistencias con el bucle de física. Sin embargo, es opcional. Si prefieres, puedes poner toda tu lógica de renderizado en OnUpdate y dejar OnDraw vacío. En ese caso, el parámetro `alpha` siempre será 0 ya que no hay interpolación, pero no causará ningún problema.

</details>

<details>
<summary>OnPause / OnResume</summary>

- `OnPause`: Se llama cuando la escena se pausa. Úsalo para pausar animaciones, detener temporizadores, etc.
- `OnResume`: Se llama cuando la escena se reanuda desde un estado de pausa. Úsalo para reanudar animaciones, reiniciar temporizadores, etc.

```cpp
void OnPause() override {
    RS_LOG_INFO("Escena pausada");
}

void OnResume() override {
    RS_LOG_INFO("Escena reanudada");
}
```

</details>

## Módulos

<details>
<summary>Core</summary>

| Archivo             | Propósito                                                                                                                                                                                       |
| ------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Application`       | Clase base. Crea escenas y regístralas con `RegisterScene<T>()` o `ChangeScene<T>()`. Accede directamente a Window, Renderer e Input.                                                           |
| `ApplicationConfig` | Configura título, resolución, máximo de pasos fijos y archivo de log antes de que comience el bucle. Todos los campos tienen valores por defecto - pasa solo lo que necesites.                  |
| `Time`              | Utilidad estática. Delta time, paso de tiempo fijo, escala de tiempo, pausa/reanudar, contadores de FPS.                                                                                        |
| `Log`               | Envuelve spdlog. Escribe en consola y archivo de log. Usa las macros `RS_LOG_INFO`, `RS_LOG_WARN`, `RS_LOG_ERROR`.                                                                              |
| `FontManager`       | Carga una fuente TTF/OTF una vez, accede globalmente para renderizado de texto. Puedes establecer una fuente por defecto usando `SetDefaultFont("ruta/fuente.ttf")` en `OnAttach() override`.   |
| `BackendFactory`    | Crea instancias concretas de `RendererAPI`, `Window` e `Input` para el backend seleccionado.                                                                                                    |
| `Scene`             | Clase base para escenas. Proporciona callbacks del ciclo de vida (OnStart, OnUpdate, OnFixedUpdate, OnDraw, OnPause, OnResume). Las escenas reciben un EngineContext para acceso a subsistemas. |
| `SceneManager`      | Gestionar escenas. Soporta operaciones de push/pop, pausa/reanudar y búsqueda de escenas por ID o nombre.                                                                                       |

</details>

<details>
<summary>Scene</summary>

En la version 0.6.0 se introdujo un nuevo sistema de escenas para permitir una arquitectura basada en escenas más robusta y flexible. El sistema de escenas proporciona una forma estructurada de organizar el código de tu aplicación en diferentes escenas (menú principal, juego, pantalla de pausa, etc.) con su propio ciclo de vida y acceso a los subsistemas del motor.

**¿Cómo crear una escena?**

Para crear una escena, hereda de la clase `Scene` y usa el macro `RS_SCENE(MiEscena)` dentro de la definición para registrar el nombre e ID de la escena automáticamente:

```cpp
class MiEscena : public Scene {
    RS_SCENE(MiEscena)
    // ... métodos override como OnAttach, OnUpdate, etc.
};
```

Luego, registra la escena en tu aplicación usando `RegisterScene`:

```cpp
app->RegisterScene<MiEscena>();
```

Hay que registrar la escena antes de poder usarla. Esto permite cambiar a esa escena por nombre o tipo en cualquier momento.
Para definir la escena inicial, puedes usar `SetInitialScene<MiEscena>()` o `ChangeScene<MiEscena>()` en tu función `CreateApplication()`:

```cpp
RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args)
{
    auto* app = new Application();
    app->RegisterScene<MiEscena>();
    app->SetInitialScene<MiEscena>(); // Establece MiEscena como la escena inicial
    return app;
}
```

Esto permite cambiar a esa escena por nombre o tipo en cualquier momento.

</details>

<details>
<summary>Graphics</summary>

| Archivo      | Propósito                                                                                                                                                                                                    |
| ------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `Shapes`     | Rellenos y contornos: rectángulos, círculos, líneas, triángulos. Cada función acepta un `OriginMode` opcional para anclar la forma en su centro o cualquier esquina/borde en lugar del top-left por defecto. |
| `Texts`      | Dibuja cadenas de texto usando la fuente cargada.                                                                                                                                                            |
| `Color`      | Struct de color RGBA `{r, g, b, a}` con conversión HSV. Construye cualquier color inline: `Color{255, 100, 0}`.                                                                                              |
| `Colors`     | Namespace de presets `constexpr`: `Colors::White`, `Colors::Black`, `Colors::Cyan`, `Colors::DarkBlue`, `Colors::RayBlack`, y más. Úsalos en lugar de construir colores a mano.                              |
| `OriginMode` | Enum usado por Shapes para controlar el punto de anclaje de una forma (TopLeft, Center, BottomRight, etc.).                                                                                                  |

</details>

<details>
<summary>Math</summary>

| Archivo      | Propósito                                                                                                                                                                                                                                                                                                                          |
| ------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Vector2`    | Vector 2D con operadores aritméticos y métodos de utilidad comunes.                                                                                                                                                                                                                                                                |
| `Vector2Int` | Vector 2D de enteros con operadores aritméticos y métodos de utilidad comunes.                                                                                                                                                                                                                                                     |
| `Vector3`    | Vector 3D, usado internamente para operaciones de color/limpieza y matemáticas generales.                                                                                                                                                                                                                                          |
| `Vector3Int` | Vector 3D de enteros con operadores aritméticos y métodos de utilidad comunes.                                                                                                                                                                                                                                                     |
| `Math`       | Helpers matemáticos comunes: clamp, lerp, map, wrap y utilidades trigonométricas.                                                                                                                                                                                                                                                  |
| `Random`     | RNG Mersenne Twister con semilla. Rangos de enteros y flotantes, helpers booleanos, muestreo de contenedores, más ruido coherente (Perlin 2D/3D, Simplex, Cellular, Value) y Fractal Brownian Motion. La semilla es auto-aleatoria al inicio; llama a `Seed()` para resultados deterministas o `SeedRandom()` para re-aleatorizar. |

</details>

<details>
<summary>Interfaces y Backend</summary>

Las tres interfaces abstractas desacoplan el código del usuario de la librería subyacente:

| Interfaz      | Responsabilidad                                           |
| ------------- | --------------------------------------------------------- |
| `RendererAPI` | Inicio/fin de frame, limpieza de pantalla, control VSync. |
| `Window`      | Título, tamaño, fullscreen, relación de aspecto.          |
| `Input`       | Teclado, botones del mouse, posición del cursor, scroll.  |

El backend de `Raylib` es la única implementación incluida. `RaylibRendererAPI`,
`RaylibWindow` y `RaylibInput` satisfacen cada interfaz. Todos los headers
específicos de raylib están confinados a esta capa y nunca se filtran al código del usuario.

</details>

## Compilación

Requisitos mínimos: **CMake 3.28**, **C++20** y **Ninja**.
Las dependencias se gestionan mediante [vcpkg](https://vcpkg.io/) (incluido como submódulo).

### Usando CMake Presets

```bash
cmake --preset debug              # Configurar Debug (Ninja)
cmake --build --preset debug      # Compilar Debug

cmake --preset release            # Configurar Release (Ninja)
cmake --build --preset release    # Compilar Release
```

### CMake Manual

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DRS_BUILD_EXAMPLES=ON
cmake --build build --config Release
```

## Inicio Rápido

Crea una escena heredando de `Scene` y sobreescribe los métodos del ciclo de vida. Luego regístrala con tu aplicación usando `RegisterScene<T>()` o `ChangeScene<T>()`, después de `SetInitialScene<T>()` para definir la escena inicial.

```cpp
#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"

using namespace RS;

class MyScene : public Scene {
    RS_SCENE(MyScene)

private:
    Vector2 position = {400, 300};
    Vector2 velocity = {150, 100};

    void OnAttach() override {
           GetWindow().SetTitle("Mi Primera App Raysim");
           GetWindow().SetSize(800, 600);
        Time::SetTargetFPS(60);
    }

    void OnFixedUpdate(float fixedDt) override {
        position += velocity * fixedDt;

            float width  = static_cast<float>(GetWindow().GetWidth());
            float height = static_cast<float>(GetWindow().GetHeight());

        if (position.x < 20 || position.x > width - 20)  velocity.x *= -1;
        if (position.y < 20 || position.y > height - 20) velocity.y *= -1;
    }

    void OnDraw(float /*alpha*/) override {
        GetRenderer().ClearScreen(Colors::DarkBlue);
        Shapes::DrawCircle(position.x, position.y, 20.0f, Colors::RayWhite);
    }
};

RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args)
{
    auto* app = new Application();
    app->RegisterScene<MyScene>();
    app->SetInitialScene<MyScene>();
    return app;
}
```

> **Nota:** La semilla es auto-aleatoria al inicio. Llama a `SetRandomSeed(valor)` en `OnAttach()` solo si necesitas reproducibilidad.

## Licencia

Este proyecto está licenciado bajo la **Apache License 2.0**.
Consulta el archivo [LICENSE](LICENSE) para más detalles.
