#pragma once

// Forward declaration of Emulator
class Emulator;
// Forward declaration of SDL types
struct SDL_Window;
struct SDL_Renderer;

class Shell
{
public:
	Shell();
	~Shell();

	bool Init();
	void Run();
	void Shutdown();

protected:
	bool m_Initialised = false;
	bool m_Running = false, m_RequestingRestart = false;

	SDL_Window* m_Window;
	SDL_Renderer* m_Renderer;

	Emulator* m_Emulator = nullptr;

	std::string m_ImguiIniPath;
};
