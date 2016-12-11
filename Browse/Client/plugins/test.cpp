#include <windows.h>
#define DLL_EXPORT __declspec(dllexport) // only on Windows

// Export C interface (resolved overloading etc)
#ifdef __cplusplus
extern "C" {
#endif

DLL_EXPORT int HelloWorld()
{
	return 42;
}

#ifdef __cplusplus
}
#endif