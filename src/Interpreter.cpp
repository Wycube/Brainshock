#include "Interpreter.hpp"

#include <chrono>
#include <thread>

namespace bs {

	//--------------- Helper Functions ----------------//

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

	size_t handleStartLoop(unsigned char value, Program &program, std::size_t instPtr, bool &success) {
		success = true;
		
		if(value == 0) {
			unsigned int open = 0;

			//Search for ending bracket
			for(std::size_t i = instPtr + 1; i < program.source.size(); i++) {
				if(program.source[i] == ']')
					if(open == 0)
						return i;
					else
						open--;
				else if(program.source[i] == '[')
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


	//--------------- Interpreter Methods and Constructors ---------------//

	Interpreter::Interpreter(std::ostream &stream, bool numInput, std::size_t memSize) : m_stream(stream), m_numInput(numInput), m_instPtr(0), m_dataPtr(0) {
		m_memory = Tape(memSize);
	}

	Interpreter::~Interpreter() { }

	char Interpreter::getChar() {
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

		//-n should the input be converted, if possible
		if(m_numInput) {
			if(std::isdigit(temp)) {
				std::string num;
				num += temp;
				temp = m_inBuffer.back();
				
				//Keep getting digits as long as it is under 256
				while(std::isdigit(temp) && !m_inBuffer.empty()) {
					if(std::stoi(num + temp) < 256) {
						num += temp;
						m_inBuffer.pop_back();
					} else break;
					temp = m_inBuffer.back();
				}
				
				temp = static_cast<char>(std::stoi(num));
			}
		}

		return temp;
	}


	//--------------- BasicInterpreter Implementation ---------------//

	BasicInterpreter::BasicInterpreter(std::ostream &stream, bool numInput, std::size_t memSize) : Interpreter(stream, numInput, memSize) { }
	
	BasicInterpreter::~BasicInterpreter() { }

	bool BasicInterpreter::loadProgram(const char *program, bool process, bool resetDataPtr, unsigned int optimization) {
		m_emitter.loadSource(program);

		m_instPtr = 0;

		if(resetDataPtr)
			m_dataPtr = 0;

		if(process) {
			m_emitter.tokenize();

			if(!m_emitter.expr()) { m_error = m_emitter.getError(); return false; } //Program has invalid syntax

			if(optimization > 0) {
				m_emitter.optimize(optimization);

				//This is called to update the jump locations of the brackets
				if(!m_emitter.expr()) { m_error = m_emitter.getError(); return false; } //If there was an error in the optimizing, which shouldn't happen
			}
		}

		m_program = m_emitter.emit();

		return true;
	}

	bool BasicInterpreter::stepProcessed() {
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
				    m_memory[m_dataPtr] = 0;
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

	bool BasicInterpreter::stepUnprocessed() {
		if(m_instPtr > m_program.source.size()) {
			m_error = "Execution gone past the end of the program instructions";
			return false;
		}

		char inst = m_program.source[m_instPtr];
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
	bool BasicInterpreter::step() {
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
	bool BasicInterpreter::run(float runSpeed) {
		if(runSpeed < 0)
			runSpeed = 0; //Just set zero for anything negative
		
		bool regulate = runSpeed != 0;

		if(!regulate) {
			if(m_program.processed) {
				while(m_instPtr < m_program.tokens.size())
					if(!stepProcessed()) return false;
			} else {
				while(m_instPtr < m_program.source.size()) {
					if(!stepUnprocessed()) return false;
				}
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
				while(m_instPtr < m_program.source.size()) {
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

}
