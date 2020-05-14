#include "interpreters/Memory.hpp"
#include <cstdlib>

int main(int argc, char *argv[]) {
	bs::Tape tape = bs::Tape(30000);

	for(int i = 0; i < 30000; i++) {
		tape[i] = static_cast<unsigned char>(i % 255);
	}

	if(argc > 1) {
		tape.fPrint(std::atoi(argv[1]));
	} else {
		tape.fPrint(0);
		tape.fPrint(1);
		tape.fPrint(2);
		tape.fPrint(3);
		tape.fPrint(4);
		tape.fPrint(29995);
		tape.fPrint(29996);
		tape.fPrint(29997);
		tape.fPrint(29998);
		tape.fPrint(29999);
	}
}