#ifndef MAIN_H
#define MAIN_H

#include <jni.h>
#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <android/log.h>
#include <time.h>
#include <signal.h>
#include <ucontext.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <dirent.h>

// Типы
typedef unsigned int uint32_t;
typedef unsigned long uintptr_t;

// Константы
#define PAGE_SIZE 4096
#define CRYPT_MASK 0x0

// OBFUSCATE макрос
#define OBFUSCATE(str) str

// Структуры для RakNet
class RakClientInterface {
public:
    virtual ~RakClientInterface() {}
    virtual bool Connect(const char* host, unsigned short port, unsigned int a, unsigned int b, unsigned int c) = 0;
    virtual void Disconnect(unsigned int blockDuration, unsigned char orderingChannel = 0) = 0;
    virtual void* Receive() = 0;
    virtual void Send(const char* data, const int length, unsigned char priority, unsigned char reliability, char orderingChannel, bool broadcast) = 0;
};

class RakNetworkFactory {
public:
    static RakClientInterface* GetRakClientInterface() { return nullptr; }
};

// Структуры для пакетов
#pragma pack(1)
struct Packet {
    unsigned char data[2048];
    int length;
    unsigned char priority;
    unsigned char reliability;
    char orderingChannel;
    bool broadcast;
};

struct Address {
    char host[256];
    unsigned short port;
};
#pragma pack()

// Функции
uintptr_t FindLibrary(const char* library);
void* GetGlobalActivity(JNIEnv* env);
void WriteToLog(const char* tag, const char* fmt, ...);
uint32_t GetTickCount();

// Макросы логирования
#define LOG_INFO(fmt, ...) WriteToLog("SAMP-INFO", fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) WriteToLog("SAMP-ERROR", fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) WriteToLog("SAMP-DEBUG", fmt, ##__VA_ARGS__)
#define LOG_CONNECTION(fmt, ...) WriteToLog("SAMP-CONN", fmt, ##__VA_ARGS__)
#define LOG_PACKET(fmt, ...) WriteToLog("SAMP-PACKET", fmt, ##__VA_ARGS__)

// Класс для хуков
class CHook {
public:
    static bool InlineHook(uintptr_t lib, uint32_t offset, uintptr_t hook, uintptr_t* orig);
    static void UnFuck(uintptr_t ptr);
    static void WriteMemory(uintptr_t dest, uintptr_t src, size_t size);
    static void ReadMemory(uintptr_t dest, uintptr_t src, size_t size);
    static void NOP(uintptr_t addr, unsigned int count);
    static void MethodHook(uintptr_t lib, uint32_t offset, uintptr_t func);
};

// Внешние переменные
extern uintptr_t g_libGTASA;
extern uintptr_t g_libSAMP;
extern JavaVM* mVm;
extern JNIEnv* mEnv;

#endif