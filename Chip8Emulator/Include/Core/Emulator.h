#pragma once

enum class CompatibilityMode
{
	Chip8,
	Chip8E,
	Chip16,
	Chip48,
	SuperChip,
	XOChip10,
	XOChip11,
	NumModes
};

FORCEINLINE const char* GetCompatibilityModeName(CompatibilityMode mode)
{
	switch (mode)
	{
	case CompatibilityMode::Chip8:
		return "Chip 8";
	case CompatibilityMode::Chip8E:
		return "Chip 8E";
	case CompatibilityMode::Chip16:
		return "Chip 16";
	case CompatibilityMode::Chip48:
		return "Chip 48";
	case CompatibilityMode::SuperChip:
		return "Super-Chip";
	case CompatibilityMode::XOChip10:
		return "XO-Chip 1.0";
	case CompatibilityMode::XOChip11:
		return "XO-Chip 1.1";
	default: // Clang says this is unreachable, but it is not! (Someone passing in an integer casted to CompatibilityMode that is out of range)
		return "Unknown";
	}
}

class Emulator
{
	friend class Shell;

public:
	[[nodiscard]] FORCEINLINE CompatibilityMode GetCompatibilityMode() const { return m_CompatibilityMode; }
	void SetCompatibilityMode(CompatibilityMode mode);

	// TODO: Ability to load empty rom for editing
	void LoadRom(CompatibilityMode mode, const std::string& path);
	void LoadRom(CompatibilityMode mode, const std::vector<uint8_t>& bytes);
	void LoadRom(const std::string& path);
	void LoadRomFromBytes(const std::vector<uint8_t>& bytes);

	bool WriteToMemory(int offset, const void* data, size_t size);
	FORCEINLINE bool WriteToMemory(int offset, const std::vector<uint8_t>& data)
	{
		return WriteToMemory(offset, data.data(), data.size());
	}

protected:
	void FDE();
	void Initialise();
	void CreateBuffers();
	void ResetEmulatorState();
	void ZeroMem();
	void ZeroDisplay();
	void AddFontToMemory();

	// --------------
	// Emulator state
	// --------------
	bool m_Running = false;
	uint8_t* m_Memory = nullptr;
	uint32_t m_CurrentMemorySize = 0;
	uint16_t m_ProgramCounter = 0x200;
	uint16_t m_IRegister = 0;
	std::stack<uint16_t> m_Stack;
	uint8_t m_DelayTimer = 0;
	uint8_t m_SoundTimer = 0;
	uint8_t m_VRegisters[16] = { 0 };
	uint8_t* m_Display = nullptr;
	uint16_t m_DisplayWidth = 64, m_DisplayHeight = 32;

	// -----------------
	// Emulator settings
	// -----------------
	CompatibilityMode m_CompatibilityMode = CompatibilityMode::Chip8;
	int m_CyclesPerSecond = 700;
	
#ifdef C8_DEBUG
	bool m_DebugLogs = true;
#else
	bool m_DebugLogs = false;
#endif
};
