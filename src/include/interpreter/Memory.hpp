#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstddef>
#include <cstring>
#include <iostream>

namespace bs {

	static int num = 0;
	
	enum DUMP_BASE {
		BASE_HEX = 16,
		BASE_DEC = 10,
		BASE_BIN = 2
	};

	/** Memory used in Brainf, it is a array of bytes */
	struct Tape {
		Tape(std::size_t size = 0);
		~Tape() { delete[] m_cells; }

		Tape& operator=(Tape const &other) { m_size = other.m_size; m_cells = new unsigned char[m_size]; memcpy(m_cells, other.m_cells, m_size); return *this; }

		void fPrint(int cell);
		void fDump(DUMP_BASE base = BASE_HEX, bool ascii = false);

		inline unsigned char& operator[] (std::size_t index) {
			if(index < 0 || index > m_size) {
				outOfBounds = true;
				return 0;
			}

			outOfBounds = false;
			return m_cells[index];
		}

		bool outOfBounds = false;
		std::size_t m_size;
		unsigned char *m_cells;
	};
}

#endif //MEMORY_HPP








