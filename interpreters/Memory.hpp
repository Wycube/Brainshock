#ifndef MEMORY_HPP
#define MEMORY_HPP

namespace bs {
	
	/** Memory used in Brainf, it is a tape of bytes */
	struct Tape {
		unsigned int m_size;
		unsigned char *m_cells;

		void printTape(int cell);

		unsigned char& operator[] (int index) {
			//Out-of-Bounds is undefined behavior

			return m_cells[index];
		}

		Tape(int size = 0);
		~Tape() {delete[] m_cells;}
	};
}

#endif