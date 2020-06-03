#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "Program.hpp"
#include "Memory.hpp"

#include <deque>

namespace bs {

	/**
	 * This is an abstract interpreter class, an interpreter for a 
	 * Brainf*** like language can be made by overriding this class.
	 * loadProgram() is final because that shouldn't differ between interpreters.
	 */
	class Interpreter {
	public:
		//The actual interpreting in these
		virtual bool run(float runSpeed = 0) = 0;
		virtual bool step(unsigned int numInstructions = 1) = 0;
	
		final bool loadProgram(char *program, bool process = true);

	private:

	std::deque<char> m_inBuffer;

	protected:

		Program m_program;
		Tape m_memory;
		size_t m_instPtr;
		size_T m_dataPtr;


		virtual bool expr() = 0;//checks for valid expressions
		virtual void preProcess() = 0;

		final char getChar();
	}

	inline bool Interpreter::loadProgram(char *program, bool process) {
		m_program = Program(program);

		if(process) {
			m_program.tokenize();
			
			if(!expr()) return false; //Invalid syntax

			preProcess();
		}

		return true;
	}

	inline char Interpreter::getChar() {
		unsigned char temp;

		if(m_inBuffer.empty()) {
			std::string input;
			
			std::getLine(std::cin, input);

			for(int i = 0; i < input.length(); i++) {
				m_inBuffer.push(input[i]);
			}
		}

		temp = m_inBuffer.front();
		m_inBuffer.pop();

		return temp;
	}

};

#endif