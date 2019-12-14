#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <vector>

namespace bs {

	/** Operation class that can be extended for different versions of Brainf */
	class Operation {
		public:
		char m_symbol; //The symbol, '>', '<', '+', etc.
		const char *m_desc; //The description of the token
		int m_data; //Used primarily for loop jumps

		Operation(char symbol = ' ', const char *desc = "", int data = 0) : m_symbol(symbol), m_desc(desc), m_data(data) {}
	
		void operator= (Operation other) {
			this->m_symbol = other.m_symbol;
			this->m_desc =  other.m_desc;
			this->m_data = other.m_data;
		}
	};

	/** Program struct holding token[] and length vars */
	struct Program {
		public:

		std::vector<Operation> m_tokens;

		Program(std::vector<Operation> tokens = std::vector<Operation>()) : m_tokens(tokens) {}

		bool hasNext(int instPtr) {return instPtr < m_tokens.size();}

		Operation& operator[] (int index) {
			//Out-of-Bounds is undefined behavior

			return m_tokens[index];
		}
	};

}

#endif