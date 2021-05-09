#include "jit/Emitter.hpp"
#include "jit/Runtime.hpp"
#include "jit/Platform.hpp"

#include <cstdio>
#include <iostream>

void my_print(uint8_t number) {
    printf("%i\n", number);
}

int main() {
    bs::jit::x86_64Emitter emitter;
    bs::jit::JITRuntime runtime;

    using Func = void (*)(uint8_t*);

    #if defined(PLATFORM_WINDOWS)
    bs::jit::x64GPRegister arg = bs::jit::rcx; 
    #else
    bs::jit::x64GPRegister arg = bs::jit::rdi;
    #endif

    //Truth Machine
    emitter.push_reg(bs::jit::r14);
    emitter.push_reg(bs::jit::r10);
    emitter.movabs((uint64_t)my_print, bs::jit::r10);
    emitter.mov(arg, bs::jit::r14);
    emitter.cmpb_at_reg(1, bs::jit::r14);
    emitter.jnz("end");
    emitter.emitLabel("loop");
    emitter.mov(1, arg);
    emitter.call_at_reg(bs::jit::r10);
    emitter.movabs((uint64_t)my_print, bs::jit::r10);
    emitter.cmpb_at_reg(1, bs::jit::r14);
    emitter.jz("loop");
    emitter.emitLabel("end");
    emitter.mov(0, arg);
    emitter.call_at_reg(bs::jit::r10);
    emitter.pop_reg(bs::jit::r10);
    emitter.pop_reg(bs::jit::r14);
    emitter.ret();

    if(!emitter.resolveLabels()) {
        printf("Failed to resolve labels\n");
        exit(-1);
    }

    runtime.loadCode(emitter.getCode());

    Func func = reinterpret_cast<Func>(runtime.getMemory());

    uint8_t byte = 1;
    std::cin >> byte;
    byte -= 48;

    func(&byte);

    //printf("%lu\n", value);
}