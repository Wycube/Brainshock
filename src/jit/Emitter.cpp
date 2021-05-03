#include "jit/Emitter.hpp"

namespace bs {

    std::vector<uint8_t> x64Emitter::getCode() {
        return m_code;
    }

    std::size_t x64Emitter::size() {
        return m_code.size();
    }

    void x64Emitter::clear() {
        m_code.clear();
    }

    void x64Emitter::emitBytes(std::vector<uint8_t> bytes) {
        for(uint8_t byte : bytes) {
            m_code.push_back(byte);
        }
    }

    void x64Emitter::emitU32(uint32_t value) {
        m_code.push_back(GET_BYTE(value, 0));
        m_code.push_back(GET_BYTE(value, 1));
        m_code.push_back(GET_BYTE(value, 2));
        m_code.push_back(GET_BYTE(value, 3));
    }

    void x64Emitter::emitU64(uint64_t value) {
        m_code.push_back(GET_BYTE(value, 0));
        m_code.push_back(GET_BYTE(value, 1));
        m_code.push_back(GET_BYTE(value, 2));
        m_code.push_back(GET_BYTE(value, 3));
        m_code.push_back(GET_BYTE(value, 4));
        m_code.push_back(GET_BYTE(value, 5));
        m_code.push_back(GET_BYTE(value, 6));
        m_code.push_back(GET_BYTE(value, 7));
    }

    //Select x86-64 Instructions
    //A lot of instructions change the prefix based on whether they are acting on registers
    //rax - rbp or r8 - r15, so that is accounted for.

    //Near returns from a procedure a.k.a the function
    void x64Emitter::ret() {
        emitBytes({0xC3});
    }

    //Moves a 64 bit immediate value into reg
    void x64Emitter::movabs(uint64_t immediate, x64Register reg) {
        if(reg <= rbp) {
            emitBytes({0x48, static_cast<uint8_t>(0xB8 + reg)}); 
        } else {
            emitBytes({0x49, static_cast<uint8_t>(0xB8 + (reg - 8))});
        }

        emitInt(immediate);
    }

    //Increments the value in reg
    void x64Emitter::inc(x64Register reg) {
        if(reg <= rbp) {
            emitBytes({0x48, 0xFF, static_cast<uint8_t>(0xC0 + reg)});
        } else {
            emitBytes({0x49, 0xFF, static_cast<uint8_t>(0xC0 + (reg - 8))});
        }
    }

    //Decrements the value in reg
    void x64Emitter::dec(x64Register reg) {
        if(reg <= rbp) {
            emitBytes({0x48, 0xFF, static_cast<uint8_t>(0xC8 + reg)});
        } else {
            emitBytes({0x49, 0xFF, static_cast<uint8_t>(0xC8 + (reg - 8))});
        }
    }

    //Adds the byte value to the byte pointed to by register reg
    //There is an offset from the pointer, the second to last operand, which isn't used right now, but might be useful later
    void x64Emitter::addb_at_reg(uint8_t value, x64Register reg) {
        if(reg <= rbp) {
            emitBytes({0x80, static_cast<uint8_t>(0x45 + reg), 0x00, value});
        } else {
            emitBytes({0x41, 0x80, static_cast<uint8_t>(0x45 + (reg - 8)), 0x00, value});
        }
    }
    
    //Subtracts the byte value from the byte pointed to by register reg
    void x64Emitter::subb_at_reg(uint8_t value, x64Register reg) {
        if(reg <= rbp) {
            emitBytes({0x80, static_cast<uint8_t>(0x68 + reg), 0x00, value});
        } else {
            emitBytes({0x41, 0x80, static_cast<uint8_t>(0x68 + (reg - 8)), 0x00, value});
        }
    }

    //Adds the integer value to the value in register reg
    void x64Emitter::add_to_reg(uint32_t value, x64Register reg) {
        if(reg == rax) {
            emitBytes({0x48, 0x05}); //This one is different, for some reason
        } else if(reg <= rbp) {
            emitBytes({0x48, 0x81, static_cast<uint8_t>(0xC0 + reg)});
        } else {
            emitBytes({0x49, 0x81, static_cast<uint8_t>(0xC0 + (reg - 8))});
        }

        emitInt(value);
    }

    //Subtracts the integer value from the value in register reg
    void x64Emitter::sub_to_reg(uint32_t value, x64Register reg) {
        if(reg == rax) {
            emitBytes({0x48, 0x2D}); //This one is different, for some reason
        } else if(reg <= rbp) {
            emitBytes({0x48, 0x81, static_cast<uint8_t>(0xE8 + reg)});
        } else {
            emitBytes({0x49, 0x81, static_cast<uint8_t>(0xE8 + (reg - 8))});
        }

        emitInt(value);
    }

}