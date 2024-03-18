#include "c8pch.h"

#include "Core/Shell.h"

int main()
{
	InitLog();
	C8_TRACE("Hello! Welcome to Chip-8 :)");

	Shell* shell = new Shell();
	shell->Run();
	delete shell;

	C8_TRACE("Goodbye :)");
}
