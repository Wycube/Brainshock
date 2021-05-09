#ifndef JIT_INTERPRETER_HPP
#define JIT_INTERPRETER_HPP

#include <cstdint>
#include <vector>
#include <iostream>

#include "Interpreter.hpp"
#include "jit/Emitter.hpp"
#include "jit/Runtime.hpp"

namespace bs {

namespace jit {

    class JITInterpreter : public Interpreter {
    public:
        
        JITInterpreter(std::ostream &stream = std::cout, std::size_t memSize = 30000);

        bool loadProgram(const char *program, bool process = true, bool resetDataPtr = true, unsigned int optimization = 2) override;
		bool run(float runSpeed = 0) override; //Run speed doesn't matter now, cause it can't be controlled, easily at least
		bool step() override; //This doesn't do anything

    private:

        JITRuntime m_runtime;
        x86_64Emitter m_jit_emitter;

        void printChar(char c); //A function to print in the jit assembly, as an intermediate
        bool compile();
    };

} //namespace jit

} //namespace bs

#endif //JIT_INTERPRETER_HPP