#include "Utils.hpp"

namespace utils
{
	bool IsKeyDown(DWORD VK)
	{
		return (GetAsyncKeyState(VK) & 0x8000) != 0;
	}

}