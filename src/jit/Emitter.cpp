#include "jit/Emitter.hpp"

#include <algorithm>

namespace bs {

namespace jit {

    std::vector<uint8_t> x86_64Emitter::getCode() {
        return m_code;
    }

    std::size_t x86_64Emitter::size() {
        return m_code.size();
    }

    void x86_64Emitter::clear() {
        m_code.clear();
    }

    void x86_64Emitter::emitBytes(std::vector<uint8_t> bytes) {
        for(uint8_t byte : bytes) {
            m_code.push_back(byte);
        }
    }



    //Select x86-64 Instructions
    //A lot of instructions change the prefix based on whether they are acting on registers
    //rax - rbp or r8 - r15, so that is accounted for.

    //Near returns from a procedure a.k.a the function
    void x86_64Emitter::ret() {
        emitBytes({0xC3});
    }

    //Moves a 64 bit immediate value into reg
    void x86_64Emitter::movabs(uint64_t immediate, x64GPRegister reg) {
        if(reg <= rdi) {
            emitBytes({0x48, static_cast<uint8_t>(0xB8 + reg)}); 
        } else {
            emitBytes({0x49, static_cast<uint8_t>(0xB8 + (reg - 8))});
        }

        emitInt(immediate);
    }

    //Moves a 32 bit immediate value into reg
    void x86_64Emitter::mov(uint32_t immediate, x64GPRegister reg) {
        if(reg <= rdi) {
            emitBytes({0x48, 0xC7, static_cast<uint8_t>(0xC0 + reg)}); 
        } else {
            emitBytes({0x49, 0xC7, static_cast<uint8_t>(0xC0 + (reg - 8))});
        }

        emitInt(immediate);
    }

    //Moves the contents of one register to another
    void x86_64Emitter::mov(x64GPRegister src, x64GPRegister dest) {
        uint8_t prefix = 0b01001000;
        prefix |= dest > rdi ? 1 : 0;
        prefix |= src > rdi ? 0b100 : 0;

        uint8_t modrm = 0b11000000;
        modrm |= src << 3;
        modrm |= dest;

        emitBytes({prefix, 0x89, modrm});
    }

    //Increments the value in reg
    void x86_64Emitter::inc(x64GPRegister reg) {
        if(reg <= rdi) {
            emitBytes({0x48, 0xFF, static_cast<uint8_t>(0xC0 + reg)});
        } else {
            emitBytes({0x49, 0xFF, static_cast<uint8_t>(0xC0 + (reg - 8))});
        }
    }

    //Decrements the value in reg
    void x86_64Emitter::dec(x64GPRegister reg) {
        if(reg <= rdi) {
            emitBytes({0x48, 0xFF, static_cast<uint8_t>(0xC8 + reg)});
        } else {
            emitBytes({0x49, 0xFF, static_cast<uint8_t>(0xC8 + (reg - 8))});
        }
    }

    //Adds the byte value to the byte pointed to by register reg
    //There is an offset from the pointer, the second to last operand, which is present because
    //it is impossible to encode r13 without an offset and I don't want to add an edge case.
    void x86_64Emitter::addb_at_reg(uint8_t value, x64GPRegister reg) {
        if(reg <= rdi) {
            emitBytes({0x80, static_cast<uint8_t>(0x40 + reg), 0x00, value});
        } else {
            emitBytes({0x41, 0x80, static_cast<uint8_t>(0x40 + (reg - 8)), 0x00, value});
        }
    }
    
    //Subtracts the byte value from the byte pointed to by register reg
    void x86_64Emitter::subb_at_reg(uint8_t value, x64GPRegister reg) {
        if(reg <= rdi) {
            emitBytes({0x80, static_cast<uint8_t>(0x68 + reg), 0x00, value});
        } else {
            emitBytes({0x41, 0x80, static_cast<uint8_t>(0x68 + (reg - 8)), 0x00, value});
        }
    }

