#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "Program.hpp"
#include "Memory.hpp"

#include <deque>
#include <iostream>


namespace bs {

    class Interpreter {
    public:

        Interpreter(std::ostream &stream = std::cout, std::size_t memSize = 30000);

        virtual bool loadProgram(const char *program, bool process = true, bool resetDataPtr = true, unsigned int optimization = 2) = 0;
		virtual bool run(float runSpeed = 0) = 0;
		virtual bool step() = 0;

        //Getters
		inline Program& getProgram()    { return m_program; }
		inline Tape& getMemory()        { return m_memory; }
		inline std::size_t getInstPtr() { return m_instPtr; }
		inline std::size_t getDataPtr() { return m_dataPtr; }
		inline std::string getError()   { return m_error; }

    protected:

        std::deque<char> m_inBuffer;
        std::ostream &m_stream;
		IREmitter m_emitter;
		Tape m_memory;
		Program m_program;
		std::size_t m_instPtr = 0;
		std::size_t m_dataPtr = 0;
		std::string m_error;

		char getChar();
    };


    class BasicInterpreter : public Interpreter {
	public:

		BasicInterpreter(std::ostream &stream = std::cout, std::size_t memSize = 30000);

		bool loadProgram(const char *program, bool process = true, bool resetDataPtr = true, unsigned int optimization = 2) override;
		bool run(float runSpeed = 0) override;
		bool step() override;

	private:
		
		std::deque<std::size_t> m_jumpTable;
		bool stepProcessed();
		bool stepUnprocessed();
	};

}

#endif //INTERPRETER_HPP