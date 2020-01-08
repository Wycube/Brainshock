#include "Interpreter.hpp"

namespace bs {

	InterpFlags iflags;

	/**
	* Manages the input buffer and is used for the INPUT
	* operation. Pops and adds values to the queue,
	* it can add many chars at once. 
	*
	* @return A char from the input buffer
	*/
	unsigned char Interpreter::getInput() {
			unsigned char temp;

			if(m_inBuffer.empty()) {
				std::string input;
				
				std::cin >> input;

				for(int i = 0; i < input.length(); i++) {
					m_inBuffer.push(input[i]);
				}
			}

			temp = m_inBuffer.front();
			m_inBuffer.pop();

			//Checking and accounting for number input
			if(iflags.numInput) {
				if(47 < temp && temp < 59)
					temp -= 48;
			}

			return temp;
	}

}