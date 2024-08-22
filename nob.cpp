#define NOB_IMPLEMENTATION
#include "nob.hpp"


int main(int argc, char **argv) {
	nob::instance nob(argc, argv);
	nob.log << nob::INFO << "Hello!";
	nob.log << nob::MIN << "Test This Too!";
	nob.log << nob::DEBUG << "Hello World";
	return nob.code();
}
