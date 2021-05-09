#ifndef EMITTER_HPP
#define EMITTER_HPP

#define GET_BYTE(number, byte) number >> (8 * byte) & 0xff

#include <cstdint>
#include <vector>
#include <string>

namespace bs {

namespace jit {

    struct Label {
        std::string name;
        std::size_t location;
        bool reference;

        bool operator==(const Label &other) {
            return name == other.name && location == other.location && reference == other.reference;
        }

        bool operator==(const std::string &other) {
            return name == other;
        }
    };

    //64-bit registers
    enum x64GPRegister : uint8_t {
        rax = 0,
        rcx = 1,
        rdx = 2,
        rbx = 3,
        rsp = 4,
        rbp = 5,
        rsi = 6,
        rdi = 7,
        r8  = 8,
        r9  = 9,
        r10 = 10,
        r11 = 11,
        r12 = 12,
        r13 = 13,
        r14 = 14,
        r15 = 15
    };

    class x86_64Emitter {
    public:

        std::vector<uint8_t> getCode();
        std::size_t size();
        void clear();

        void emitBytes(std::vector<uint8_t> bytes);

        //Pushes a integer value into the byte vector in Little-Endian
        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
        void emitInt(T value) {
            int num_bytes = sizeof(T);

            for(size_t i = 0; i < num_bytes; i++) {
                m_code.push_back(GET_BYTE(value, i));
            }
        }

        //Instructions
        //r13 will be the register storing the tape memory pointer
        //Assembly instructions are in AT&T syntax, i.e. op src, dest
        //Since I'm only implementing a handful of instructions I won't bother with
        //explicitly implementing prefixes and ModRM and that other garbage.

        void ret();                                            // ret
        void movabs(uint64_t immediate, x64GPRegister reg);    // movabs imm64, %r
        void mov(uint32_t immediate, x64GPRegister reg);       // mov imm32, %r
        void mov(x64GPRegister src, x64GPRegister dest);       // mov %src, %dest
        void mov_at_reg(uint8_t immediate, x64GPRegister reg); // movb imm8, (%r)
        void mov_al_at_reg(x64GPRegister reg);                 // movb %al, (%r)
        void inc(x64GPRegister reg);                           // inc %r
        void dec(x64GPRegister reg);                           // dec %r
        void addb_at_reg(uint8_t value, x64GPRegister reg);    // addb value, 0(%r) or in intel syntax: add BYTE PTR [%r + 0x0], value
        void subb_at_reg(uint8_t value, x64GPRegister reg);    // subb value, 0(%r)
        void add_to_reg(uint32_t value, x64GPRegister reg);    // add imm32, %r
        void sub_from_reg(uint32_t value, x64GPRegister reg);  // sub imm32, %r
        void push_reg(x64GPRegister reg);                      // push %r
        void pop_reg(x64GPRegister reg);                       // pop %r
        void cmpb_at_reg(uint8_t value, x64GPRegister reg);    // cmpb value, 0(%r)
        void jnz(int32_t relative);                            // jnz relative_address -- the same as jne
        void jz(int32_t relative);                             // jz relative_address -- the same as je
        void jnz(const std::string &label_name);               // a jnz but with a label to be backpatched later
        void jz(const std::string &label_name);                // a jz but with a label to be backpatched later
        void call_at_reg(x64GPRegister reg);                   // call %r -- indirect absolute memory addressing with the register

        void emitLabel(const std::string &label_name);
        bool resolveLabels();

    private:

        std::vector<uint8_t> m_code;
        std::vector<Label> m_src_labels;
        std::vector<Label> m_ref_labels;
    };

} //namespace jit

} //namespace bs

#endif //EMITTER_HPP