#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <string>
#include <vector>
#include <cstddef>

namespace bs {

	struct Token {
		char identifier;
		unsigned int data;
	};

	struct Program {

	std::string program;
	std::vector<Token> tokens;
	bool processed;

	void tokenize();
	std::size_t length();

	inline char& operator[](std::size_t index) {
		if(processed) {
			return tokens[index].identifier;
		} else {
			return program[index];
		}
	}
	};

}

#endif //PROGRAM_HPP