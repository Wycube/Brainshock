#include "Memory.hpp"

#include <cstring>
#include <cmath>
#include <cctype>
#include <string>
#include <iostream>
#include <iomanip>
#include <bitset>

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




	//Helper function for fPrint, formats number
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
	 *                   26
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

		//Now print it
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

	//Helper function for fDump
	int numDigits(int number) {
		std::string strNum = std::to_string(number);
		return strNum.length();
	}

	/**
	 * Sort of like hexdump but as a function on the tape.
	 * Can print in 3 different bases defined in the DUMP_BASE enum.
	 *
	 * @param base The base to print the values as
	 * @param ascii Whether or not the ascii value should be printed as well
	 */
	void Tape::fDump(DUMP_BASE base, bool ascii) {
		std::size_t valuesPerLine, totalLines;
		int baseDigits;

		//Values chosen based on a 80x24 terminal window
		if(base == BASE_HEX) {
			valuesPerLine = 16;
			baseDigits = 2;
		} else if(base == BASE_DEC) {
			valuesPerLine = 10;
			baseDigits = 3;
		} else if(base == BASE_BIN) {
			valuesPerLine = 4;
			baseDigits = 8;
		} else {
			valuesPerLine = 16;//Hex is default
			baseDigits = 2;
		}

		totalLines = std::ceil(m_size / valuesPerLine);
		int digits = numDigits(m_size);
		int currentLine = 0;

		//Checking for duplicates
		std::stringstream lastLine;
		std::stringstream currLine;
		std::string endLine;
		bool duplicate = false;

		//ascii part
		std::string asChar;

		//Formatting
		lastLine << std::setbase(base) << std::setfill('0');
		currLine << std::setbase(base) << std::setfill('0');
		std::cout << std::setfill('0');

		for(std::size_t i = 0; i < totalLines; i++) {
			
			//Beginning deliminattor
			if(ascii)
				asChar = "(";

			for(std::size_t j = 0; j < valuesPerLine; j++) {
				//Print the number for a certain base
				std::string seperator = j == (valuesPerLine / 2) - 1 ? "  " : " ";

				unsigned char value = m_cells[j + i * valuesPerLine];

				//ASCII value printing or a period if not printable
				if(ascii) {
					asChar += isprint(value) ? value : '.';
				}

				//Output streams don't support base 2 so I used bitsets
				if(base == BASE_BIN)
					currLine << std::bitset<8>(value) << seperator;
				else
					currLine << std::setw(baseDigits) << static_cast<int>(value) << seperator;
			}

			//Beginning deliminattor
			if(ascii)
				asChar += ")";

			//Check for duplicate
			if(currLine.str() == lastLine.str()) {
				duplicate = true;

				//Clear currLine after storing it
				endLine = currLine.str();
				currLine.str(std::string());
			} else {

				//Indicates duplicate lines
				if(duplicate)
					std::cout << "*" << std::endl;

				duplicate = false;

				//Print current line
				std::cout << std::setw(digits) << currentLine << " : ";

				if(ascii)
					std::cout << currLine.str() << asChar << std::endl;
				else
					std::cout << currLine.str() << std::endl;

				//Clear currLine after swap
				lastLine.swap(currLine);
				currLine.str(std::string());
			}

			currentLine += valuesPerLine;
		}

		//CurrentLine has to be lowered because it's at the end
		if(duplicate) {
			std::cout << "*" << std::endl;
			std::cout << std::setw(digits) << (currentLine -= valuesPerLine) << " : ";
			if(ascii)
					std::cout << endLine << asChar << std::endl;
				else
					std::cout << endLine << std::endl;
		}

		//This is a big function, there is probably a better way to do this, idk

		//Reset Attributes
		std::cout << std::setbase(10) << std::setfill(' ');
	}

}