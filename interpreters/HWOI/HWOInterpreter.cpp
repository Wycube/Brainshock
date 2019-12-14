#include "HWOInterpreter.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

namespace bs {

	namespace frick {

		/** Standard Heavy-Weight Interpreter Implementation */

		/**
		 * Tokenizes and optionally optimizes the loaded program
		 * 
		 * @param program The proram that will result and be modified
		 * @param string The string that contains the Brainf program but hasn't been parsed
		 *
		 * @return True if the processing succeeded, false otherwise
		 */
		bool StdHWInterpreter::process(bs::Program& program, const char *string) {
			m_lexer.tokenize(program, string);

			if(!m_lexer.expr(program))
				return false;

			return true;
		}

		/**
		 * Manages the input buffer and is used for the INPUT
		 * operation. Pops and adds values to the queue,
		 * it can add many chars at once. 
		 *
		 * @return A char from the input buffer
		 */
		unsigned char StdHWInterpreter::getInput() {
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
			if(m_flags.numInput) {
				if(47 < temp && temp < 59)
					temp -= 48;
			}

			return temp;
		}

		/** 
		 * Steps the specified number of steps, and if it can't step
		 * all of them then it prints the number of steps it couldn't do 
		 *
		 * @param numSteps The number of operations to step forward
		 *
		 * @return true if able to step the full number of steps, false otherwise
		 */
		bool StdHWInterpreter::step(int numSteps) {
			int notDone = 0;

			for(int i = 0; i < numSteps; i++) {
				Operation current = m_program[m_instPtr];

				switch(current.m_symbol) {
					case RSHIFT : m_cellPtr++;
					break;
					case LSHIFT : m_cellPtr--;
					break;
					case INCREMENT : m_tape[m_cellPtr]++;
					break;
					case DECREMENT : m_tape.m_cells[m_cellPtr]--;
					break;
					case OPENLOOP : if(m_tape.m_cells[m_cellPtr] == 0) m_instPtr = current.m_data;
					break;
					case CLOSELOOP : if(!(m_tape.m_cells[m_cellPtr] == 0)) m_instPtr = current.m_data;
					break;
					case OUTPUT : std::cout << char(m_tape.m_cells[m_cellPtr]) << std::flush;
					break;
					case INPUT : m_tape[m_cellPtr] = getInput();
					break;
					case DEBUG : m_debug = !m_debug;
					break;
					default : notDone++;
					break;
				}

				m_instPtr++;

				if(m_debug) {
					std::cout << "Cell:" << m_cellPtr << " Inst:" << current.m_symbol << " " << current.m_desc << " Data:" << current.m_data << std::endl;
				}
			}

			if(notDone > 0) {
				std::cout << notDone << " Steps not done" << std::endl;
				return false;
			}
		
			return true;
		}

		/**
		 * Runs as fast as possible by stepping once in while loop
		 * until the end of the program is reached.
		 *
		 * @param ips Speed of execution in Instructions Per Second
		 */
		void StdHWInterpreter::run(float ips) {	
			if(ips < 0)
				return;

			if(ips == IPS_FAST) { //Run as fast as possible
				while(m_program.hasNext(m_instPtr)) {
					step();
				}
			} else { //Run varied speed
				std::chrono::steady_clock::time_point start;
				std::chrono::steady_clock::time_point end;
				std::chrono::duration<double, std::ratio<1, 1000>> elapsed;

				std::chrono::duration<double, std::ratio<1, 1000>> insTime(1 / double(ips) * 1000); //Instruction time in milliseconds

				while(m_program.hasNext(m_instPtr)) {
					start = std::chrono::steady_clock::now();

					step();

					end = std::chrono::steady_clock::now();

					elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1000>>>(end - start);

					if(elapsed.count() < insTime.count())
						std::this_thread::sleep_for(insTime - elapsed);
				}
			}
		}

	}
}