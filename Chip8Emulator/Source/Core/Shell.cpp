#include "c8pch.h"
#include "Core/Shell.h"

#include <SDL3/SDL.h>
#include <imgui.h>
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"

Shell::Shell()
{ }

Shell::~Shell()
{ }

bool Shell::Init()
{
	// Initialize SDL.
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD))
	{
		C8_ERROR("SDL failed to initialize: {0}", SDL_GetError());
		return false;
	}

	// Enable native IME.
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

	// Create window and renderer.
	m_Window = SDL_CreateWindow("Chip8 Emulator", 1280, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
	if (!m_Window)
	{
		C8_ERROR("SDL failed to create window: {0}", SDL_GetError());
		return false;
	}
	m_Renderer = SDL_CreateRenderer(m_Window, nullptr, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (m_Renderer == nullptr)
	{
		C8_ERROR("SDL failed to create renderer: {0}", SDL_GetError());
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
	ImGui::StyleColorsDark();

	// Setup imgui backends
	if (!ImGui_ImplSDL3_InitForSDLRenderer(m_Window, m_Renderer))
		return false;
	if (!ImGui_ImplSDLRenderer3_Init(m_Renderer))
		return false;

	m_Initialised = true;

	return true;
}

void Shell::Run()
{
	do
	{
		Init();
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
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							m_Running = false;
							break;
						case SDLK_r:
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
			ImGui::ShowDemoWindow();

			// Draw imgui
			ImGui::Render();
			constexpr ImVec4 clearColor = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
			SDL_SetRenderDrawColor(m_Renderer, static_cast<Uint8>(clearColor.x * 255), static_cast<Uint8>(clearColor.y * 255),
				static_cast<Uint8>(clearColor.z * 255), static_cast<Uint8>(clearColor.w * 255));
			SDL_RenderClear(m_Renderer);
			ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());

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
