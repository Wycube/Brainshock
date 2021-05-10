#include "jit/Runtime.hpp"

#include <cassert>
#include <cstring>
#include <cstdio>

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
        m_page_size = si.dwPageSize;
        m_mem_size = m_page_size * 2;
        m_memory = (uint8_t*)VirtualAlloc(nullptr, si.dwPageSize * 2, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    #else
        long page_size = sysconf(_SC_PAGESIZE);
        m_page_size = page_size;
        m_mem_size = m_page_size * 2;
        m_memory = (uint8_t*)mmap(nullptr, page_size * 2, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    #endif
        assert(m_memory != nullptr);
    }

    void JITRuntime::freeBuffer() {
        assert(m_memory != nullptr);

    #if defined(PLATFORM_WINDOWS)
        VirtualFree(m_memory, 0, MEM_RELEASE);
    #else
        munmap(m_memory, m_mem_size);
    #endif

        m_mem_size = 0;
    }

    void JITRuntime::protectBuffer() {
    #if defined(PLATFORM_WINDOWS)
        DWORD old;
        VirtualProtect(m_memory, m_mem_size, PAGE_EXECUTE_READ, &old);
    #else
        mprotect(m_memory, m_mem_size, PROT_READ | PROT_EXEC);
    #endif

        m_isExecutable = true;
    }

    void JITRuntime::resizeBuffer(size_t pages) {
        freeBuffer();

        m_isExecutable = false;
        m_mem_size = m_page_size * pages;
    #if defined(PLATFORM_WINDOWS)
        m_memory = (uint8_t*)VirtualAlloc(nullptr, m_mem_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    #else
        m_memory = (uint8_t*)mmap(nullptr, m_mem_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    #endif
        assert(m_memory != nullptr);
    }

    void JITRuntime::loadCode(const std::vector<uint8_t> &code) {
        //Make sure code can fit into the buffer
        if(code.size() > m_mem_size) {
            //Make sure the amount of pages allocated is not truncated below the needed amount
            resizeBuffer((code.size() / m_page_size) + 1); 
        }

        memcpy(m_memory, code.data(), code.size());
        protectBuffer();
    }

    void* JITRuntime::getMemory() {
        return m_memory;
    }

    bool JITRuntime::isExecutable() {
        return m_isExecutable;
    }

} //namespace jit

} //namespace bs