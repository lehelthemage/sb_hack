#include <windows.h>
#include <gl\gl.h>

BOOL __stdcall DllMain (HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			//DisableThreadLibraryCalls (hOriginalDll);
			//return Init();
			break;
		case DLL_PROCESS_DETACH:
			/*if ( hOriginalDll != NULL )
			{
				FreeLibrary(hOriginalDll);
				hOriginalDll = NULL;
			}*/
			break;
	}
	return TRUE;
}