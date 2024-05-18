#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
	typedef lib_t HINSTANCE;
	#define LOAD_LIBRARY(libname) (lib_t)LoadLibrary(libname)
	#define GET_FUNCTION(lib, func) GetProcAddress(lib, func)
	#define CLOSE_LIBRARY(lib) FreeLibrary(lib)
#else
	#include <dlfcn.h>
	typedef lib_t void*;
	#define LOAD_LIBRARY(libname) (lib_t)dlopen(libname, RTLD_LAZY)
	#define GET_FUNCTION(lib, func) dlsym(lib, func)
	#define CLOSE_LIBRARY(lib) dlclose(lib)
#endif

#define GET_FUNC(funcdef, lib, funcname) (funcdef)GET_FUNCTION(lib, funcname)

class dll_lib {
protected:
	lib_t handle;
	const char* path;
public:
	dll_lib(const char* path);
	dll_lib() = delete;
	void* get(const char* functionname);
	~dll_lib();
};


#ifdef DLL_LIB_IMPLEMENTATION

dll_lib::dll_lib(const char* path) {
	handle = LOAD_LIBRARY(path);
	this->path = strdup(path);
}

void* dll_lib::get(const char* functionname) {
	return GET_FUNCTION(handle, functionname);
}

#undef DLL_LIB_IMPLEMENTATION
#endif // DLL_LIB_IMPLEMENTATION
