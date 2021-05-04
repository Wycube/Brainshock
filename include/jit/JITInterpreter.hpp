#ifndef JIT_INTERPRETER_HPP
#define JIT_INTERPRETER_HPP

#include <cstdint>
#include <vector>
#include <iostream>

#include "Interpreter.hpp"
#include "jit/Emitter.hpp"

namespace bs {

namespace jit {

    using JITFunc = void (*)(uint64_t *tape_memory);

    class JITRuntime {
    public:

        JITRuntime();
        ~JITRuntime();

        void loadCode(const std::vector<uint8_t> &code);
        void* getMemory();
        bool isExecutable();

    private:

        void createBuffer();
        void freeBuffer();
        void finalizeBuffer();

        bool m_isExecutable;
        uint8_t *m_memory;
    };

} //namespace jit

    class JITInterpreter : public Interpreter {
    public:
        
        JITInterpreter(std::ostream &stream = std::cout, std::size_t memSize = 30000);

        bool loadProgram(const char *program, bool process = true, bool resetDataPtr = true, unsigned int optimization = 2) override;
		bool run(float runSpeed = 0) override; //Run speed doesn't matter now, cause it can't be controlled, easily
		bool step() override; //This doesn't do anything

    private:

        jit::JITRuntime m_runtime;
        jit::x64Emitter m_emitter;

        void printChar(char character); //A function to print in the jit assembly, as an intermediate

    };

} //namespace bs

#endif //JIT_INTERPRETER_HPP