#include "jit/JITInterpreter.hpp"

#include <cassert>

#include "jit/Platform.hpp"

namespace bs {

namespace jit {

    //--------------- JIT Runtime class methods ---------------//
    JITRuntime::JITRuntime() : m_isExecutable(false) {
        createBuffer();
    }

    JITRuntime::~JITRuntime() {
        freeBuffer();
    }

    //No execution permission because buffers allocated this way cannot have execution and write privelages
    //at the same time, so another function will adjust the permissions later.
    void JITRuntime::createBuffer() {
    #if defined(PLATFORM_WINDOWS)
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        m_memory = (uint8_t*)VirtualAlloc(nullptr, si.dwPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    #elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
        long page_size = sysconf(_SC_PAGESIZE);
        m_memory = (uint8_t*)mmap(nullptr, page_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    #endif
        assert(m_memory != nullptr);
    }

    void JITRuntime::freeBuffer() {
        assert(m_memory != nullptr);

        #if defined(PLATFORM_WINDOWS)
            VirtualFree(m_memory, 0, MEM_RELEASE);
        #elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
            long page_size = sysconf(_SC_PAGESIZE);
            munmap(m_memory, page_size);
        #endif
    }

    void JITRuntime::finalizeBuffer() {
        #if defined(PLATFORM_WINDOWS)
            DWORD old;
            VirtualProtect(m_memory, sizeof(*m_memory), PAGE_EXECUTE_READ, &old);
        #elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
            mprotect(m_memory, sizeof(*m_memory), PROT_READ | PROT_EXEC);
        #endif

        m_isExecutable = true;
    }

    void JITRuntime::loadCode(const std::vector<uint8_t> &code) {
        memcpy(m_memory, code.data(), code.size());
        finalizeBuffer();
    }

    bool JITRuntime::isExecutable() {
        return m_isExecutable;
    }

} //namespace jit


    //--------------- JIT Interpreter class methods ---------------//


} //namespace bs