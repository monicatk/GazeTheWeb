#include <windows.h>
#define DLL_EXPORT __declspec(dllexport) // only on Windows

// Export C interface (resolved overloading etc)
#ifdef __cplusplus
extern "C" {
#endif

	// Interface
	DLL_EXPORT int HelloWorld();

#ifdef __cplusplus
}
#endif