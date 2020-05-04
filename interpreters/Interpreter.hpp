#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <queue>
#include <iostream>
#include <string>
#include "Memory.hpp"

namespace bs {

	/** The flags used for setting certain things for the interpreters */
	struct InterpFlags {
		bool numInput = false; //Converts digit input from 1-9 to their actual value
		bool optimizeOne = false; //First level of optimization, generic/relation-based
		bool optimizeTwo = false; //Second level of optimization, language specific
	};

	/**
	 * A base interpreter class that contains the bare bones needed for
	 * a brainf interpreter.
	 */
	class Interpreter {
		friend class Interpreter_Interface;

		protected:

		Tape m_memory; //The memory tape, defualted to 30,000
		std::queue<unsigned char> m_inBuffer; //Input buffer

		unsigned char getInput();

		Interpreter(int memSize = 30000) : m_memory() {}
	};


	/**
	 * A class used to interface with an interpreter, so that
	 * the memory tape and getInput() method can be accessed but
	 * not the input buffer.
	 */
	class Interpreter_Interface {
		friend struct Instruction;

		Interpreter& m_interpreter;

		/**
		 * A function used to interface with the tape object in the interpreter. 
		 *
		 * @return A reference to the memory tape object.
		 */
		inline Tape& getTape() {
			return m_interpreter.m_memory;
		}

		/**
		 * A function used to interface with the input buffer in the interpreter.
		 *
		 * @return A char from the input buffer
		 */
		inline char getInput() {
			return m_interpreter.getInput();
		}

		Interpreter_Interface(Interpreter &interpreter) : m_interpreter(interpreter) {}
	};

}

#endif