#ifndef JIT_INTERPRETER_HPP
#define JIT_INTERPRETER_HPP

#include <cstdint>
#include <vector>
#include <stack>
#include <iostream>

#include "Interpreter.hpp"
#include "jit/Emitter.hpp"
#include "jit/Runtime.hpp"

namespace bs {

namespace jit {

    class JITInterpreter : public Interpreter {
    public:
        
        JITInterpreter(std::ostream &stream = std::cout, std::size_t memSize = 30000);
        ~JITInterpreter();

        bool loadProgram(const char *program, bool process = true, bool resetDataPtr = true, unsigned int optimization = 2) override;
		bool run(float runSpeed = 0) override; //Run speed doesn't matter now, cause it can't be controlled, easily at least
		bool step() override; //This doesn't do anything

    private:

        JITRuntime m_runtime;
        x86_64Emitter m_jit_emitter;

        bool compile();
        bool compileInstr(Token instr, unsigned int label_counter, std::stack<unsigned int> &label_stack);
        bool compileInstr(char instr, unsigned int label_counter, std::stack<unsigned int> &label_stack);

        friend char func_getChar(JITInterpreter *instance);
    };

} //namespace jit

} //namespace bs

#endif //JIT_INTERPRETER_HPP