#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstddef>

namespace bs {
	
	enum DUMP_BASE {
		BASE_HEX,
		BASE_DEC,
		BASE_BIN
	};

	/** Memory used in Brainf, it is a array of bytes */
	struct Tape {
		std::size_t m_size;
		unsigned char *m_cells;

		Tape(std::size_t size = 0);
		~Tape() {delete[] m_cells;}

		//void printTape(int cell); //
		void fPrint(int cell);
		void fDump(DUMP_BASE base = BASE_HEX, bool ascii = false);

		inline unsigned char& operator[] (int index) {
			//Out-of-Bounds is undefined behavior

			return m_cells[index];
		}
	};
}

#endif //MEMORY_HPP