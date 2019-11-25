#include "Interpreter.h"
#include <iostream>
#include <string>

namespace bs {
	//Tape Class
	Tape::Tape(int length) : m_length(length) {
		m_cells = new unsigned char[length] {0};
		m_cellPtr = 0;
	}

	Tape::~Tape() {
		delete[] m_cells;
	}

	std::string formatChar(unsigned char num) {
		if(num < 10) {
			return std::string("  ") + char(num + 48);
		} else if(num < 100) {
			char first = char(num / 10);
			char second = num - first * 10;

			return std::string(" ") + char(first + 48) + char(second + 48);
		} else {
			char first = char(num / 100);
			char second = char((num - first * 100) / 10);
			char third = num - (first * 100 + second * 10);

			return std::string() + char(first + 48) + char(second + 48) + char(third + 48);
		}
	}

	//Print formatted like:
	//ex.                 V
	//        23  24  25  26  27  28  29  30
	//   ...|  0| 10|120|  4| 12| 11|123|255|... 
	void Tape::printTape() {

		std::string numLA = "";
		std::string numLB = ""; 
		std::string numLC = "";
		std::string numLD = "";

		if(m_cellPtr > 3)
			numLA = "|" + formatChar(m_cells[m_cellPtr - 4]);
		if(m_cellPtr > 2)
			numLB = "|" + formatChar(m_cells[m_cellPtr - 3]);
		if(m_cellPtr > 1)
			numLC = "|" + formatChar(m_cells[m_cellPtr - 2]);
		if(m_cellPtr > 0)
			numLD = "|" + formatChar(m_cells[m_cellPtr - 1]);

		std::string numM = "|" + formatChar(m_cells[m_cellPtr]);
		std::string numRA = "|" + formatChar(m_cells[m_cellPtr + 1]);
		std::string numRB = "|" + formatChar(m_cells[m_cellPtr + 2]);
		std::string numRC = "|" + formatChar(m_cells[m_cellPtr + 3]);
		std::string numRD = "|" + formatChar(m_cells[m_cellPtr + 4]);
		
		std::string row = numLA + numLB + numLC + numLD + numM + numRA + numRB + numRC + numRD + "|";

		int pos = numLA.length() + numLB.length() + numLC.length() + numLD.length() + int(numM.length() / 2);

		if(m_cellPtr > 5) {
			pos += 3;
		}

		std::cout << std::endl;

		for(int i = 0; i < pos; i++)
			std::cout << ' ';
		std::cout << m_cellPtr << std::endl;

		for(int i = 0; i < pos; i++)
			std::cout << ' ';
		std::cout << 'v' << std::endl;

		if(m_cellPtr > 5) {
			std::cout << "...";
		}

		std::cout << row << "..." << std::endl;
	}

	void Tape::setCell(unsigned char value) {
		m_cells[m_cellPtr] = value;
	}

	char Tape::getCell() {
		return m_cells[m_cellPtr];
	}

	//Program Class
	Program::Program(const char* string) {
		m_program = string;
		m_length = std::string(string).length();
		m_instPtr = 0;
	}

	bool Program::jump(int instruction) {
		if(instruction < 0 || instruction >= m_length) {
			return false;
		} else {
			m_instPtr = instruction;
			return true;
		}
	}

	bool Program::hasNext() {
		if(!(m_instPtr < m_length))
			return false;
		
		return true;
	}

	char Program::getInst() {
		return m_program[m_instPtr];
	}

	int Program::getPtr() {
		return m_instPtr;
	}

	//Interpreter Class
	Interpreter::Interpreter(const char* string, int memLength): m_program(string), m_tape(memLength) {}

	bool Interpreter::step(int numSteps) {
		if(numSteps < 0)
			return false;

		char *desc = "";

		for(int i = 0; i < numSteps; i++) {
			if(!m_program.hasNext()) {
				std::cout << "Could not complete " << i << " steps";
				return false;
			}

			//Check instruction TODO: In the future get from lexical parser
			switch(m_program.getInst()) {
				case '>' : m_tape.incPtr(); desc = "Shifts the data pointer 1 to the right";
				break; //Shift Right
				case '<' : m_tape.decPtr(); desc = "Shifts the data pointer 1 to the left";
				break; //Shift Left
				case '+' : m_tape.setCell(m_tape.getCell() + 1); desc = "Increments the memory cell at the data pointer";
				break; //Increment
				case '-' : m_tape.setCell(m_tape.getCell() - 1); desc = "Decrements the memory cell at the data pointer";
				break; //Decrement
				case '[' : m_jmp = m_program.getPtr(); desc = "Starts a loop, if current cell is 0 skip to close bracket, else continue";
				break; //Start Loop
				case ']' : 
					if(m_tape.getCell() != 0) m_program.jump(m_jmp); desc = "Ends a loop, if current cell is 0 continue past, if not jump to open loop";
				break; //End Loop
				case '#' : m_debug = !m_debug; desc = "Enables/Disables the boolean value that outputs debug info";
				break; //Enable/Disable Debug
			}

			//Debug Printing
			if(m_debug) {
				std::cout << "cell:" << std::to_string(m_tape.m_cellPtr) << " inst:" << m_program.getInst() << " | " << desc << std::endl;
			}

			m_program.incPtr();
		}

		return true;
	}

	bool Interpreter::run() {
		while(m_program.hasNext()) {
			step();
		}

		m_tape.printTape();

		return true;
	}
}