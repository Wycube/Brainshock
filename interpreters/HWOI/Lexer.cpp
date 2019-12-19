#include "Lexer.hpp"
#include <vector>
#include <iostream>

namespace bs {

	namespace frick {

		/**
		 * Gets the description of the character specified, if it's a 
		 * valid token.
		 *
		 * @param character The char to check a description for
		 *
		 * @return A const char* of the description.
		 */
		const char* getDesc(char character) {
			switch(character) {
					case RSHIFT : return "Shifts the data pointer 1 to the right";
					break;
					case LSHIFT : return "Shifts the data pointer 1 to the left";
					break;
					case INCREMENT : return "Increments the memory cell at the data pointer";
					break;
					case DECREMENT : return "Decrements the memory cell at the data pointer";
					break;
					case OPENLOOP : return "Starts a loop, if current cell is 0 skip to close bracket, else continue";
					break;
					case CLOSELOOP : return "Ends a loop, if current cell is 0 continue past, if not jump to open loop";
					break;
					case OUTPUT : return "Outputs the current memory cell as an ASCII character";
					break;
					case INPUT : return "Set the current memory cell to a char from input";
					break;
					case DEBUG : return "Enables/Disables the debug info";
					break;
					default: return "";
					break;
			}
		}

		/* StdLexer Implementation */

		/**
		 * Converts a string of characters that represent a
		 * Brainf*ck program to an array of Operations in
		 * the standard Brainf version
		 *
		 * @param program The program class to be loaded with the operations
		 * @param instructions The string of instructions in a c-string
		 */
		void StdLexer::tokenize(Program& program, const char *instructions) {
			int ptr = 0;
			char current = 1;

			while(current != 0) {
				//step through the instructions and tokenize
				current = instructions[ptr++];
				
				switch(current) {
					case RSHIFT : program.m_tokens.push_back(Operation(current, getDesc(current), 1));
					break;
					case LSHIFT : program.m_tokens.push_back(Operation(current, getDesc(current), 1));
					break;
					case INCREMENT : program.m_tokens.push_back(Operation(current, getDesc(current), 1));
					break;
					case DECREMENT : program.m_tokens.push_back(Operation(current, getDesc(current), 1));
					break;
					case OPENLOOP : program.m_tokens.push_back(Operation(current, getDesc(current), 1));
					break;
					case CLOSELOOP : program.m_tokens.push_back(Operation(current, getDesc(current), 1));
					break;
					case OUTPUT : program.m_tokens.push_back(Operation(current, getDesc(current), 1));
					break;
					case INPUT : program.m_tokens.push_back(Operation(current, getDesc(current), 1));
					break;
					case DEBUG : program.m_tokens.push_back(Operation(current, getDesc(current), 1));
					break;
					default: //Not one of the tokens, just skip
					break;
				}
			}
		}

		/**
		 * Checks the input program for errors in OPENLOOP and
		 * and CLOSELOOP operation placement and makes sure
		 * they are in pairs, then sets their jump value
		 * (ie data variable) accordingly.
		 *
		 * @param program The program to Check
		 *
		 * @return True if the programs loops are correct, false otherwise
		 */
		bool StdLexer::expr(Program& program) {
			std::vector<std::pair<char, int>> loops;

			//Find indices of loops
			for(int i = 0; i < program.m_tokens.size(); i++) {
				if(program[i].m_symbol == OPENLOOP)
					loops.emplace_back(OPENLOOP, i);
				else if(program[i].m_symbol == CLOSELOOP)
					loops.emplace_back(CLOSELOOP, i);
			}

			//Check if they are complete
			for(int i = 0; i < loops.size(); i++) {
				if(loops[i].first == OPENLOOP) {
					int index = loops[i].second + 1; //location after openloop to avoid counting twice
					int open = 1;

					while(index <= program.m_tokens.size()) {
						if(program[index].m_symbol == OPENLOOP)
							open++;
						else if(program[index].m_symbol == CLOSELOOP)
							open--;

						if(open == 0) {
							program[loops[i].second].m_data = index; //Jump value
							break;
						}

						index++;
					}

					if(open != 0) {
						return false;
					}

				} else if(loops[i].first == CLOSELOOP) {
					int index = loops[i].second - 1; //location before closeloop to avoid counting twice
					int close = 1;

					while(index >= 0) {
						if(program[index].m_symbol == CLOSELOOP)
							close++;
						else if(program[index].m_symbol == OPENLOOP)
							close--;

						if(close == 0) {
							program[loops[i].second].m_data = index; //Jump value
							break;
						}

						index--;
					}

					if(close != 0) {
						return false;
					}
				}
			}

			return true;
		}

		/**
		 * Optimizes an array of operations in order to have the
		 * same behavior with less operations
		 * ex. removes +- or ><, >><<
		 *	    turns ++- into + or >>><< into >
		 *     removes [] at the beginning or
		 *     immediatly after another loop etc 
		 *
		 * @param program The program to optimize
		 */ 
		void StdLexer::optimize(Program& program) {
			
		}

		/* EffLexer Implemenetation */

