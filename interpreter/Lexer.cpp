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
	
			return Sequence(nullptr, 0);
		}

		/*Checks expressions and verifies that there is no
		* syntax errors, aka open loops without the close
		* loop counterpart, or vise versa
		*/
		Sequence StdLexer::expr(Sequence seq) {
			return Sequence(nullptr, 0);
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