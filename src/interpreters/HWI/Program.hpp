#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <vector>

namespace bs {

	/** Instruction class that can be extended for different versions of Brainf */
	class Instruction {
		public:
		char m_symbol; //The symbol, '>', '<', '+', etc.
		const char *m_desc; //The description of the instruction
		int m_data; //Used primarily for loop jumps

		Instruction(char symbol = ' ', const char *desc = "", int data = 0) : m_symbol(symbol), m_desc(desc), m_data(data) {}
	
		inline void operator= (Instruction other) {
			this->m_symbol = other.m_symbol;
			this->m_desc =  other.m_desc;
			this->m_data = other.m_data;
		}
	};

	/** Program struct holding token[] and length vars */
	struct Program {
		public:

		std::vector<Instruction> m_tokens;

		Program(std::vector<Instruction> tokens = std::vector<Instruction>()) : m_tokens(tokens) {}

		inline bool hasNext(int instPtr) {return instPtr < m_tokens.size();}

		inline Instruction& operator[] (int index) {
			//Out-of-Bounds is undefined behavior

			return m_tokens[index];
		}
	};

}

#endif