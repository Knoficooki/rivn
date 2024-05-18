#include <stdio.h>

#define CINI_NO_CPP
#define CINI_IMPLEMENTATION
#include <cini/cini.h>

#include <lib.h>

typedef int *(libfunc)(int argc, char** argv);

char* getifarg(int argn, int argc, char** argv) {
	
}

int main(int argc, char** argv) {
	for(int i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}
	dll_lib lib("setup.so");
	(libfunc)(lib.get())(argc, argv);

	

	return 0;
}
