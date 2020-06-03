#include "Brainf.hpp"

#include <iostream>

enum BrainfInstructions {
	SHIFT_RIGHT = '>',
	SHIFT_LEFT = '<',
	INCREMENT = '+',
	DECREMENT = '-',
	START_LOOP = '[',
	END_LOOP = ']',
	INPUT = ',',
	OUTPUT = '.'
}

namespace bs {

	BrainfInterpreter::BrainfInterpreter(size_t memSize) {
		m_memory = Tape(memSize);
	}

	bool stepProcessed(unsigned int numInstructions) {
		if(tokens.empty())
				return false;

		for(size_t i = 0; i < numInstructions; i++) {
			Token inst = m_program.tokens[m_instPtr++];

			switch(inst.identifier) {
				case SHIFT_RIGHT : m_dataPtr += inst.data;
				break;
				case SHIFT_LEFT : m_dataPtr -= inst.data;
				break;
				case INCREMENT : m_memory[m_dataPtr] += inst.data;
				break;
				case DECREMENT : m_memory[m_dataPtr] -= inst.data;
				break;
				case START_LOOP : if(!m_memory[m_dataPtr]) m_instPtr += inst.data;//equal to zero
				break;
				case END_LOOP : if(m_memory[m_dataPtr]) m_instPtr -= inst.data;//Anything but zero
				break;
				case INPUT : m_memory[m_dataPtr] = getChar();
				break;
				case OUTPUT : std::cout << m_memory[m_dataPtr];
				break;
				default : break;
			}
		}
	}

	size_t handleStartLoop(unsigned char value, Program &program, size_t instPtr) {
		if(!value) {
			//Search forn ending bracket
			size_t i = instPtr;
			char inst = m_program.program[i];

			while(inst != ']') {
				inst = m_program.program[++i];
			}

			return i;
		} else {
			//Indicates that this bracket should be pushed onto the jump table
			return 0; 
		}
	}

	bool stepUnprocessed(unsigned int numInstructions) {
		for(size_t i = 0; i < m_program.program.length(); i++) {
			char inst = m_program.program[i];

			switch(inst) {
				case SHIFT_RIGHT : m_dataPtr++;
				break;
				case SHIFT_LEFT : m_dataPtr--;
				break;
				case INCREMENT : m_memory[m_dataPtr]++;
				break;
				case DECREMENT : m_memory[m_dataPtr]--;
				break;
				case START_LOOP :
					size_t jumpValue = handleStartLoop(m_memory[m_dataPtr], m_program, m_instPtr);

					if(!jumpValue)
						m_jumpTable.push_back(m_instPtr);
					else
						m_instPtr = jumpValue;
				break;
				case END_LOOP : 
					if(m_memory[m_dataPtr])
						m_instPtr = m_jumpTable.back();
					else
						m_jumpTable.pop_back();
				break;
				case INPUT : m_memory[m_dataPtr] = getChar();
				break;
				case OUTPUT : std::cout << m_memory[m_dataPtr];
				break;
				default : break;
			}
		}
	}

	/**
	* This steps the execution a certain number of steps, or to the
	* end whichever is first. 
	*
	* @param numInstructions The number of instructions to move forward by. 
	*
	* @return True if all the instructions were executed successfully.
	*/
	bool BrainfInterpreter::step(unsigned int numInstructions) {
		
	}

	/**
	* This is the run function, which run speed can be adjusted and is
	* controlled by some chrono utilities. It basically just steps and
	* checks the delta time, waits or steps again. It is not guaranteed
	* to go as fast as set but it will try. Also zero means as fast as possible.
	*
	* @param runSpeed The speed to run at in instructions per second
	*
	* @return True if the program executed successfully.
	*/
	bool BrainfInterpreter::run(float runSpeed) {

	}

	/**
	* This method is responsible for putting certain data into tokens
	* and validation check, for [] it can put relative jump information.
	*
	* @return Whether or not m_program has valid syntax.
	*/
	bool BrainfInterpreter::expr() {

	}

	/**
	* This method will do the optimization like folding repetive 
	* instructions into one and other creative things I can find
	* or think of, without modifying the behavior.
	*/
	void BrainfInterpreter::preProcess() {
		//Optimizations *yay*
	}

}