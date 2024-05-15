#include <stdio.h>

#define CINI_NO_CLASS_SYSTEM
#define CINI_IMPLEMENTATION
#include <cini/cini.h>

int libexample(int argc, char** argv) {

}

char* getifarg(int argn, int argc, char** argv) {
	
}

int main(int argc, char** argv) {
	for(int i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}
	
	

	return 0;
}
