#include "pch.h"

#include "Misc.h"

namespace Engine::Util {

	std::string WstToStr(const std::wstring& wstr)
	{
		const wchar_t *str = wstr.c_str();
		char mbs[256] = { 0 };
		mbstate_t shiftState = mbstate_t();
		setlocale(LC_ALL, "");
		wcsrtombs_s(nullptr, mbs, wstr.size() + 1, &str, 256, &shiftState);

		return mbs;
	}

}