#include "Lexer.hpp"
#include <vector>
#include <iostream>

namespace bs {

	namespace frick {

		/* StdLexer Implementation */

		/**
		 * Converts a string of characters that represent a
		 * Brainf*ck program to an array of Operations in
		 * the standard Brainf version
		 *
		 * @param program The program to be tokenized
		 */
		void StdLexer::tokenize(Program& program, const char *string) {
			int ptr = 0;
			char current = 1;

			while(current != 0) {
				//step through string and tokenize
				current = string[ptr++];
				
				switch(current) {
					case RSHIFT : program.m_tokens.push_back(Operation(RSHIFT, "Shifts the data pointer 1 to the right"));
					break;
					case LSHIFT : program.m_tokens.push_back(Operation(LSHIFT, "Shifts the data pointer 1 to the left"));
					break;
					case INCREMENT : program.m_tokens.push_back(Operation(INCREMENT, "Increments the memory cell at the data pointer"));
					break;
					case DECREMENT : program.m_tokens.push_back(Operation(DECREMENT, "Decrements the memory cell at the data pointer"));
					break;
					case OPENLOOP : program.m_tokens.push_back(Operation(OPENLOOP, "Starts a loop, if current cell is 0 skip to close bracket, else continue"));
					break;
					case CLOSELOOP : program.m_tokens.push_back(Operation(CLOSELOOP, "Ends a loop, if current cell is 0 continue past, if not jump to open loop"));
					break;
					case OUTPUT : program.m_tokens.push_back(Operation(OUTPUT, "Outputs the current memory cell as an ASCII character"));
					break;
					case INPUT : program.m_tokens.push_back(Operation(INPUT, "Set the current memory cell to a char from input"));
					break;
					case DEBUG : program.m_tokens.push_back(Operation(DEBUG, "Enables/Disables the debug info"));
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

					while(index < program.m_tokens.size()) {
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

					if(open != 0)
						return false;

				} else if(loops[i].first == CLOSELOOP) {
					int index = loops[i].second - 1; //location before closeloop to avoid counting twice
					int close = 1;

					while(index > 0) {
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

					if(close != 0)
						return false;
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

	}
}