#ifndef JIT_RUNTIME_HPP
#define JIT_RUNTIME_HPP

#include <cstdint>
#include <vector>

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
        void protectBuffer();
        void resizeBuffer(size_t pages);

        bool m_isExecutable;
        size_t m_page_size;
        size_t m_mem_size;
        uint8_t *m_memory;
    };

} //namespace jit

} //namespace bs

#endif //JIT_RUNTIME_HPP