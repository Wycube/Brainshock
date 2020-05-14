#include "Memory.hpp"
#include <string>
#include <cstring>
#include <iostream>

namespace bs {

	/**
	 * Constructs an array of bytes of the specified size
	 * initialized with all zeroes.
	 */
	 Tape::Tape(std::size_t size) : m_size(size) {
		m_cells = new unsigned char[m_size];
		
		memset(m_cells, 0, size);
	 }

	/**
	 * Print the tape formatted like this.
	 *                    V
	 *                    26
	 *   ...|  0| 10|120|  4| 12| 11|123|255|... 
	 *
	 * @param cell Where to print in the tape, centered on that cell
	 */
	 /*
	void Tape::printTape(int cell) {

		std::string numLA = "";
		std::string numLB = ""; 
		std::string numLC = "";
		std::string numLD = "";

		if(cell > 3)
			numLA = "|" + formatChar(m_cells[cell - 4]);
		if(cell > 2)
			numLB = "|" + formatChar(m_cells[cell - 3]);
		if(cell > 1)
			numLC = "|" + formatChar(m_cells[cell - 2]);
		if(cell > 0)
			numLD = "|" + formatChar(m_cells[cell - 1]);

		std::string numM = "|" + formatChar(m_cells[cell]);
		std::string numRA = "|" + formatChar(m_cells[cell + 1]);
		std::string numRB = "|" + formatChar(m_cells[cell + 2]);
		std::string numRC = "|" + formatChar(m_cells[cell + 3]);
		std::string numRD = "|" + formatChar(m_cells[cell + 4]);
		
		std::string row = numLA + numLB + numLC + numLD + numM + numRA + numRB + numRC + numRD + "|";

		int pos = numLA.length() + numLB.length() + numLC.length() + numLD.length() + int(numM.length() / 2);

		if(cell > 5) {
			pos += 3;
		}

		std::cout << std::endl;

		for(int i = 0; i < pos; i++)
			std::cout << ' ';
		std::cout << cell << std::endl;

		for(int i = 0; i < pos; i++)
			std::cout << ' ';
		std::cout << 'v' << std::endl;

		if(cell > 5) {
			std::cout << "...";
		}

		std::cout << row << "..." << std::endl;
	}
	*/




		/**
	 * Formats a string so that the character has a certain
	 * number of spaces before it based on the number of digits.
	 * Used for the fPrint() function.
	 *
	 * @param num The byte for formatting
	 *
	 * @return A std::string with the spaces and number
	 */
	std::string formatChar(unsigned char num) {
		int intNum = static_cast<int>(num);

		if(num < 10) {
			return std::string("  ") + std::to_string(intNum);
		} else if(num < 100) {
			return std::string(" ") + std::to_string(intNum);
		} else {
			return std::string(std::to_string(intNum));
		}
	}

	/**
	 * Print the tape formatted like this.
	 *                    26
	 *                    v
	 *   ...|  0| 10|120|  4| 12| 11|123|... 
	 *
	 * @param cell Where to print in the tape, centered on that cell
	 */
	void Tape::fPrint(int cell) {
		int arrowPos = 0;
		int offSet = 0;
		bool beginEllipsis, endEllipsis;
		std::string finalString;

		//Used if cell is within the first 4 cells of the tape
		beginEllipsis = cell > 3;

		if(beginEllipsis) {
			offSet = 3;
			arrowPos += 4;
			finalString = "...|";
		} else {
			offSet = cell;
			arrowPos += 1;
			finalString += "|";
		}

		//Checking if cell is within the last 4 cells
		endEllipsis = cell < m_size - 4;
		offSet = endEllipsis ? offSet : 7 - (m_size - cell); //Makes sure the loop doesn't go past the size

		std::cout << "s:" << std::to_string(m_size) << " eE:" << std::to_string(endEllipsis) << std::endl;

		for(int i = 0; i < 7; i++) {
			std::string formatted = formatChar(m_cells[cell - offSet + i]);
			finalString += formatted + "|";
		}

		//Now calculate offset for the arrow
		arrowPos += beginEllipsis ? 12 : cell * 4; //Brings arrow to left wall of the middle cell
		arrowPos += 1; //Brings arrow to the middle

		if(endEllipsis) {
			finalString += "...";
		} else {
			arrowPos += (4 - (m_size - cell)) * 4;						
		}

		for(int i = 0; i < arrowPos - 1; i++) {
			std::cout << " ";
		}
		std::cout << std::to_string(cell) << std::endl;

		for(int i = 0; i < arrowPos; i++) {
			std::cout << " ";
		}
		std::cout << "v" << std::endl;

		std::cout << finalString << std::endl;
	}

}