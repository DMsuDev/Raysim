#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"

#include "LissajousSimulation.hpp"

RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args)
{
    RS::ApplicationConfig config;
    config.Window.Title  = "Raysim - Lissajous Curves";
    config.Window.Width  = 1200;
    config.Window.Height = 800;
    config.TargetFPS     = 60;
    config.VSync         = true;

    auto* app = new RS::Application(config);
    app->RegisterScene<LissajousSimulation>();
    app->ChangeScene<LissajousSimulation>();
    return app;
}
