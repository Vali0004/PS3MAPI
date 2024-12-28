#include "ps3mapi.h"
#define LINK_BINDINGS
#include "c_bindings.h"

#if defined(_DLL) && !defined(_LIB)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReasonForCall, LPVOID lpReserved)
{
	// We don't need to do much here
	return TRUE;
}
#endif