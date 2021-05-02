#include "jit/Emitter.hpp"

namespace bs {

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

    void x64Emitter::clear() {
        m_code.clear();
    }

    std::vector<uint8_t> x64Emitter::getCode() {
        return m_code;
    }

}