#pragma once

namespace Thread {
    void Create(void (*func)());
    void Sleep(int ms);
}