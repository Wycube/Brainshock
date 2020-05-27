#include "Program.hpp"

namespace bs {

	/**
	 * Takes the program string in the program and iterates
	 * through it turning all the characters into Tokens, these
	 * Tokens don't have any meaning until they reach the interpreter.
	 * Note that the method clears the tokens vector before starting.
	 */
	void Program::tokenize() {
		tokens.clear();

		for(size_t i = 0; i < program.length(); i++) {
			tokens.push_back(Token{program[i], 1});
		}
	}

}