#include "c8pch.h"

#include <SDL3/SDL_main.h>
#include "Core/Shell.h"

int SDL_main(int argc, char* argv[])
{
	Shell* shell = new Shell();
	shell->Run();
	delete shell;

	C8_TRACE("Goodbye :)");
	return 0;
}