		/**
		 * Converts a string of characters that represent a
		 * Brainf*ck program to an array of Operations and 
		 * condenses multiple sequential operations into one.
		 *
		 * @param program The program class to be loaded with the operations
		 * @param instructions The string of instructions in a c-string
		 */
		void EffLexer::tokenize(Program& program, const char *instructions) {
			int ptr = 0;
			int count = 0;
			char current = 1;
			char last;	
			bool isToken = false;
			bool isRepeatable = false;
			bool isZero = false;

			while(current != 0) {
				//step through the instructions and tokenize
				current = instructions[ptr++];
				
				switch(current) {
					case RSHIFT : isToken = true; isRepeatable = true;//program.m_tokens.push_back(Operation(RSHIFT, "Shifts the data pointer 1 to the right", 1));
					break;
					case LSHIFT : isToken = true; isRepeatable = true;//program.m_tokens.push_back(Operation(LSHIFT, "Shifts the data pointer 1 to the left", 1));
					break;
					case INCREMENT : isToken = true; isRepeatable = true;//program.m_tokens.push_back(Operation(INCREMENT, "Increments the memory cell at the data pointer", 1));
					break;
					case DECREMENT : isToken = true; isRepeatable = true;//program.m_tokens.push_back(Operation(DECREMENT, "Decrements the memory cell at the data pointer", 1));
					break;
					case OPENLOOP : isToken = true; isRepeatable = false;//program.m_tokens.push_back(Operation(OPENLOOP, "Starts a loop, if current cell is 0 skip to close bracket, else continue"));
					break;
					case CLOSELOOP : isToken = true; isRepeatable = false;//program.m_tokens.push_back(Operation(CLOSELOOP, "Ends a loop, if current cell is 0 continue past, if not jump to open loop"));
					break;
					case OUTPUT : isToken = true; isRepeatable = false;//program.m_tokens.push_back(Operation(OUTPUT, "Outputs the current memory cell as an ASCII character"));
					break;
					case INPUT : isToken = true; isRepeatable = false;//program.m_tokens.push_back(Operation(INPUT, "Set the current memory cell to a char from input"));
					break;
					case DEBUG : isToken = true; isRepeatable = false;//program.m_tokens.push_back(Operation(DEBUG, "Enables/Disables the debug info"));
					break;
					default: isToken = false; isRepeatable = false;//Not one of the tokens, just skip
					break;
				}

				if(count == 0) {
					isZero = true;
				} else {
					isZero = false;
				}

				if(isToken) {
					if(isRepeatable) {
						if(!isZero) {
							if(current == last) {
								count++;
								continue;
							} else {
								program.m_tokens.push_back(Operation(last, getDesc(last), count));
								count = 1;
							}
						} else {
							count++;
						}
					} else {
						if(!isZero) {
							program.m_tokens.push_back(Operation(last, getDesc(last), count));
							count = 0;
						}

						program.m_tokens.push_back(Operation(current, getDesc(current), 1));
					}

				} else {
					if(!isZero) {
						program.m_tokens.push_back(Operation(last, getDesc(last), count));
						count = 0;
					}
				}

				last = current;				
			}
		}

		/**
		 * Checks the input program for errors in OPENLOOP and
		 * and CLOSELOOP operation placement and makes sure
		 * they are in pairs, then sets their jump value
		 * (ie data variable) accordingly.
		 *
		 * @param program The program to Check
		 *
		 * @return True if the programs loops are correct, false otherwise
		 */
		bool EffLexer::expr(Program& program) {
			std::vector<std::pair<char, int>> loops;

			//Find indices of loops
			for(int i = 0; i < program.m_tokens.size(); i++) {
				if(program[i].m_symbol == OPENLOOP)
					loops.emplace_back(OPENLOOP, i);
				else if(program[i].m_symbol == CLOSELOOP)
					loops.emplace_back(CLOSELOOP, i);
			}

			//Check if they are complete
			for(int i = 0; i < loops.size(); i++) {
				if(loops[i].first == OPENLOOP) {
					int index = loops[i].second + 1; //location after openloop to avoid counting twice
					int open = 1;

					while(index <= program.m_tokens.size()) {
						if(program[index].m_symbol == OPENLOOP)
							open++;
						else if(program[index].m_symbol == CLOSELOOP)
							open--;

						if(open == 0) {
							program[loops[i].second].m_data = index; //Jump value
							break;
						}

						index++;
					}

					if(open != 0) {
						return false;
					}

				} else if(loops[i].first == CLOSELOOP) {
					int index = loops[i].second - 1; //location before closeloop to avoid counting twice
					int close = 1;

					while(index >= 0) {
						if(program[index].m_symbol == CLOSELOOP)
							close++;
						else if(program[index].m_symbol == OPENLOOP)
							close--;

						if(close == 0) {
							program[loops[i].second].m_data = index; //Jump value
							break;
						}

						index--;
					}

					if(close != 0) {
						return false;
					}
				}
			}

			return true;
		}

		/**
		 * Optimizes an array of operations in order to have the
		 * same behavior with less operations
		 * ex. removes +- or ><, >><<
		 *	    turns ++- into + or >>><< into >
		 *     removes [] at the beginning or
		 *     immediatly after another loop etc 
		 *
		 * @param program The program to optimize
		 */ 
		void EffLexer::optimize(Program& program) {
			
		}

	}
}