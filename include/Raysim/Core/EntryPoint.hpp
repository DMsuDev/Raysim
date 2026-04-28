#pragma once
#include "Raysim/Core/Base.hpp"
#include "Raysim/Core/Application.hpp"

extern RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	RS::Log::Init();

	RS_PROFILE_BEGIN_SESSION("Startup", "RaysimProfile-Startup.json");
	auto app = RS::CreateApplication({ argc, argv });
	RS_PROFILE_END_SESSION();

	RS_PROFILE_BEGIN_SESSION("Runtime", "RaysimProfile-Runtime.json");
	app->Run();
	RS_PROFILE_END_SESSION();

	RS_PROFILE_BEGIN_SESSION("Shutdown", "RaysimProfile-Shutdown.json");
	delete app;
	RS_PROFILE_END_SESSION();

	RS::Log::Shutdown();
}
