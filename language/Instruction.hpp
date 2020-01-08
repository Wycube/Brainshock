#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include "../interpreters/Interpreter.hpp"

namespace bs {

	/**
	* A structure representing the definition of the most basic form of a brainf type instruction
	* it has what it looks like, the char*, what it does, the function pointer,
	* and a description.
	*/
	struct Instruction_Def {
		const char *m_identifier; //What the instruction looks like ex. '+', '-'
		const char *m_desc; //A description of what this instruction does
		const void (*m_func)(Interpreter_Interface&); //What the instruction does

		Instruction_Def(const char *identifier, const void (*func)(Interpreter_Interface&)) : m_identifier(identifier), m_func(func) {}
	};


	/**
	 * A structure representing a reference to a definition of an instruction via a 
	 * integer index of the list of instructions in a language, and a generic numeric data
	 * variable.
	 */
	struct Instruction_Ref {
		friend struct Language;

		int m_index; //Instructions index in the language instruction list
		int m_data; //Instructions commonly have some sort of data with them, jump value, increment number, etc


		void operator= (Instruction_Ref other) {
			this->m_index = other.m_index;
			this->m_data = other.m_data;
		}

		private:

		Instruction_Ref(int index, int data = 0) : m_index(index), m_data(data) {}
	};

}

#endif