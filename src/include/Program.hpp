#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <string>
#include <vector>

namespace bs {

	struct Token {
		char identifier;
		int data;
	};

	struct Program {
	public:

	std::string program;
	std::vector<Token> tokens;
	bool processed;

	void tokenize();
	};

}

#endif //PROGRAM_HPP