    //Adds the integer value to the value in register reg
    void x86_64Emitter::add_to_reg(uint32_t value, x64GPRegister reg) {
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
    void x86_64Emitter::sub_to_reg(uint32_t value, x64GPRegister reg) {
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
    void x86_64Emitter::push_reg(x64GPRegister reg) {
        if(reg <= rdi) {
            emitBytes({static_cast<uint8_t>(0x50 + reg)});
        } else {
            emitBytes({0x41, static_cast<uint8_t>(0x50 + (reg - 8))});
        }
    }

    //Pops a register off the stack
    void x86_64Emitter::pop_reg(x64GPRegister reg) {
        if(reg <= rdi) {
            emitBytes({static_cast<uint8_t>(0x58 + reg)});
        } else {
            emitBytes({0x41, static_cast<uint8_t>(0x58 + (reg - 8))});
        }
    }

    //Compare the contents pointed to by the specified register with the byte value
    void x86_64Emitter::cmpb_at_reg(uint8_t value, x64GPRegister reg) {
        if(reg <= rdi) {
            emitBytes({0x80, static_cast<uint8_t>(0x78 + reg), 0x00, value});
        } else {
            emitBytes({0x41, 0x80, static_cast<uint8_t>(0x78 + (reg - 8)), 0x00, value});
        }
    }

    //Jump if not zero, the address is relative
    void x86_64Emitter::jnz(int32_t relative) {
        emitBytes({0x0F, 0x85});
        emitInt(relative);
    }

    //Jump if zero, the address is relative
    void x86_64Emitter::jz(int32_t relative) {
        emitBytes({0x0F, 0x84});
        emitInt(relative);
    }

    //Jump if not zero, a label is used and resolved later
    void x86_64Emitter::jnz(const std::string &label_name) {
        emitBytes({0x0F, 0x85});
        emitInt<uint32_t>(0);

        m_ref_labels.push_back(Label{label_name, m_code.size() - 4, true});
    }

    //Jump if zero, a label is used and resolved later
    void x86_64Emitter::jz(const std::string &label_name) {
        emitBytes({0x0F, 0x84});
        emitInt<uint32_t>(0);

        m_ref_labels.push_back(Label{label_name, m_code.size() - 4, true});
    }

    //Call the function at the address stored in the 64-bit register
    void x86_64Emitter::call_at_reg(x64GPRegister reg) {
        if(reg <= rdi) {
            emitBytes({0xFF, static_cast<uint8_t>(0xD0 + reg)});
        } else {
            emitBytes({0x41, 0xFF, static_cast<uint8_t>(0xD0 + (reg - 8))});
        }
    }

    //Create a label at the current memory location, duplicates aren't added
    void x86_64Emitter::emitLabel(const std::string &label_name) {
        auto iter = std::find(m_src_labels.begin(), m_src_labels.end(), label_name);

        if(iter != m_src_labels.end()) {
            return;
        }

        m_src_labels.push_back(Label{label_name, m_code.size(), false});
    }

    //Resolve the labels, return whether it succeeded
    bool x86_64Emitter::resolveLabels() {
        //Loop through reference labels and calculate the relative offset from the source label
        for(const Label &label : m_ref_labels) {
            auto iter = std::find(m_src_labels.begin(), m_src_labels.end(), label.name);

            if(iter == m_src_labels.end()) {
                return false;
            }

            //Subtract 4 because the offset starts from the byte after the instruction but the location is at the start of the operand
            int32_t offset = iter->location - label.location - 4;

            m_code[label.location + 0] = GET_BYTE(offset, 0);
            m_code[label.location + 1] = GET_BYTE(offset, 1);
            m_code[label.location + 2] = GET_BYTE(offset, 2);
            m_code[label.location + 3] = GET_BYTE(offset, 3);
        }

        m_src_labels.clear();
        m_ref_labels.clear();

        return true;
    }

}

}