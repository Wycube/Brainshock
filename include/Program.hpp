#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <string>
#include <vector>
#include <deque>
#include <cstddef>

namespace bs {

	struct Token {
		char identifier;
		unsigned int data;
	};

	class Program {
	public:
	
		std::string source; //Raw program, as string
		std::vector<Token> tokens; //Preprocessed into Token intermediates
		bool processed;

		void tokenize();

		inline char& operator[](std::size_t index) {
			if(processed) {
				return tokens[index].identifier;
			} else {
				return source[index];
			}
		}
	};

	// The Intermediate Representation
	enum BrainfInstructions {
		SHIFT_RIGHT = '>',
		SHIFT_LEFT = '<',
		INCREMENT = '+',
		DECREMENT = '-',
		START_LOOP = '[',
		END_LOOP = ']',
		INPUT = ',',
		OUTPUT = '.',
		//Extended Tokens for optimization
		CLEAR = 'z',
		COPY = 'c'
	};

	class IREmitter {
	private:

		Program m_source;
		std::string m_error;

	public:

		IREmitter() { }
		IREmitter(const char *source);

		void loadSource(const char *source);
		void optimize(unsigned int level = 2);
		bool expr();
		Program emit();

		inline std::string getError() { return m_error; };
	};

}

#endif //PROGRAM_HPP
