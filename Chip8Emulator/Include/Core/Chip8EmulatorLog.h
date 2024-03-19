#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h" // Needed for logging some types

extern std::shared_ptr<spdlog::logger> s_Chip8EmulatorLogger;

void InitLog(const char* prefPath);

#ifndef C8_NO_LOG
	#define C8_TRACE(...)       s_Chip8EmulatorLogger->trace(__VA_ARGS__)
	#define C8_INFO(...)        s_Chip8EmulatorLogger->info(__VA_ARGS__)
	#define C8_WARN(...)        s_Chip8EmulatorLogger->warn(__VA_ARGS__)
	#define C8_ERROR(...)       s_Chip8EmulatorLogger->error(__VA_ARGS__)
	#define C8_CRITICAL(...)    s_Chip8EmulatorLogger->critical(__VA_ARGS__)
#else			 
	#define C8_TRACE(...)      
	#define C8_INFO(...)       
	#define C8_WARN(...)       
	#define C8_ERROR(...)      
	#define C8_CRITICAL(...)   
#endif
