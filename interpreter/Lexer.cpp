#include "Lexer.h"
#include <vector>
#include <iostream>

namespace bs {

	//The standard Brainf*** implementation
	namespace frick {
		/*Converts a string of characters that represent a
		* Brainf*ck program to an array of Tokens only in
		* the standard Brainf*ck dialect
		*/
		Sequence StdLexer::tokenize(const char *program) {
			int ptr = 0;
			char current;
			std::vector<StdToken> tokens;

			while(current != 0) {
				//step through string and tokenize
				current = *(program + ptr++);
				
				switch(current) {
					case RSHIFT : tokens.push_back(StdToken(RSHIFT, "Shifts the data pointer 1 to the right"));
					break;
					case LSHIFT : tokens.push_back(StdToken(LSHIFT, "Shifts the data pointer 1 to the left"));
					break;
					case INCREMENT : tokens.push_back(StdToken(INCREMENT, "Increments the memory cell at the data pointer"));
					break;
					case DECREMENT : tokens.push_back(StdToken(DECREMENT, "Decrements the memory cell at the data pointer"));
					break;
					case OPENLOOP : tokens.push_back(StdToken(OPENLOOP, "Starts a loop, if current cell is 0 skip to close bracket, else continue"));
					break;
					case CLOSELOOP : tokens.push_back(StdToken(CLOSELOOP, "Ends a loop, if current cell is 0 continue past, if not jump to open loop"));
					break;
					case OUTPUT : tokens.push_back(StdToken(INPUT, "Outputs the current memory cell as an ASCII character"));
					break;
					case INPUT : tokens.push_back(StdToken(OUTPUT, "Set the current memory cell to a char from input"));
					break;
					case DEBUG : tokens.push_back(StdToken(DEBUG, "Enables/Disables the boolean value that outputs debug info"));
					break;
					default: //Not one of the tokens, just skip
					break;
				}
			}

			StdToken *fin = new StdToken[tokens.size()];

			for(int i = 0; i < tokens.size(); i++)
				fin[i] = tokens[i];
	
			return Sequence(fin, tokens.size());
		}

		/*Checks expressions and verifies that there is no
		* syntax errors, aka open loops without the close
		* loop counterpart, or vise versa
		*/
		bool StdLexer::expr(Sequence seq) {
			std::vector<std::pair<char, int>> loops;

			//Find indices of loops
			for(int i = 0; i < seq.m_len; i++) {
				if(seq.m_seq[i].m_symbol == OPENLOOP)
					loops.emplace_back(OPENLOOP, i);
				else if(seq.m_seq[i].m_symbol == CLOSELOOP)
					loops.emplace_back(CLOSELOOP, i);
			}

			//Check if they are complete
			for(int i = 0; i < loops.size(); i++) {
				if(loops[i].first == OPENLOOP) {
					int i = loops[i].second; //location of openloop
					int open = 1;

					while(i < seq.m_len) {
						if(seq.m_seq[i].m_symbol == OPENLOOP)
							open++;
						else if(seq.m_seq[i].m_symbol == CLOSELOOP)
							open--;

						if(open == 0)
							break;

						i++;
					}

					if(open != 0)
						return false;

				} else if(loops[i].second == CLOSELOOP) {
					int i = loops[i].second; //location of closeloop
					int close = 1;

					while(i > 0) {
						if(seq.m_seq[i].m_symbol == CLOSELOOP)
							close++;
						else if(seq.m_seq[i].m_symbol == OPENLOOP)
							close--;

						if(close == 0)
							break;

						i--;
					}

					if(close != 0)
						return false;
				}
			}

			return true;
		}

		/*Optimizes an array of Tokens in order to have the
		* same behavior with less tokens
		* ex. removes +- or ><, >><<
		*	    turns ++- into + or >>><< into >
		*     removes [] at the beginning or
		*     immediatly after another loop etc
		*/ 
		Sequence StdLexer::optimize(Sequence seq) {
			//Do the optimizinging
			//This is the fun one to write
			return Sequence(nullptr, 0);
		}
	}
}