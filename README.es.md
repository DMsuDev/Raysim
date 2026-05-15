<p align="center">
  <img src="docs/images/banner.png" alt="Raysim Banner" width="720" />
</p>

[![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=flat&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/Build-CMake-064F8C?style=flat&logo=cmake&logoColor=white)](https://cmake.org/)
![Status](https://img.shields.io/badge/Status-Beta-blue?style=flat)
[![Version](https://img.shields.io/badge/Version-0.94.1-brightgreen?style=flat)](https://github.com/DMsuDev/Raysim/releases)
[![License](https://img.shields.io/badge/Licencia-Apache%202.0-lightgrey?style=flat)](LICENSE)

[English Readme](https://github.com/DMsuDev/Raysim/blob/main/README.md)
• [Readme Español](https://github.com/DMsuDev/Raysim/blob/main/README.es.md)
• [Changelog](docs/CHANGELOG.md)
• [Architecture](docs/ARCHITECTURE.md)

Raysim es un framework de C++ para gráficos 2D y aplicaciones interactivas, construido sobre [raylib](https://www.raylib.com/).

Inspirado arquitectónicamente en el [**Hazel Engine de The Cherno**](https://github.com/TheCherno/Hazel), ofrece una API limpia basada en clases para dibujar formas, manejar entrada, gestionar el tiempo y ejecutar simulaciones con paso de tiempo fijo.

Útil para aprender programación gráfica, prototipar ideas o construir pequeños juegos y simulaciones.

Comentarios y contribuciones son bienvenidos. ❤️

> [!NOTE]
> Este proyecto está en **Beta**. La API principal es estable pero puede seguir evolucionando. Algunas funcionalidades aún están en implementación.

## Demos Rápidos

<p align="center">
  <img src="docs/gif/Mouse2D.gif" alt="Mouse2D" width="720" /><br />
  <b>Mouse2D</b>
</p>

<p align="center">
  <img src="docs/gif/NoiseLandscape.gif" alt="NoiseLandscape" width="720" /><br />
  <b>NoiseLandscape</b>
</p>

<p align="center">
  <img src="docs/gif/SceneShowcase.gif" alt="SceneShowcase" width="720" /><br />
  <b>SceneShowcase</b>
</p>

| Ejemplo           | Descripción                                                                                                                                                                                                           |
| ----------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `BouncingBalls`   | Simulación de física con gravedad, atracción/repulsión del mouse y generación de bolas. Genera bolas con clic izquierdo, activa o desactiva la gravedad con G y observa cómo colisionan y rebotan contra las paredes. |
| `LissajousCurves` | Visualizador de curvas paramétricas con cambio de fase animado y presets de frecuencia. La curva evoluciona en tiempo real; prueba cambiar presets desde el panel ImGui para ver cómo se transforman los patrones.    |
| `Mouse2D`         | Sigue la posición del mouse con interpolación exponencial suave y dibuja un círculo que lo persigue. Un buen punto de partida para entender el pipeline de entrada y renderizado.                                     |
| `NoiseLandscape`  | Terreno con desplazamiento generado proceduralmente en capas usando ruido Perlin, Simplex, Cellular y Value. Pulsa R para regenerar todas las capas y obtener un paisaje completamente nuevo.                         |
| `SceneShowcase`   | Tres escenas visuales independientes (StarField, PlasmaArt, ClockMandala) accesibles desde un overlay de navegación ImGui compartido. Muestra cómo cambiar escenas en tiempo de ejecución usando el SceneManager.     |

Todos los ejemplos se compilan junto con la librería. Consulta la sección [Compilación](#-compilación) para las instrucciones de configuración.

> [!TIP]
> ¿Buscas la referencia completa de módulos, callbacks del ciclo de vida y el sistema de eventos? Consulta [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## 🔧 Compilación

Para obtener información detallada sobre requisitos del sistema, plataformas soportadas e información de dependencias, consulta el documento [System & Build Requirements](docs/REQUIREMENTS.md).

> [!NOTE]
> El soporte para **macOS** no ha sido probado oficialmente. El sistema de compilación y las dependencias deberían funcionar en teoría, pero la compatibilidad no está garantizada.

### 1. Clonar con submódulos

vcpkg está incluido como submódulo de Git, por lo que es necesario inicializarlo después de clonar el repositorio. Si lo clonaste sin `--recurse-submodules`, ejecuta:

```bash
git submodule update --init --recursive
```

### 2. Setup

Una vez que el submódulo esté listo, el script de configuración inicializa vcpkg e instala automáticamente todas las dependencias requeridas:

```bash
./tools/setup_all.sh       # Linux / macOS
.\tools\setup_all.ps1      # Windows (PowerShell)
```

### 3. Build

Usar CMake presets es la forma recomendada y más rápida de compilar:

```bash
cmake --preset debug              # Configurar Debug (Ninja)
cmake --build --preset debug      # Compilar Debug

cmake --preset release            # Configurar Release (Ninja)
cmake --build --preset release    # Compilar Release
```

El preset `debug` habilita ASan y UBSan por defecto.

> [!WARNING]
> El soporte de sanitizadores es experimental y puede no funcionar en todas las cadenas de herramientas. En MinGW se deshabilitan automáticamente.

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

## 🚀 Inicio Rápido

> [!TIP]
> El ejemplo a continuación es intencionalmente mínimo. Para una visión más profunda del ciclo de vida de las escenas, layers, eventos y todos los módulos disponibles, consulta [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) y [docs/EXAMPLES.md](docs/EXAMPLES.md).

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

        float width  = static_cast<float>(window().GetWidth());
        float height = static_cast<float>(window().GetHeight());

        if (position.x < 20 || position.x > width  - 20) velocity.x *= -1;
        if (position.y < 20 || position.y > height - 20) velocity.y *= -1;
    }

    void OnDraw(float /*alpha*/) override {
        renderer().ClearScreen(Colors::DarkBlue);
        Shapes::DrawCircle(position.x, position.y, 20.0f, Colors::RayWhite);
    }
};

RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args)
{
    RS::ApplicationConfig config;
    config.Window.Title  = "Mi Primera Escena";
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
  <img src="docs/images/footer.png" alt="Raysim Footer" width="720" />
</p>
