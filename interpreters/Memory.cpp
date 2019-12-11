#include "Memory.hpp"
#include <string>
#include <iostream>

namespace bs {

	/**
	 * Formats a string so that the char has a certain
	 * number of spaces before it based on the number of digits.
	 * Used for the printTape() function.
	 *
	 * @param num The number for formatting
	 *
	 * @return A std::string with the spaces and number
	 */
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

	/**
	 * Print the tape formatted like this.
	 *                    V
	 *                    26
	 *   ...|  0| 10|120|  4| 12| 11|123|255|... 
	 *
	 * @param cell Where to print in the tape, centered on that cell
	 */
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

}