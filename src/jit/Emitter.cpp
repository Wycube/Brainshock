#include "jit/Emitter.hpp"

namespace bs {

namespace jit {

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



    //Select x86-64 Instructions
    //A lot of instructions change the prefix based on whether they are acting on registers
    //rax - rbp or r8 - r15, so that is accounted for.

    //Near returns from a procedure a.k.a the function
    void x64Emitter::ret() {
        emitBytes({0xC3});
    }

    //Moves a 64 bit immediate value into reg
    void x64Emitter::movabs(uint64_t immediate, x64Register reg) {
        if(reg <= rdi) {
            emitBytes({0x48, static_cast<uint8_t>(0xB8 + reg)}); 
        } else {
            emitBytes({0x49, static_cast<uint8_t>(0xB8 + (reg - 8))});
        }

        emitInt(immediate);
    }

    //Moves a 32 bit immediate value into reg
    void x64Emitter::mov(uint32_t immediate, x64Register reg) {
        if(reg <= rdi) {
            emitBytes({0x48, 0xC7, static_cast<uint8_t>(0xC0 + reg)}); 
        } else {
            emitBytes({0x49, 0xC7, static_cast<uint8_t>(0xC0 + (reg - 8))});
        }

        emitInt(immediate);
    }

    //Moves the contents of one register to another
    void x64Emitter::mov(x64Register src, x64Register dest) {
        uint8_t prefix = 0b01001000;
        prefix |= dest > rdi ? 1 : 0;
        prefix |= src > rdi ? 0b100 : 0;

        uint8_t modrm = 0b11000000;
        modrm |= src << 3;
        modrm |= dest;

        emitBytes({prefix, 0x89, modrm});
    }

    //Increments the value in reg
    void x64Emitter::inc(x64Register reg) {
        if(reg <= rdi) {
            emitBytes({0x48, 0xFF, static_cast<uint8_t>(0xC0 + reg)});
        } else {
            emitBytes({0x49, 0xFF, static_cast<uint8_t>(0xC0 + (reg - 8))});
        }
    }

    //Decrements the value in reg
    void x64Emitter::dec(x64Register reg) {
        if(reg <= rdi) {
            emitBytes({0x48, 0xFF, static_cast<uint8_t>(0xC8 + reg)});
        } else {
            emitBytes({0x49, 0xFF, static_cast<uint8_t>(0xC8 + (reg - 8))});
        }
    }

    //Adds the byte value to the byte pointed to by register reg
    //There is an offset from the pointer, the second to last operand, which is present because
    //it is impossible to encode r13 without an offset and I don't want to add an edge case.
    void x64Emitter::addb_at_reg(uint8_t value, x64Register reg) {
        if(reg <= rdi) {
            emitBytes({0x80, static_cast<uint8_t>(0x40 + reg), 0x00, value});
        } else {
            emitBytes({0x41, 0x80, static_cast<uint8_t>(0x40 + (reg - 8)), 0x00, value});
        }
    }
    
    //Subtracts the byte value from the byte pointed to by register reg
    void x64Emitter::subb_at_reg(uint8_t value, x64Register reg) {
        if(reg <= rdi) {
            emitBytes({0x80, static_cast<uint8_t>(0x68 + reg), 0x00, value});
        } else {
            emitBytes({0x41, 0x80, static_cast<uint8_t>(0x68 + (reg - 8)), 0x00, value});
        }
    }

    //Adds the integer value to the value in register reg
    void x64Emitter::add_to_reg(uint32_t value, x64Register reg) {
        if(reg == rax) {
            emitBytes({0x48, 0x05}); //This one is different, for some reason
        } else if(reg <= rdi) {
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
        } else if(reg <= rdi) {
            emitBytes({0x48, 0x81, static_cast<uint8_t>(0xE8 + reg)});
        } else {
            emitBytes({0x49, 0x81, static_cast<uint8_t>(0xE8 + (reg - 8))});
        }

        emitInt(value);
    }

    //Pushes a register onto the stack
    void x64Emitter::push_reg(x64Register reg) {
        if(reg <= rdi) {
            emitBytes({static_cast<uint8_t>(0x50 + reg)});
        } else {
            emitBytes({0x41, static_cast<uint8_t>(0x50 + (reg - 8))});
        }
    }

    //Pops a register off the stack
    void x64Emitter::pop_reg(x64Register reg) {
        if(reg <= rdi) {
            emitBytes({static_cast<uint8_t>(0x58 + reg)});
        } else {
            emitBytes({0x41, static_cast<uint8_t>(0x58 + (reg - 8))});
        }
    }

    //Compare the contents pointed to by the specified register with the byte value
    void x64Emitter::cmpb_at_reg(uint8_t value, x64Register reg) {
        if(reg <= rdi) {
            emitBytes({0x80, static_cast<uint8_t>(0x78 + reg), 0x00, value});
        } else {
            emitBytes({0x41, 0x80, static_cast<uint8_t>(0x78 + (reg - 8)), 0x00, value});
        }
    }

    //Jump if not zero, the address is relative
    void x64Emitter::jnz(uint32_t relative) {
        emitBytes({0x0F, 0x85});
        emitInt(relative);
    }

    //Jump if zero, the address is relative
    void x64Emitter::jz(uint32_t relative) {
        emitBytes({0x0F, 0x84});
        emitInt(relative);
    }

}

}