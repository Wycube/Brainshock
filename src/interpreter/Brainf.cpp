#include "Brainf.hpp"

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
};


	BrainfInterpreter::BrainfInterpreter(std::ostream &stream, std::size_t memSize) : m_stream(stream) {
		m_memory = Tape(memSize);
	}

	std::string checkMemoryError(Tape &memory, char instruction, std::size_t instPtr) {
		std::string error;
		
		if(memory.outOfBounds) {
			error = "Out-of-Bounds memory access on instruction '";
			error += instruction;
			error += "' at character ";
			error += std::to_string(instPtr + 1);
			memory[0]; //This is just to reset the error value internally
		}

		return error;
	}
	
	bool BrainfInterpreter::loadProgram(const char *program, bool process, bool resetDataPtr, unsigned int optimization) {
		m_program = Program();
		m_program.program = std::string(program);

		m_instPtr = 0;

		if(resetDataPtr)
			m_dataPtr = 0;

		if(process) {
			m_program.tokenize();

			if(!expr()) return false; //Program has invalid syntax

			if(optimization > 0) {
				preProcess(optimization);

				//This is called to update the jump locations of the brackets
				if(!expr()) return false; //If there was an error in the optimizing, which shouldn't happen
			}	
		}

		return true;
	}

	char BrainfInterpreter::getChar() {
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

	bool BrainfInterpreter::stepProcessed() {
		if(m_program.tokens.empty()) {
			m_error = "No program provided";
			return false;
		} else if(m_instPtr > m_program.tokens.size()) {
			m_error = "Execution gone past end of program";
			return false;
		}

		Token inst = m_program.tokens[m_instPtr];

		switch(inst.identifier) {
			case SHIFT_RIGHT : m_dataPtr += inst.data;
			break;
			case SHIFT_LEFT : m_dataPtr -= inst.data;
			break;
			case INCREMENT : m_memory[m_dataPtr] += inst.data;
			break;
			case DECREMENT : m_memory[m_dataPtr] -= inst.data;
			break;
			case START_LOOP : if(m_memory[m_dataPtr] == 0) m_instPtr = inst.data;
			break;
			case END_LOOP : if(m_memory[m_dataPtr] != 0) m_instPtr = inst.data;
			break;
			case INPUT : m_memory[m_dataPtr] = getChar();
			break;
			case OUTPUT : m_stream << m_memory[m_dataPtr] << std::flush;
			break;
			case CLEAR : m_memory[m_dataPtr] = 0;
			break;
			case COPY : m_memory[m_dataPtr + 1] = m_memory[m_dataPtr];
				    m_memory[m_dataPtr + 2] = m_memory[m_dataPtr];
				    m_memory[m_dataPtr]     = 0;
			break;
		}

		//Check for out-of-bounds memory access
		std::string error = checkMemoryError(m_memory, inst.identifier, m_instPtr);
		if(error != "") {
			m_error = error;
			return false;
		}


		m_instPtr++;

		return true;
	}

	size_t handleStartLoop(unsigned char value, Program &program, std::size_t instPtr, bool &success) {
		success = true;
		
		if(value == 0) {
			unsigned int open = 0;

			//Search for ending bracket
			for(std::size_t i = instPtr + 1; i < program.program.size(); i++) {
				if(program.program[i] == ']')
					if(open == 0)
						return i;
					else
						open--;
				else if(program.program[i] == '[')
					open++;
			}

			//Couldn't find ending bracket
			success = false;

			return 1;
		} else {
			//Indicates that this bracket should be pushed onto the jump table
			return 0; 
		}
	}

	bool BrainfInterpreter::stepUnprocessed() {
		if(m_instPtr > m_program.program.size()) {
			m_error = "Execution gone past the end of the program instructions";
			return false;
		}

		char inst = m_program.program[m_instPtr];
		std::size_t jumpValue;
		bool success;

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
				jumpValue = handleStartLoop(m_memory[m_dataPtr], m_program, m_instPtr, success);

				if(jumpValue == 0) {
					m_jumpTable.push_back(m_instPtr);	
				} else if(success) {
					m_instPtr = jumpValue;
				} else {
					m_error = "No matching bracket ] for instruction '";
					m_error	+= inst;
					m_error	+= "' at character ";
					m_error += std::to_string(m_instPtr + 1);
					return false;
				}
			break;
			case END_LOOP : 
				if(m_jumpTable.empty()) {
					m_error = "No matching bracket [ for instruction '";
					m_error += inst;
					m_error += "' at character ";
					m_error += std::to_string(m_instPtr + 1);
					return false;
				}

				if(m_memory[m_dataPtr] != 0) {
					m_instPtr = m_jumpTable.back();
				} else {
					m_jumpTable.pop_back();
				}
			break;
			case INPUT : m_memory[m_dataPtr] = getChar();
			break;
			case OUTPUT : m_stream << m_memory[m_dataPtr] << std::flush;
			break;
		}

		//Check for out-of-bounds access
		std::string error = checkMemoryError(m_memory, inst, m_instPtr);
		if(error != "") {
			m_error = error;
			return false;
		}

		m_instPtr++;

		return true;
	}

	/**
	* This steps the execution a single step.  
	*
	* @return True if the instruction was executed successfully.
	*/
	bool BrainfInterpreter::step() {
		if(m_program.processed)
			return stepProcessed();
		else
			return stepUnprocessed();
	}

	/**
	* This is the run function, which run speed can be adjusted and is
	* to go as fast as set but it will try. Also zero means as fast as possible.
	*
	* @param runSpeed The speed to run at in Instructions per second
	*
	* @return True if the program executed successfully.
	*/
	bool BrainfInterpreter::run(float runSpeed) {
		if(runSpeed < 0)
			runSpeed = 0; //Just set zero for anything negative
		
		bool regulate = runSpeed != 0;

		if(!regulate) {
			if(m_program.processed) {
				while(m_instPtr < m_program.tokens.size())
					if(!stepProcessed()) return false;
			} else {
				while(m_instPtr < m_program.program.size())
					if(!stepUnprocessed()) return false;
			}
		} else {
			//Initialize variables for timing
			int milliPerInst = regulate ? 1 / runSpeed : 0;
			std::chrono::milliseconds delta;
			auto execTime = std::chrono::milliseconds(milliPerInst);
			auto currentTime = std::chrono::steady_clock::now();
			auto lastTime = currentTime;

			if(m_program.processed) {
				while(m_instPtr < m_program.tokens.size()) {
					currentTime = std::chrono::steady_clock::now();
					delta = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
					lastTime = currentTime;

					//Should the execution be stopped for a bit to stay in time
					if(delta > execTime) {
						std::this_thread::sleep_for(delta - execTime);
					}

					if(!stepProcessed()) return false;
				}
			} else {
				while(m_instPtr < m_program.program.size()) {
					currentTime = std::chrono::steady_clock::now();
					delta = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
					lastTime = currentTime;

					//Should the execution be stopped for a bit to stay in time
					if(delta > execTime) {
						std::this_thread::sleep_for(delta - execTime);
					}

					if(!stepUnprocessed()) return false;
				}
			}
		}

		return true;
	}

	/**
	* This method is responsible for putting certain data into tokens
	* and validation check, for [ and ] it can put jump information.
	*
	* @return Whether or not m_program has valid syntax.
	*/
	bool BrainfInterpreter::expr() {
		std::deque<unsigned int> openLoops;

		for(std::size_t i = 0; i < m_program.tokens.size(); i++) {
			if(m_program[i] == '[') {
				openLoops.push_back(i);
			} else if(m_program[i] == ']') {
				if(openLoops.empty()) {
					m_error = "Too many ']' for open loops '['";
					return false;
				}

				//Assign jump back address
				m_program.tokens[i].data = openLoops.back();

				//Assign jump forward address
				m_program.tokens[openLoops.back()].data = i;

				openLoops.pop_back();
			}
		}

		if(!openLoops.empty())
			m_error = "Too many '[' for close loops ']'";

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
	* This method will do the optimization like folding repetitive
	* instructions into one and other creative things I can find
	* or think of, without modifying the behavior.
	*/
	void BrainfInterpreter::preProcess(unsigned int optimization) {
		std::vector<Token> newTokens;
		std::size_t i = 0;

		//Remove all characters but <>-+,.[]
		for(std::size_t j = 0; j < m_program.tokens.size();) {
			char c = m_program[j];

			if(c != SHIFT_LEFT && c != SHIFT_RIGHT && c != INCREMENT && c != DECREMENT &&
			   c != START_LOOP && c != END_LOOP    && c != INPUT     && c != OUTPUT) {
				m_program.tokens.erase(m_program.tokens.begin() + j);
			} else {
				j++;
			}
		}

		if(optimization < 1)
			return;

		//First Pass
		while(i < m_program.tokens.size()) {
			char current = m_program.tokens[i].identifier;
			char previous = i > 0 ? m_program.tokens[i - 1].identifier : 0;
			char next = i < m_program.tokens.size() - 1 ? m_program.tokens[i + 1].identifier : 0;

			if(current == SHIFT_LEFT || current == SHIFT_RIGHT ||
			   current == INCREMENT  || current == DECREMENT) {
				//Counts instructions and uses that as data for a single
				//instruction, like run length encoding

				unsigned int sum = 1;
				char temp = next;

				while(temp == current) {
					sum++;
					temp = m_program.tokens[i + sum].identifier;
				}

				newTokens.push_back(Token{current, sum});

				i += sum;
			} else if(current == START_LOOP) {
				
				bool special = false; //Determines if a normal loop token should be pushed

				//Check for very beginning of program
				//These are usually for comments
				if(i == 0) {
					int count = 1;

					while(m_program.tokens[i + count].identifier != END_LOOP) {
						count++;
					}

					i += count + 1;

					special = true;
				} else if(next == INCREMENT) {
					//Checks for the clear instruction
					if(m_program.tokens[i + 2].identifier == END_LOOP) {
						newTokens.push_back(Token{CLEAR, 1});
						i += 3;

						special = true;
					}
				} else if(next == DECREMENT) {
					//Is it another clear instruction or is it a copy
					if(m_program.tokens[i + 2].identifier == END_LOOP) {
						newTokens.push_back(Token{CLEAR, 1});
						i += 3;

						special = true;
					} else {
						//Check for copy instruction
						std::string expected = ">+>+<<]"; //Sequence is [->+>+<<]
						std::string actual;
                                                                                                                        
						for(std::size_t j = 0; j < 7; j++) {
							actual += m_program.tokens[i + j + 2].identifier;
						}	
						
						if(actual == expected) {
							newTokens.push_back(Token{COPY, 1});
							i += 9;

							special = true;
						}
					}
				} else if(previous == END_LOOP) {
					//Gets rid of loops that occur right after another loop
					char temp = next;
					int count = 1;

					while(temp != END_LOOP) {
						count++;
						temp = m_program.tokens[i + count].identifier;
					}

					i += count;

					special = true;
				} 
				
				//Nothing special just a start loop
				if(!special) {
					newTokens.push_back(Token{current, 1});
					i++;
				}
			} else {
				//These are just whatever
				newTokens.push_back(Token{current, 1});
				i++;
			}
		}

		//Replace the token lists in m_program
		m_program.tokens.swap(newTokens);
		newTokens.clear();

		i = 0;


		if(optimization < 2)
			return;

		//Second Pass
		while(i < m_program.tokens.size()) {
			char current = m_program.tokens[i].identifier;
			char next = i < m_program.tokens.size() - 1 ? m_program.tokens[i + 1].identifier : 0;

			//Optimize for opposing operators +- ><
			if(current == SHIFT_LEFT || current == SHIFT_RIGHT ||
			   current == INCREMENT  || current == DECREMENT) {
				
				if(isOpposing(current, next)) {
					if(m_program.tokens[i].data == m_program.tokens[i + 1].data) {
						//Remove both
						m_program.tokens.erase(m_program.tokens.begin() + i);
						m_program.tokens.erase(m_program.tokens.begin() + i);
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
				} else {
					i++;
				}
			} else {
				i++;
			}
		}
	}

}
