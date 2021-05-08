#include "jit/Emitter.hpp"

#include <cstdio>

int main() {
    bs::jit::x86_64Emitter emitter;
    emitter.push_reg(bs::jit::r8);
    emitter.push_reg(bs::jit::rsi);
    emitter.movabs(255, bs::jit::r10);
    emitter.movabs(255, bs::jit::rax);
    emitter.mov(255, bs::jit::r13);
    emitter.mov(254, bs::jit::rdi);
    emitter.mov(bs::jit::rax, bs::jit::rbp);
    emitter.mov(bs::jit::rbp, bs::jit::r9);
    emitter.mov(bs::jit::r9, bs::jit::rcx);
    emitter.add_to_reg(10, bs::jit::r10);
    emitter.add_to_reg(0x55005500, bs::jit::rax);
    emitter.addb_at_reg(2, bs::jit::r10);
    emitter.addb_at_reg(2, bs::jit::rsi);
    emitter.sub_to_reg(2, bs::jit::r10);
    emitter.sub_to_reg(2, bs::jit::rbp);
    emitter.subb_at_reg(2, bs::jit::r10);
    emitter.subb_at_reg(2, bs::jit::rbp);
    emitter.emitLabel("yeet_2");
    emitter.inc(bs::jit::r13);
    emitter.inc(bs::jit::rcx);
    emitter.dec(bs::jit::r14);
    emitter.dec(bs::jit::rbx);
    emitter.call_at_reg(bs::jit::rbx);
    emitter.call_at_reg(bs::jit::r12);
    emitter.cmpb_at_reg(5, bs::jit::rcx);
    emitter.cmpb_at_reg(0, bs::jit::r15);
    emitter.jnz("yeet_1");
    emitter.jz("yeet_2");
    emitter.pop_reg(bs::jit::r8);
    emitter.pop_reg(bs::jit::rsi);
    emitter.emitLabel("yeet_1");
    emitter.ret();

    if(!emitter.resolveLabels()) {
        std::fprintf(stderr, "Error: Labels failed to resolve!\n");
        std::exit(-1);
    }

    std::vector<uint8_t> code = emitter.getCode();

    for(uint8_t byte : code) {
        std::printf("\\x%0.2X", byte);
    }

    std::printf("\n");
}