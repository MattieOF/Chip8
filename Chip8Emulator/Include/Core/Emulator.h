#pragma once

enum class CompatibilityMode
{
	Chip8,
	Chip8E,
	Chip16,
	Chip48,
	SuperChip,
	XOChip
};

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

protected:
	void Initialise();
	void CreateBuffers();
	void ZeroMem();

	uint8_t* m_Memory = nullptr;
	uint32_t m_CurrentMemorySize = 0;

#ifdef C8_DEBUG
	bool m_DebugLogs = true;
#else
	bool m_DebugLogs = false;
#endif

	CompatibilityMode m_CompatibilityMode = CompatibilityMode::Chip8;
};
