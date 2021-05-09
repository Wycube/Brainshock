#include "Program.hpp"

namespace bs {

	/**
	 * Takes the program string in the program and iterates
	 * through it turning all the characters into Tokens, these
	 * Tokens don't have any meaning until they reach the interpreter.
	 * Note that the method clears the tokens vector before starting.
	 */
	void Program::tokenize() {
		tokens.clear();

		for(size_t i = 0; i < source.length(); i++) {
			tokens.push_back(Token{source[i], 1});
		}

		processed = true;
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
	 * Takes in source code, then optionally optimizes it. And emits it as 
	 * a Program class with the sort-of IR.
	 */
	IREmitter::IREmitter(const char *source) {
		m_source.source = source;
	}

	/*
	 * So the default constructor can be used,
	 */
	void IREmitter::loadSource(const char *source) {
		m_source.source = source;
	}

	/**
	* This method will do the optimization like folding repetitive
	* instructions into one and other creative things I can find
	* or think of, without modifying the behavior.
	* TODO: Split this method up
	*/
	void IREmitter::optimize(unsigned int level) {
		std::vector<Token> newTokens;
		std::size_t i = 0;

		//Remove all characters but <>-+,.[]
		for(std::size_t j = 0; j < m_source.tokens.size();) {
			char c = m_source[j];

			if(c != SHIFT_LEFT && c != SHIFT_RIGHT && c != INCREMENT && c != DECREMENT &&
			   c != START_LOOP && c != END_LOOP    && c != INPUT     && c != OUTPUT) {
				m_source.tokens.erase(m_source.tokens.begin() + j);
			} else {
				j++;
			}
		}

		if(level < 1)
			return;

		//First Pass
		while(i < m_source.tokens.size()) {
			char current = m_source.tokens[i].identifier;
			char previous = i > 0 ? m_source.tokens[i - 1].identifier : 0;
			char next = i < m_source.tokens.size() - 1 ? m_source.tokens[i + 1].identifier : 0;

			if(current == SHIFT_LEFT || current == SHIFT_RIGHT ||
			   current == INCREMENT  || current == DECREMENT) {
				//Counts instructions and uses that as data for a single
				//instruction, like run length encoding

				unsigned int sum = 1;
				char temp = next;

				while(temp == current && (sum + i) < m_source.tokens.size() - 1) {
					sum++;
					temp = m_source.tokens[i + sum].identifier;
				}

				newTokens.push_back(Token{current, sum});

				i += sum;
			} else if(current == START_LOOP) {
				
				bool special = false; //Determines if a normal loop token should be pushed

				//Check for very beginning of program
				//These are usually for comments
				if(i == 0) {
					int count = 1;

					while(m_source.tokens[i + count].identifier != END_LOOP) {
						count++;
					}

					i += count + 1;

					special = true;
				} else if(next == INCREMENT) {
					//Checks for the clear instruction
					if(m_source.tokens[i + 2].identifier == END_LOOP) {
						newTokens.push_back(Token{CLEAR, 1});
						i += 3;

						special = true;
					}
				} else if(next == DECREMENT) {
					//Is it another clear instruction or is it a copy
					if(m_source.tokens[i + 2].identifier == END_LOOP) {
						newTokens.push_back(Token{CLEAR, 1});
						i += 3;

						special = true;
					} else {
						//Check for copy instruction, should be moved into the second pass later
						std::string expected = ">+>+<<]"; //Sequence is [->+>+<<]
						std::string actual;
                                                                                                                        
						for(std::size_t j = 0; j < 7; j++) {
							actual += m_source.tokens[i + j + 2].identifier;
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
						temp = m_source.tokens[i + count].identifier;
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
		m_source.tokens.swap(newTokens);
		newTokens.clear();

		i = 0;


		if(level < 2)
			return;

		//Second Pass
		while(i < m_source.tokens.size()) {
			char current = m_source.tokens[i].identifier;
			char next = i < m_source.tokens.size() - 1 ? m_source.tokens[i + 1].identifier : 0;

			//Optimize for opposing operators +- ><
			if(current == SHIFT_LEFT || current == SHIFT_RIGHT ||
			   current == INCREMENT  || current == DECREMENT) {
				
				if(isOpposing(current, next)) {
					if(m_source.tokens[i].data == m_source.tokens[i + 1].data) {
						//Remove both
						m_source.tokens.erase(m_source.tokens.begin() + i);
						m_source.tokens.erase(m_source.tokens.begin() + i);
					} else if(m_source.tokens[i].data > m_source.tokens[i + 1].data) {
						//Take away the seconds' data, from the firsts', and then remove it
						m_source.tokens[i].data -= m_source.tokens[i + 1].data;
						m_source.tokens.erase(m_source.tokens.begin() + i + 1);
						i++;
					} else {
						//Take away the firsts' data, from the seconds', then remove it
						m_source.tokens[i + 1].data -= m_source.tokens[i].data;
						m_source.tokens.erase(m_source.tokens.begin() + i);
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

	/**
	* This method is responsible for putting certain data into tokens
	* and validation check, for [ and ] it can put jump information.
	*
	* @return Whether or not m_program has valid syntax.
	*/
	bool IREmitter::expr() {
		std::deque<unsigned int> openLoops;

		for(std::size_t i = 0; i < m_source.tokens.size(); i++) {
			if(m_source[i] == '[') {
				openLoops.push_back(i);
			} else if(m_source[i] == ']') {
				if(openLoops.empty()) {
					m_error = "Too many ']' for open loops '['";
					return false;
				}

				//Assign jump back address
				m_source.tokens[i].data = openLoops.back();

				//Assign jump forward address
				m_source.tokens[openLoops.back()].data = i;

				openLoops.pop_back();
			}
		}

		if(!openLoops.empty())
			m_error = "Too many '[' for closed loops ']'";

		return openLoops.empty();
	}

	//Just does the tokenize method
	void IREmitter::tokenize() {
		m_source.tokenize();
	}

	Program IREmitter::emit() {
		return m_source;
	}

}