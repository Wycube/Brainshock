#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "Program.hpp"
#include "Memory.hpp"

#include <deque>
#include <iostream>

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
		virtual bool step(std::size_t numInstructions = 1) = 0;
	
		inline bool loadProgram(char *program, bool process = true);

	private:

	std::deque<char> m_inBuffer;

	protected:

		Program m_program;
		Tape m_memory;
		std::size_t m_instPtr;
		std::size_t m_dataPtr;


		virtual bool expr() = 0; //checks for valid expressions
		virtual void preProcess() = 0;

		inline char getChar();
	};

	inline bool Interpreter::loadProgram(char *program, bool process) {
		m_program = Program();
		m_program.program = std::string(program);

		if(process) {
			m_program.tokenize();
			
			if(!expr()) return false; //Invalid syntax

			preProcess();
		}

		return true;
	}

	inline char Interpreter::getChar() {
		char temp;

		if(m_inBuffer.empty()) {
			std::string input;
			
			std::getline(std::cin, input);

			for(int i = 0; i < input.length(); i++) {
				m_inBuffer.push_front(input[i]);
			}
		}

		temp = m_inBuffer.front();
		m_inBuffer.pop_back();

		return temp;
	}

};

#endif