#ifndef JIT_INTERPRETER_HPP
#define JIT_INTERPRETER_HPP

#include <cstdint>
#include <vector>
#include <iostream>

#include "Interpreter.hpp"
#include "jit/Emitter.hpp"
#include "jit/Runtime.hpp"

namespace bs {

    class JITInterpreter : public Interpreter {
    public:
        
        JITInterpreter(std::ostream &stream = std::cout, std::size_t memSize = 30000);

        bool loadProgram(const char *program, bool process = true, bool resetDataPtr = true, unsigned int optimization = 2) override;
		bool run(float runSpeed = 0) override; //Run speed doesn't matter now, cause it can't be controlled, easily
		bool step() override; //This doesn't do anything

    private:

        jit::JITRuntime m_runtime;
        jit::x86_64Emitter m_emitter;

        void printChar(char character); //A function to print in the jit assembly, as an intermediate

    };

} //namespace bs

#endif //JIT_INTERPRETER_HPP