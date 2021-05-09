#include "jit/JITInterpreter.hpp"

#include "jit/Platform.hpp"

namespace bs {

namespace jit {

    //Functions for using in the jit, so I don't have to deal with method pointers
    void func_printChar(char *c) {
        std::cout << *c << std::flush;
    }

    char func_getChar(JITInterpreter *instance) {
        return instance->getChar();
    } 

    //--------------- JIT Interpreter class methods ---------------//

    JITInterpreter::JITInterpreter(std::ostream &stream, std::size_t memSize) : Interpreter(stream, memSize) { }

    JITInterpreter::~JITInterpreter() { }

    bool JITInterpreter::loadProgram(const char *program, bool process, bool resetDataPtr, unsigned int optimization) {
        m_emitter.loadSource(program);

		m_instPtr = 0;

		if(resetDataPtr)
			m_dataPtr = 0;

		if(process) {
			m_program.tokenize();

			if(!m_emitter.expr()) { m_error = m_emitter.getError(); return false; } //Program has invalid syntax

			if(optimization > 0) {
				m_emitter.optimize(optimization);

				//This is called to update the jump locations of the brackets
				if(!m_emitter.expr()) { m_error = m_emitter.getError(); return false; } //If there was an error in the optimizing, which shouldn't happen
			}
		}

		m_program = m_emitter.emit();

		return compile();
    }

    bool JITInterpreter::run(float runSpeed) {
        JITFunc func = reinterpret_cast<JITFunc>(m_runtime.getMemory());

        func((uint64_t*)m_memory.m_cells);

        return true;
    }

    bool JITInterpreter::step() {
        return false;
    }

    bool JITInterpreter::compile() {
        //This is where the fun begins
        m_jit_emitter.push_reg(r13);
        m_jit_emitter.push_reg(r14);
        m_jit_emitter.push_reg(r15);

        m_jit_emitter.movabs(reinterpret_cast<uint64_t>(func_printChar), r14);
        m_jit_emitter.movabs(reinterpret_cast<uint64_t>(func_getChar), r15);

        std::size_t size = m_program.processed ? m_program.tokens.size() : m_program.source.size();
        unsigned int label_counter = 0;
        std::stack<unsigned int> label_stack;

        while(m_instPtr < size) {
            if(m_program.processed) {
                if(!compileInstr(m_program.tokens[m_instPtr], label_counter, label_stack)) { m_jit_emitter.clear(); return false; }
            } else {
                if(!compileInstr(m_program.source[m_instPtr], label_counter, label_stack)) { m_jit_emitter.clear(); return false; }
            }
        }

        m_jit_emitter.pop_reg(r15);
        m_jit_emitter.pop_reg(r14);
        m_jit_emitter.pop_reg(r13);
        m_jit_emitter.ret();

        if(!m_jit_emitter.resolveLabels()) {
            m_jit_emitter.clear();
            m_error = "Failed to resolve labels";
            return false;
        }

        return true;
    }

    bool JITInterpreter::compileInstr(Token instr, unsigned int label_counter, std::stack<unsigned int> &label_stack) {
        switch(instr.identifier) {
            case SHIFT_RIGHT : m_jit_emitter.add_to_reg(instr.data, r13);
            break;
            case SHIFT_LEFT : m_jit_emitter.sub_from_reg(instr.data, r13);
            break;
            case INCREMENT : m_jit_emitter.addb_at_reg(instr.data, r13);
            break;
            case DECREMENT : m_jit_emitter.subb_at_reg(instr.data, r13);
            break;
            case START_LOOP :
                label_stack.push(label_counter);

                m_jit_emitter.cmpb_at_reg(0, r13);
                m_jit_emitter.jz(std::string("end_") + std::to_string(label_counter));
                m_jit_emitter.emitLabel(std::string("start_") + std::to_string(label_counter));

                label_counter++;
            break;
            case END_LOOP :
                if(label_stack.empty()) {
                    m_error = std::string("Missing open loop at ") + std::to_string(m_instPtr);
                    return false;
                }

                m_jit_emitter.cmpb_at_reg(0, r13);
                m_jit_emitter.jnz(std::string("start_") + std::to_string(label_stack.top()));
                m_jit_emitter.emitLabel(std::string("end_") + std::to_string(label_stack.top()));

                label_stack.pop();
            break;
            case INPUT :
                //Move a pointer to this instance into rdi for the System V ABI, and rcx for the Windows ABI
                #if defined(PLATFORM_WINDOWS)
                    m_jit_emitter.movabs(reinterpret_cast<uint64_t>(this), rcx);
                #else
                    m_jit_emitter.movabs(reinterpret_cast<uint64_t>(this), rdi);
                #endif

                m_jit_emitter.call_at_reg(r15);
                m_jit_emitter.mov_al_at_reg(r13);
            break;
            case OUTPUT :
                #if defined(PLATFORM_WINDOWS)
                    m_jit_emitter.mov(r13, rcx);
                #else
                    m_jit_emitter.mov(r13, rdi);
                #endif

                m_jit_emitter.call_at_reg(r14);
            break;
            case CLEAR : m_jit_emitter.mov_at_reg(0, r13);
            break;
            case COPY : //Later
            break;
        }

        return true;
    }

    bool JITInterpreter::compileInstr(char instr, unsigned int label_counter, std::stack<unsigned int> &label_stack) {
        switch(instr) {
            case SHIFT_RIGHT : m_jit_emitter.inc(r13);
            break;
            case SHIFT_LEFT : m_jit_emitter.dec(r13);
            break;
            case INCREMENT : m_jit_emitter.addb_at_reg(1, r13);
            break;
            case DECREMENT : m_jit_emitter.subb_at_reg(1, r13);
            break;
            case START_LOOP :
                label_stack.push(label_counter);

                m_jit_emitter.cmpb_at_reg(0, r13);
                m_jit_emitter.jz(std::string("end_") + std::to_string(label_counter));
                m_jit_emitter.emitLabel(std::string("start_") + std::to_string(label_counter));

                label_counter++;
            break;
            case END_LOOP : 
                if(label_stack.empty()) {
                    m_error = std::string("Missing open loop at ") + std::to_string(m_instPtr);
                    return false;
                }

                m_jit_emitter.cmpb_at_reg(0, r13);
                m_jit_emitter.jnz(std::string("start_") + std::to_string(label_stack.top()));
                m_jit_emitter.emitLabel(std::string("end_") + std::to_string(label_stack.top()));

                label_stack.pop();
            break;
            case INPUT :
                //Move a pointer to this instance into rdi for the System V ABI, and rcx for the Windows ABI
                #if defined(PLATFORM_WINDOWS)
                    m_jit_emitter.movabs(reinterpret_cast<uint64_t>(this), rcx);
                #else
                    m_jit_emitter.movabs(reinterpret_cast<uint64_t>(this), rdi);
                #endif

                m_jit_emitter.call_at_reg(r15);
                m_jit_emitter.mov_al_at_reg(r13);
            break;
            case OUTPUT :
                #if defined(PLATFORM_WINDOWS)
                    m_jit_emitter.mov(r13, rcx);
                #else
                    m_jit_emitter.mov(r13, rdi);
                #endif

                m_jit_emitter.call_at_reg(r14);
            break;
        }

        return true;
    }

} //namespace jit

} //namespace bs