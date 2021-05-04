#ifndef EMITTER_HPP
#define EMITTER_HPP

#define GET_BYTE(number, byte) number >> (8 * byte) & 0xff

#include <cstdint>
#include <vector>

namespace bs {

namespace jit {

    //64-bit registers
    enum x64Register : uint8_t {
        rax = 0,
        rcx = 1,
        rdx = 2,
        rbx = 3,
        rsi = 6,
        rdi = 7,
        rsp = 4,
        rbp = 5,
        r8  = 8,
        r9  = 9,
        r10 = 10,
        r11 = 11,
        r12 = 12,
        r13 = 13,
        r14 = 14,
        r15 = 15
    };

    class x64Emitter {
    public:

        std::vector<uint8_t> getCode();
        std::size_t size();
        void clear();

        void emitBytes(std::vector<uint8_t> bytes);
        void emitU32(uint32_t value);
        void emitU64(uint64_t value);

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

        void ret();                                        // ret
        void movabs(uint64_t immediate, x64Register reg);  // movabs imm64, %r
        void inc(x64Register reg);                         // inc %r
        void dec(x64Register reg);                         // dec %r
        void addb_at_reg(uint8_t value, x64Register reg);  // addb value, 0(%r) or in intel syntax: add BYTE PTR [%r + 0x0], value
        void subb_at_reg(uint8_t value, x64Register reg);  // subb value, 0(%r)
        void add_to_reg(uint32_t value, x64Register reg);  // add imm32, %r
        void sub_to_reg(uint32_t value, x64Register reg);  // sub imm32, %r
        void push_reg(x64Register reg);                    // push %r
        void pop_reg(x64Register reg);                     // pop %r


    private:

        std::vector<uint8_t> m_code;
    };

} //namespace jit

} //namespace bs

#endif //EMITTER_HPP