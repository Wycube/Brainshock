#ifndef INTERPRETER_H
#define INTERPRETER_H

namespace bs {

	class Tape {
		public:
		const int m_length;

		Tape(int length);
		~Tape();

		void incPtr() {m_cellPtr++;}
		void decPtr() {m_cellPtr--;}
		void printTape();
		void setCell(unsigned char value);
		char getCell();

		//private:
		int m_cellPtr;
		unsigned char *m_cells;
	};

	class Program {
		private:
		const char *m_program;
		int m_length;
		int m_instPtr;

		public:
		Program(const char *string);

		void incPtr() {m_instPtr++;}
		void decPtr() {m_instPtr--;}
		bool jump(int instruction);
		bool hasNext();
		char getInst();
		int getPtr();
	};

	class Interpreter {
		private:
		Program m_program;
		Tape m_tape;
		
		public:
		bool m_debug = false;
		int m_jmp;

		Interpreter(const char *string, int memLength);

		bool run();
		bool step(int numSteps = 1);

	};
}
#endif