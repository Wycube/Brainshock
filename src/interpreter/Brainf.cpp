#include "Brainf.hpp"

#include <iostream>
#include <chrono>
#include <thread>

namespace bs {

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
}


	BrainfInterpreter::BrainfInterpreter(std::size_t memSize) {
		m_memory = Tape(memSize);
	}

	bool BrainfInterpreter::stepProcessed(std::size_t numInstructions) {
		if(tokens.empty())
				return false;

		for(std::size_t i = 0; i < numInstructions; i++) {
			if(m_instPtr > m_program.length())
				return false;

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
				case CLEAR : m_memory[m_dataPtr] = 0;
				break;
				case COPY : m_memory[m_dataPtr + 1] = m_memory[m_dataPtr];
					          m_mempry[m_dataPtr + 2] = m_memory[m_dataPtr];
										m_memory[m_dataPtr] = 0;
				break;
			}
		}

		return true;
	}

	size_t handleStartLoop(unsigned char value, Program &program, std::size_t instPtr) {
		if(!value) {
			//Search forn ending bracket
			std::size_t i = instPtr;
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

	bool BrainfInterpreter::stepUnprocessed(std::size_t numInstructions) {
		for(std::size_t i = 0; i < numInstructions; i++) {
			if(m_instPtr > m_program.length())
				return false;

			char inst = m_program.program[m_instPtr++];

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
			}
		}

		return true;
	}

	/**
	* This steps the execution a certain number of steps, or to the
	* end whichever is first. 
	*
	* @param numInstructions The number of instructions to move forward by. 
	*
	* @return True if all the instructions were executed successfully.
	*/
	bool BrainfInterpreter::step(std::size_t numInstructions) {
		if(m_program.processed)
			return stepProcessed(numInstructions);
		else
			return stepUnprocessed(numInstructions);
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
		if(runSpeed < 0)
			runSpeed = 0; //Just do zero for anything negative

		bool regulate = !(runSpeed == 0);

		int milliPerInst = regulate ? 1 / runspeed : 0;
		std::chrono::milliseconds delta;
		auto execTime = std::chrono::milliseconds(milliPerInst);
		auto currentTime = std::chrono::steady_clock::now();
		auto lastTime = currentTime;

		if(!regulate) {
			for(std::size_t i = m_instPtr; i < program.length(); i++) {
			step();
			}
		} else {
			for(std::size_t i = m_instPtr; i < progSize; i++) {
					currentTime = std::chrono::steady_clock::now();
					delta = currentTime - lastTime;
					lastTime = currentTime;

				//Should the execution be stopped for a bit to stay in time
				if(delta > execTime) {
					std::this_thread::sleep_for(delta - execTime);
				}

				if(!step()) return false;
			}
		}

		return true;
	}

	/**
	* This method is responsible for putting certain data into tokens
	* and validation check, for [] it can put relative jump information.
	*
	* @return Whether or not m_program has valid syntax.
	*/
	bool BrainfInterpreter::expr() {
		std::deque<unsigned int> openLoops;

		for(std::size_t i = 0; i < m_program.length(); i++) {
			if(m_program[i] == '[') {
				openLoops.push_back(i);
			} else if(m_program[i] == ']') {
				if(openLoops.empty()) {
					return false;
				}

				//Assign jump back address
				m_program.tokens[i].data = openLoops.peek();

				//Assign jump forward address
				m_program.tokens[openLoops.peek()].data = i;

				openLoops.pop_back();
			}
		}

		return openLoops.empty();
	}

	/**
	 * Just a little helper function to
	 * tell if the two instructions oppose, or do the 
	 * opposite of eachother.
	 */
	bool isOpposing(char first, char second) {
		switch(first) {
			case SHIFT_RIGHT : return second == SHIFT_LEFT;
			case SHIFT_LEFT : return second == SHIFT_RIGHT;
			case INCREMENT : return second == DECREMENT;
			case DECREMENT : return second == INCREMENT;
			default : return false;
		}
	}

	/**
	* This method will do the optimization like folding repetive 
	* instructions into one and other creative things I can find
	* or think of, without modifying the behavior.
	*/
	void BrainfInterpreter::preProcess() {
		std::vector<Token> newTokens;

		std::size_t i;

		//First Pass
		while(i < m_program.length()) {
			char current = m_program.tokens[i].identifier;
			char previous = i > 0 ? m_program.tokens[i - 1].identifier : 0;
			char next = i < m_.program.length() - 1 ? m_program.tokens[i + 1].identifier : 0;
			

			if(current == SHIFT_LEFT || current == SHIFT_RIGHT ||
				 current == INCREMENT  || current == DECREMENT) {
				//Counts instructions and uses that as data for a single
				//instruction, like run length encoding

				int sum = 1;
				char temp = next;

				while(temp == current) {
					sum++;
					temp = m_program.tokens[i + sum].identifier;
				}

				newTokens.push_back(Token(current, sum));

				i += sum + 1;
			} else if(current == OPEN_LOOP) {
				
				//Check for very beginning of program
				//These are usually comments
				if(!i) {
					int count = 1;

					while(m_program.tokens[i].identifier != CLOSE_LOOP) {
						count++;
					}

					i += count + 1;
				} else if(next == DECREMENT || next == INCREMENT) {
					//Checks for the clear instruction
					if(m_program.tokens[i + 2].identifier == CLOSE_LOOP) {
						newTokens.push_back(Token(CLEAR, 1));

						i += 3;
					}
				} else if(previous == CLOSE_LOOP) {
					//Gets rid of loops that occur right after another loop
					char temp = next;
					int count = 1;

					while(temp != CLOSE_LOOP) {
						count++;
						temp = m_program.tokens[i + count];
					}

					i += count;
				} else if(next == DECREMENT) {
					//Check for copy instruction
					std::string expected = ">+>+<<]";//Sequence is [->+>+<<]
					std::string actual;

					for(std::size_t j = 0; j < 7; j++) {
						actual += m_program.tokens[i + j].identifier;
					}

					newTokens.push_back(Token(COPY, 1));

					i += 9;
				}
			}
		}

		//Replace the token lists in m_program
		m_program.tokens.swap(newTokens);
		newTokens.clear();


		//Second Pass
		while(i < m_program.length()) {
			char current = m_program.tokens[i].identifier;
			char next = i < m_.program.length() - 1 ? m_program.tokens[i + 1].identifier : 0;

			//Optimize for opposing operators <> +- ><
			if(current == SHIFT_LEFT || current == SHIFT_RIGHT ||
				 current == INCREMENT  || current == DECREMENT) {
				
				if(isOpposing(current, next)) {
					if(m_program.tokens[i].data == m_program.tokens[i + 1].data) {
						//Remove both
						m_program.tokens.erase(m_program.tokens.begin() + i);
						m_program.tokens.erase(m_program.tokens.begin() + i + 1);
					} else if(m_program.tokens[i].data > m_program.tokens[i + 1].data) {
						//Take away the seconds' data, from the firsts', and then remove it
						m_program.tokens[i].data -= m_program.tokens[i + 1].data;
						m_program.tokens.erase(m_program.tokens.begin() + i + 1);
						i++;
					} else {
						//Take away the firsts' data, from the seconds', then remove it
						m_program.tokens[i + 1].data -= m_program.tokens[i].data;
						m_program.tokens.erase(m_program.tokens.begin() + i);
						i++;
					}
				}
			}
		}

		m_program.processed = true;
	}

}