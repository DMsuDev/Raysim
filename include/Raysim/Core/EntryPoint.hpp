/**********************************************************************************************
 *   Raysim - A C++ framework for 2D graphics and interactive applications
 *
 *   LICENSE: Apache License, Version 2.0
 *
 *            Copyright 2026 Dayron Mustelier (@DMsuDev)
 *
 *   Raysim is licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *             http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 **********************************************************************************************/

#pragma once

#include "Raysim/Core/Base.hpp"
#include "Raysim/Core/Application.hpp"
#include "Raysim/Utils/SystemUtils.hpp"

extern RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	RS::Log::Init(true, RS::LogFileMode::Truncate);

	RS_CORE_INFO("========= LOG START: {} {} =========", RS::Utils::Time::FullDate(), RS::Utils::Time::ClockTime());

	RS_PROFILE_BEGIN_SESSION("Startup", "RaysimProfile-Startup.json");
	auto app = RS::CreateApplication({ argc, argv });
	RS_PROFILE_END_SESSION();

	RS_PROFILE_BEGIN_SESSION("Runtime", "RaysimProfile-Runtime.json");
	app->Run();
	RS_PROFILE_END_SESSION();

	RS_PROFILE_BEGIN_SESSION("Shutdown", "RaysimProfile-Shutdown.json");
	delete app;
	RS_PROFILE_END_SESSION();

	RS_CORE_INFO("========= LOG END: {} {} =========", RS::Utils::Time::FullDate(), RS::Utils::Time::ClockTime());

	RS::Log::Shutdown();
}
