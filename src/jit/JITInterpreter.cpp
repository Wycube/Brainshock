#include "jit/JITInterpreter.hpp"

namespace bs {

namespace jit {

    //--------------- JIT Interpreter class methods ---------------//

    JITInterpreter::JITInterpreter(std::ostream &stream, std::size_t memSize) : Interpreter(stream, memSize) { }

    bool JITInterpreter::loadProgram(const char *program, bool process, bool resetDataPtr, unsigned int optimization) {
        m_emitter.loadSource(program);

		m_instPtr = 0;

		if(resetDataPtr)
			m_dataPtr = 0;

		if(process) {
			m_program.tokenize();

			if(!m_emitter.expr()) return false; //Program has invalid syntax

			if(optimization > 0) {
				m_emitter.optimize(optimization);

				//This is called to update the jump locations of the brackets
				if(!m_emitter.expr()) return false; //If there was an error in the optimizing, which shouldn't happen
			}
		}

		m_program = m_emitter.emit();

		return compile();
    }

    bool JITInterpreter::run(float runSpeed) {
        JITFunc func = reinterpret_cast<JITFunc>(m_runtime.getMemory());

        func((uint64_t*)m_memory.m_cells);
    }

    bool JITInterpreter::step() {
        return false;
    }

    void JITInterpreter::printChar(char c) {
        std::cout << c << std::flush;
    }

    bool JITInterpreter::compile() {
        //This is where the fun begins
        
    }

} //namespace jit

} //namespace bs