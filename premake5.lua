workspace "Chip8Emulator"
	configurations { "Debug", "Release", "Dist" }
	platforms { "Win64" }
	startproject "Chip8Emulator"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["spdlog"] = "Chip8Emulator/Vendor/spdlog/include"
IncludeDir["PPK_ASSERT"] = "Chip8Emulator/Vendor/PPK_ASSERT/Include"
IncludeDir["SDL"] = "Chip8Emulator/Vendor/SDL/include"
IncludeDir["imgui"] = "Chip8Emulator/Vendor/imgui/"

include "Chip8Emulator/Vendor/imgui.lua"

project "Chip8Emulator"
	kind "ConsoleApp"
	staticruntime "On"
	language "C++"
	location "Chip8Emulator"
	targetdir ("Build/%{prj.name}/" .. outputdir)
	objdir ("Build/%{prj.name}/Intermediates/" .. outputdir)

	pchheader "c8pch.h"
	pchsource "Chip8Emulator/Source/c8pch.cpp"

	vpaths {
		["Include"] = {"Chip8Emulator/Include/**.h", "Chip8Emulator/Include/**.hpp"},
		["Source"] = {"Chip8Emulator/Source/**.cpp", "Chip8Emulator/Source/**.c"},
	}

	files { 
		"Chip8Emulator/Include/**.h", "Chip8Emulator/Include/**.hpp", 
		"Chip8Emulator/Source/**.cpp", "Chip8Emulator/Source/**.c",

		"Chip8Emulator/Vendor/PPK_ASSERT/Source/*.cpp"
	}

	includedirs 
	{ 
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.PPK_ASSERT}",
		"%{IncludeDir.SDL}",
		"%{IncludeDir.imgui}",

		"Chip8Emulator/Include"
	}

	libdirs 
	{
		"Chip8Emulator/Vendor/SDL/lib"
	}

	links
	{
		"imgui",
		"SDL3-static"
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

os.mkdir("Chip8Emulator/Source")
os.mkdir("Chip8Emulator/Include")

filter "configurations:Debug"
	defines { "C8_DEBUG", "C8_ENABLE_ASSERTS" }
	symbols "On"
	runtime "Debug"

filter "configurations:Release"
	defines { "C8_RELEASE", "C8_ENABLE_ASSERTS" }
	optimize "On"
	symbols "On"
	runtime "Release"

filter "configurations:Dist"
	defines { "C8_DIST" }
	kind "WindowedApp"
	optimize "On"
	symbols "Off"
	runtime "Release"

filter "system:windows"
	cppdialect "C++17"
	systemversion "latest"
	defines { "C8_PLATFORM_WINDOWS" }

	links
	{
		"version",
		"winmm",
		"Imm32",
		"Cfgmgr32",
		"Setupapi"
	}

filter "platforms:Win64"
	system "Windows"
	architecture "x64"
