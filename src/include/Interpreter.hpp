#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "Program.hpp"
#include "Memory.hpp"

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
		virtual bool step(int numInstructions = 1) = 0;
	
		final bool loadProgram(char *program, bool process = true);

	private:
		Program m_program;
		Tape m_memory;

		virtual bool expr() = 0;//checks for valid expressions
		virtual void preProcess() = 0;
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

};

#endif