#include "src/interpreter/Memory.hpp"
#include <cstdlib>

int main(int argc, char *argv[]) {
	bs::Tape tape = bs::Tape(30000);

	for(int i = 0; i < 1000; i++) {
		tape[i] = static_cast<unsigned char>(i % 255);
	}

	// for(int i = 30000; i > 29000; i--) {
	// 	tape[i] = static_cast<unsigned char>(i % 255);
	// }

	tape.fDump(bs::BASE_HEX, false);
}