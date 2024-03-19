#include <c8pch.h>
#include "Core/Emulator.h"

void Emulator::SetCompatibilityMode(CompatibilityMode mode)
{
	m_CompatibilityMode = mode;
	Initialise();
}

void Emulator::LoadRom(CompatibilityMode mode, const std::string& path)
{
	SetCompatibilityMode(mode);
	LoadRom(path);
}

void Emulator::LoadRom(CompatibilityMode mode, const std::vector<uint8_t>& bytes)
{
	SetCompatibilityMode(mode);
	LoadRomFromBytes(bytes);
}

void Emulator::LoadRom(const std::string& path)
{
	std::ifstream file(path);
	if (file.is_open())
	{
		std::vector<uint8_t> bytes;
		file.seekg(0, std::ios::end);
		bytes.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(bytes.data()), bytes.size());
		file.close();

		LoadRomFromBytes(bytes);
	}
	else
	{
		C8_ERROR("Failed to open file: {}", path);
	}
}

void Emulator::LoadRomFromBytes(const std::vector<uint8_t>& bytes)
{
	if (!m_Memory)
		Initialise();
}

void Emulator::Initialise()
{
	CreateBuffers();
	ResetEmulatorState();
	AddFontToMemory();

	switch (m_CompatibilityMode)
	{
	case CompatibilityMode::Chip8:
		return;
	case CompatibilityMode::Chip8E:
	case CompatibilityMode::Chip16:
	case CompatibilityMode::Chip48:
	case CompatibilityMode::SuperChip:
	case CompatibilityMode::XOChip:
		C8_ERROR("Unimplemented compatibility mode: {0}", static_cast<int>(m_CompatibilityMode));
		break;
	default:
		C8_ERROR("Unknown compatibility mode: {0}", static_cast<int>(m_CompatibilityMode));
		return;
	}
}

void Emulator::CreateBuffers()
{
	// First, create the memory buffer
	// Let's find the size of the memory buffer based on the compatibility mode
	uint32_t memorySize = 0;
	switch (m_CompatibilityMode)
	{
	case CompatibilityMode::Chip8:
	case CompatibilityMode::Chip8E:
		memorySize = 4096;
		break;
	case CompatibilityMode::Chip16:
	case CompatibilityMode::Chip48:
	case CompatibilityMode::SuperChip:
	case CompatibilityMode::XOChip:
		memorySize = 65536;
		break;
	default:
		C8_ERROR("Unknown compatibility mode in CreateBuffers: {0}", static_cast<int>(m_CompatibilityMode));
		return;
	}

	// Now, let's create the memory buffer
	uint8_t* newMemory = new uint8_t[memorySize];
	if (m_Memory != nullptr)
	{
		// If there is currently memory, copy it over to the new memory
		// This makes switching compatibility modes during runtime possible - even if it's not recommended
		// Ensure we don't copy more than the new memory size
		if (m_DebugLogs && m_CurrentMemorySize > memorySize)
		{
			C8_INFO("Current memory size is larger than new memory size: {0} bytes > {1} bytes", m_CurrentMemorySize, memorySize);
		}
		m_CurrentMemorySize = std::min(m_CurrentMemorySize, memorySize);
		if (m_DebugLogs)
			C8_INFO("Copying {0} bytes of memory to new memory", m_CurrentMemorySize);
		memcpy(newMemory, m_Memory, m_CurrentMemorySize);
		delete[] m_Memory;
	}
	m_Memory = newMemory;
	m_CurrentMemorySize = memorySize;
	if (m_DebugLogs)
		C8_INFO("Memory buffer created with size: {0} bytes", m_CurrentMemorySize);
}

void Emulator::ResetEmulatorState()
{
	if (m_DebugLogs)
		C8_INFO("Resetting emulator state");
	if (!m_Memory)
	{
		C8_ERROR("ResetEmulatorState called before initialisation");
		return;
	}
	ZeroMem();
	m_ProgramCounter = 0x200;
	m_IRegister = 0;
	while (!m_Stack.empty()) // Clear the stack
		m_Stack.pop();
	m_DelayTimer = 0;
	m_SoundTimer = 0;
	for (int i = 0; i < 16; i++)
		m_VRegisters[i] = 0;
}

void Emulator::ZeroMem()
{
	C8_INFO("Zeroing memory");
	if (!m_Memory)
	{
		C8_ERROR("Memory buffer is null in ZeroMem");
		return;
	}
	memset(m_Memory, 0, m_CurrentMemorySize);
}

void Emulator::AddFontToMemory()
{
	const std::vector<uint8_t> font = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	// Apparently, most implementations of the CHIP-8 interpreter start the font at 0x050
	memcpy(m_Memory + 0x050, font.data(), font.size());
}
