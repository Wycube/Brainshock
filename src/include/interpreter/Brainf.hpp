#ifndef BRAINF_HPP
#define BRAINF_HPP

#include "Program.hpp"
#include "Memory.hpp"

#include <deque>
#include <stack>
#include <iostream>
#include <chrono>

namespace bs {

	class BrainfInterpreter {
	public:

		BrainfInterpreter(std::ostream &stream = std::cout, bool numInput = false, std::size_t memSize = 30000);

		bool loadProgram(const char *program, bool process = true, bool resetDataPtr = true, bool resetInputTime = false, unsigned int optimization = 2);
		bool run(float runSpeed = 0);
		bool step();
		bool drun(float runSpeed = 0); //Debug versions
		bool dstep();

		//Getters
		inline Program& getProgram()    { return m_program; }
		inline Tape& getMemory()        { return m_memory; }
		inline std::size_t getInstPtr() { return m_instPtr; }
		inline std::size_t getDataPtr() { return m_dataPtr; }
		inline std::string getError()   { return m_error; }
		inline std::chrono::microseconds getInputTime() { return m_inputTime; }

	private:
		
		std::deque<char> m_inBuffer;
		std::stack<std::size_t> m_jumpTable;
		std::ostream &m_stream;
		Program m_program;
		Tape m_memory;
		std::size_t m_instPtr = 0;
		std::size_t m_dataPtr = 0;
		std::string m_error;
		std::chrono::microseconds m_inputTime{0};
		bool m_numInput;

		char getChar();
		bool stepProcessed();
		bool stepUnprocessed();
		bool dstepProcessed(); //Debug versions
		bool dstepUnprocessed();
		bool expr();
		void preProcess(unsigned int optimization);
	};

}

#endif //BRAINF_HPP
