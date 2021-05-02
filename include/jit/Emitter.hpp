#ifndef EMITTER_HPP
#define EMITTER_HPP

#define GET_BYTE(number, byte) number >> (8 * byte) & 0xff

#include <cstdint>
#include <vector>

namespace bs {

    class x64Emitter {
    public:

        void emitBytes(std::vector<uint8_t> bytes);
        void emitU32(uint32_t value);
        void emitU64(uint64_t value);
        void clear();

        template<typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
        void emitInt(T value) {
            int num_bytes = sizeof(T);

            for(size_t i = 0; i < num_bytes; i++) {
                m_code.push_back(GET_BYTE(value, i));
            }
        }

        std::vector<uint8_t> getCode();

    private:

        std::vector<uint8_t> m_code;
    };

}

#endif //EMITTER_HPP