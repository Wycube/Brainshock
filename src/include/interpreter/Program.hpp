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
		std::string program; //Raw program, as string
		std::vector<Token> tokens; //Preprocessed into Token intermediates
		bool processed;

		void tokenize();

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
