#ifndef MEMORY_HPP
#define MEMORY_HPP

namespace bs {
	
	/** Memory used in Brainf, is a tape of bytes */
	struct Tape {
		unsigned int m_size;
		unsigned char *m_cells;

		Tape(int size);
		Tape() : Tape(30000) {} //30000 is standard size for Brainf
		~Tape() {delete[] m_cells;}

		void printTape(int cell);

		unsigned char& operator[] (int index) {
			//Out-of-Bounds is undefined behavior

			return m_cells[index];
		}
	};
}

#endif