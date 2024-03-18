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

void Emulator::ZeroMem()
{
	if (!m_Memory)
	{
		C8_ERROR("Memory buffer is null in ZeroMem");
		return;
	}
	memset(m_Memory, 0, m_CurrentMemorySize);
}
