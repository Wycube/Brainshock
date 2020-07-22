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
	 * loadProgram() is defined because that shouldn't differ between interpreters.
	 */
	class Interpreter {
	public:
		//The actual interpreting in these
		virtual bool run(float runSpeed = 0) = 0;
		virtual bool step() = 0;
	
		inline bool loadProgram(const char *program, bool process = true, bool resetDataPtr = true);

		//Getters
		inline Program& getProgram() { return m_program; }
		inline Tape& getMemory() { return m_memory; }
		inline std::size_t getInstPtr() { return m_instPtr; }
		inline std::size_t getDataPtr() { return m_dataPtr; }

		inline std::string getError() { return m_error; }

	private:

	std::deque<char> m_inBuffer;

	protected:

		virtual bool expr() = 0; //Checks for valid expressions
		virtual void preProcess() = 0;

		inline char getChar();

		Program m_program;
		Tape m_memory;
		std::size_t m_instPtr = 0;
		std::size_t m_dataPtr = 0;
		std::string m_error;
	};

	inline bool Interpreter::loadProgram(const char *program, bool process, bool resetDataPtr) {
		m_program = Program();
		m_program.program = std::string(program);

		m_instPtr = 0;

		if(resetDataPtr)
			m_dataPtr = 0;

		if(process) {
			m_program.tokenize();

			if(!expr()) return false; //Program has invalid syntax

			preProcess();

			//This is called to update the jump locations of the brackets
			if(!expr()) return false; //If there was an error in the optimizing, which shouldn't happen 
		}

		return true;
	}

	inline char Interpreter::getChar() {
		char temp;

		if(m_inBuffer.empty()) {
			std::string input;
			
			//If there was nothing entered prompt again
			while(input == "")
				std::getline(std::cin, input);

			for(std::size_t i = 0; i < input.length(); i++) {
				m_inBuffer.push_front(input[i]);
			}
		}

		temp = m_inBuffer.back();
		m_inBuffer.pop_back();

		return temp;
	}

};



#endif
