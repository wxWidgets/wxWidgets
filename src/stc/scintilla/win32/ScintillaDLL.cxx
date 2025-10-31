// Scintilla source code edit control
/** @file ScintillaDLL.cxx
 ** DLL entry point for Scintilla.
 **/
// Copyright 1998-2018 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#undef WINVER
#define WINVER 0x0500
#include <windows.h>

#include "Scintilla.h"
#include "ScintillaWin.h"

extern "C"
__declspec(dllexport)
sptr_t __stdcall Scintilla_DirectFunction(
    ScintillaWin *sci, UINT iMessage, uptr_t wParam, sptr_t lParam) {
	return Scintilla::DirectFunction(sci, iMessage, wParam, lParam);
}

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved) {
	//Platform::DebugPrintf("Scintilla::DllMain %d %d\n", hInstance, dwReason);
	if (dwReason == DLL_PROCESS_ATTACH) {
		if (!Scintilla_RegisterClasses(hInstance))
			return FALSE;
	} else if (dwReason == DLL_PROCESS_DETACH) {
		if (lpvReserved == NULL) {
			Scintilla::ResourcesRelease(true);
		}
	}
	return TRUE;
}
