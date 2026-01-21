#include "../main.h"
#include "patch.h"

#include <sys/mman.h>
#include <unistd.h>
#include <cstring>

#define PAGE_ALIGN(x) ((x) & ~0xFFF)

uintptr_t CHook::mmap_start   = 0;
uintptr_t CHook::mmap_end     = 0;
uintptr_t CHook::memlib_start = 0;
uintptr_t CHook::memlib_end   = 0;

void CHook::UnFuck(uintptr_t ptr)
{
    mprotect(
        (void*)PAGE_ALIGN(ptr),
        PAGE_SIZE * 2,
        PROT_READ | PROT_WRITE | PROT_EXEC
    );
}

void CHook::ReadMemory(uintptr_t dest, uintptr_t src, size_t size)
{
    UnFuck(src);
    memcpy((void*)dest, (void*)src, size);
}

void CHook::InitHookStuff()
{
    mmap_start = (uintptr_t)mmap(
        nullptr,
        PAGE_SIZE,
        PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (mmap_start == (uintptr_t)MAP_FAILED)
        return;

    mmap_end = mmap_start + PAGE_SIZE;
}

void CHook::JMPCode(uintptr_t func, uintptr_t addr)
{
    UnFuck(func);

    uint32_t code =
        (((addr - func - 4) >> 12) & 0x7FF) |
        0xF000 |
        (((((addr - func - 4) >> 1) & 0x7FF) | 0xB800) << 16);

    memcpy((void*)func, &code, sizeof(code));
    cacheflush(func, func + sizeof(code), 0);
}

void CHook::WriteHookProc(uintptr_t addr, uintptr_t func)
{
    return;
}

void CHook::CodeInject(uintptr_t addr, uintptr_t func, int reg)
{
    return;
}