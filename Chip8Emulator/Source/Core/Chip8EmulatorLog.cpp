#include "c8pch.h"
#include "Core/Chip8EmulatorLog.h"

#include <filesystem>
#include <iomanip>
#include <SDL3/SDL_filesystem.h>

#include "spdlog/sinks/basic_file_sink.h"

std::shared_ptr<spdlog::logger> s_Chip8EmulatorLogger;

void InitLog(const char* prefPath)
{
	if (!s_Chip8EmulatorLogger) 
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		std::vector<spdlog::sink_ptr> sinks;

		std::stringstream buffer;
		const std::time_t t = std::time(nullptr);
		const std::tm tm = *std::localtime(&t);
		buffer << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
		// Ensure the logs folder exists inside the pref path (if it exists), as spdlog fails to create the file if the folder does not exist AND it is an absolute path.
		if (prefPath)
			std::filesystem::create_directories(fmt::format("{}Logs", prefPath));
		std::string path = fmt::format("{}Logs{}{}.txt", prefPath != nullptr ? prefPath : "", static_cast<char>(std::filesystem::path::preferred_separator), buffer.str());
		sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(path));

		sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		
		s_Chip8EmulatorLogger = std::make_shared<spdlog::logger>("Chip-8", std::begin(sinks), std::end(sinks));
		s_Chip8EmulatorLogger->set_level(spdlog::level::trace);
	}
}
