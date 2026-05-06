#include "Raysim/Core/EntryPoint.hpp"

#include "StarField.hpp"
#include "PlasmaArt.hpp"
#include "ClockMandala.hpp"

// ============================================================================
// Entry point
// ============================================================================

RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs /*args*/)
{
    RS::ApplicationConfig config;
    config.Window.Title  = "SceneShowcase";
    config.Window.Width  = 1000;
    config.Window.Height = 700;

    auto* app = new RS::Application(config);
    app->RegisterScene<StarField>();
    app->RegisterScene<PlasmaArt>();
    app->RegisterScene<ClockMandala>();
    app->ChangeScene<StarField>();
    return app;
}
