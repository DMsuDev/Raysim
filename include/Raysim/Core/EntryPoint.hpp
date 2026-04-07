#pragma once

#include <iostream>
#include "Raysim/Core/Application.hpp"
#include <spdlog/spdlog.h>

extern RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	RS::Log::Init(true);
	
	RS_CORE_INFO("Starting application...");
	
	auto app = RS::CreateApplication({ argc, argv });
	
	app->Run();
	
	RS_CORE_INFO("Application exiting...");

	delete app;
	
	RS::Log::Shutdown();
}
