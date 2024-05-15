#include <stdio.h>

#define CINI_NO_CPP
#define CINI_IMPLEMENTATION
#include <cini.h>

int main(int argc, char** argv) {
	for(int i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}

	

	return 0;
}
