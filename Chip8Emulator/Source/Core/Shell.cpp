#include "c8pch.h"
#include "Core/Shell.h"

#include <SDL3/SDL.h>
#include <imgui.h>

#include "Core/Emulator.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

Shell::Shell()
{ }

Shell::~Shell()
{ }

bool Shell::Init()
{
	// Initialize SDL.
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD) != SDL_TRUE)
	{
		// If SDL fails to initialise, we must be running on a literal potato.
		// We can try to show a message box (but if SDL failed to initialise, it's unlikely to work).
		const std::string message = fmt::format("Failed to initialise SDL: {0}", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to initialise", message.c_str(), nullptr);

		// For robustness, we can initialise our logging system without the preferences path and log the error.
		InitLog(nullptr);
		C8_ERROR("{0}", message);
		
		return false;
	}

	// Now that SDL is initialised, we can create the log file with the preferences' path.
	const char* prefPath = SDL_GetPrefPath("Matt Ware", "Chip8");
	InitLog(prefPath);
	C8_TRACE("Hello! Welcome to Chip-8 :)");
	C8_TRACE("	Saving stuff to: {0}", prefPath);
	
	// Create the window.
	m_Window = SDL_CreateWindow("Chip8 Emulator", 1280, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
	if (!m_Window)
	{
		const std::string message = fmt::format("Failed to create SDL window: {0}", SDL_GetError());
		C8_ERROR("{0}", message);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to initialise", message.c_str(), nullptr);
		
		SDL_Quit();
		
		return false;
	}

	// And the renderer.
	m_Renderer = SDL_CreateRenderer(m_Window, nullptr);
	if (m_Renderer == nullptr)
	{
		const std::string message = fmt::format("Failed to create SDL renderer: {0}", SDL_GetError());
		C8_ERROR("{0}", message);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to initialise", message.c_str(), nullptr);

		// Clean up what we've created so far.
		SDL_DestroyWindow(m_Window);
		m_Window = nullptr;
		SDL_Quit();
		
		return false;
	}
	SDL_SetWindowPosition(m_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(m_Window);

	// Set up imgui.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Save the imgui settings to the preferences path, so we don't clutter wherever the user runs the emulator from.
	// For applications that ship as multiple files, saving directly to the executable directory is probably preferred (see unreal games),
	// but since I want to keep the emulator as a single file, I'll avoid writing outside the preferences' path.
	if (prefPath != nullptr) // If it doesn't exist, we'll just use the default imgui.ini location. 
	{                        // TODO: what's the bigger sin; not saving the settings, or cluttering the user's directory?
		// Save it to a member variable, so that the cstr we pass to imgui is valid for the lifetime of the shell.
		m_ImguiIniPath = fmt::format("{0}imgui.ini", prefPath);
		io.IniFilename = m_ImguiIniPath.c_str();
	}
	else
		C8_ERROR("No valid preferences path; saving imgui settings to default location (working directory).");
	ImGui::StyleColorsDark();

	// Setup imgui backends
	// First, base SDL backend
	if (!ImGui_ImplSDL3_InitForSDLRenderer(m_Window, m_Renderer))
	{
		// Looking at the source code, it seems it never returns false... but just in case!
		const std::string message = fmt::format("Failed to initialise SDL3 backend for imgui. SDL Error:", SDL_GetError());
		C8_ERROR("{0}", message);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to initialise", message.c_str(), nullptr);

		// Clean up what we've created so far.
		SDL_DestroyRenderer(m_Renderer);
		m_Renderer = nullptr;
		SDL_DestroyWindow(m_Window);
		m_Window = nullptr;
		SDL_Quit();
		
		return false;
	}

	// And the SDL renderer backend
	if (!ImGui_ImplSDLRenderer3_Init(m_Renderer))
	{
		const std::string message = fmt::format("Failed to initialise SDL3 renderer backend for imgui. SDL Error: {0}", SDL_GetError());
		C8_ERROR("{0}", message);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to initialise", message.c_str(), nullptr);

		// Clean up what we've created so far.
		ImGui_ImplSDL3_Shutdown();
		SDL_DestroyRenderer(m_Renderer);
		m_Renderer = nullptr;
		SDL_DestroyWindow(m_Window);
		m_Window = nullptr;
		SDL_Quit();
		
		return false;	
	}

	// Create emulator
	m_Emulator = new Emulator();

	m_Initialised = true;

	return true;
}

void Shell::Run()
{
	do
	{
		if (!Init())
			return;
		m_Running = true;
		while (m_Running)
		{
			// Process events
			SDL_Event event;
			if (SDL_PollEvent(&event))
			{
				ImGui_ImplSDL3_ProcessEvent(&event);
				if (event.type == SDL_EVENT_QUIT)
					m_Running = false;
				if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(m_Window))
					m_Running = false;
				if (event.type == SDL_EVENT_KEY_DOWN)
				{
					switch (event.key.key)
					{
						case SDLK_ESCAPE:
							m_Running = false;
							break;
						case SDLK_R:
							m_RequestingRestart = !m_RequestingRestart;
							if (m_RequestingRestart)
								C8_INFO("The shell will restart when closed.");
							else
								C8_INFO("The shell will shut down fully when closed.");
							break;
						default:
							break;
					}
				}
			}

			// Draw c-8

			// Begin imgui frame
			ImGui_ImplSDLRenderer3_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();

			// Draw UI
			ImGui::Begin("Emulator Playground", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
			// TODO: Make this a combo box
			if (ImGui::BeginCombo("Compatibility Mode", GetCompatibilityModeName(m_Emulator->GetCompatibilityMode())))
			{
				for (int i = 0; i < static_cast<int>(CompatibilityMode::NumModes); i++)
				{
					const bool isSelected = m_Emulator->GetCompatibilityMode() == static_cast<CompatibilityMode>(i);
					if (ImGui::Selectable(GetCompatibilityModeName(static_cast<CompatibilityMode>(i)), isSelected))
						m_Emulator->SetCompatibilityMode(static_cast<CompatibilityMode>(i));
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::Checkbox("Debug Logs", &m_Emulator->m_DebugLogs);
			ImGui::End();

			// Draw imgui
			ImGui::Render();
			constexpr ImVec4 clearColor = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
			SDL_SetRenderDrawColor(m_Renderer, static_cast<Uint8>(clearColor.x * 255), static_cast<Uint8>(clearColor.y * 255),
				static_cast<Uint8>(clearColor.z * 255), static_cast<Uint8>(clearColor.w * 255));
			SDL_RenderClear(m_Renderer);
			ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_Renderer);
			
			// Finally, present the frame.
			SDL_RenderPresent(m_Renderer);
		}
		Shutdown();
	} while (m_RequestingRestart);
}

void Shell::Shutdown()
{
	if (!m_Initialised)
		return;

	delete m_Emulator;

	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(m_Renderer);
	m_Renderer = nullptr;
	SDL_DestroyWindow(m_Window);
	m_Window = nullptr;

	SDL_ResetHints();
	SDL_Quit();

	m_Initialised = false;
}
