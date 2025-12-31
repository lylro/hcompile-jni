#include "thread.h"
#include <pthread.h>
#include <unistd.h>

namespace Thread {

void* ThreadFunc(void* arg) {
    void (*func)() = (void (*)())arg;
    func();
    return nullptr;
}

void Create(void (*func)()) {
    pthread_t t;
    pthread_create(&t, nullptr, ThreadFunc, (void*)func);
}

void Sleep(int ms) {
    usleep(ms * 1000);
}

}