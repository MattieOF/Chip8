#pragma once

// Utility functions and types
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

// Data types
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// Assertion
#include "ppk_assert.h"

// Core project files
#include "Core/Chip8EmulatorCore.h"
#include "Core/Chip8EmulatorLog.h"

// Platform specific
#ifdef C8_PLATFORM_WINDOWS
	#include "Windows.h"
#endif
