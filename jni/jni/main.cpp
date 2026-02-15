#include "main.h"
#include "jniutil.h"
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <ucontext.h>

uintptr_t g_libGTASA = 0;
uintptr_t g_libSAMP = 0;
const char* g_pAPKPackage;

jobject appContext;
JavaVM *mVm;
JNIEnv *mEnv;

// Настройки сервера
#define SERVER_IP "185.189.255.97"
#define SERVER_PORT 3005

// Путь к лог-файлу в папке Downloads
#define LOG_FILE_PATH "/storage/emulated/0/Download/samp_log.txt"

// ** НОВЫЕ ОФФСЕТЫ ИЗ ДАМПА **
#define OFFSET_CNETGAME_CONSTRUCTOR 0x07706C  // CNetGame::CNetGame (из вашего кода)
#define OFFSET_INIT_GAME_RPC        0x07B058  // RPC: InitGame
#define OFFSET_PROCESS_EVENTS       0x07AB1C  // ProcessIncomingEvent
#define OFFSET_CONNECTION_REJECTED  0x07CDD8  // RPC: ConnectionRejected
#define OFFSET_WORLD_PLAYER_ADD     0x07B9E8  // RPC: WorldPlayerAdd
#define OFFSET_SET_PLAYER_POS       0x081890  // RPC: ScrSetPlayerPos

// Мьютекс для синхронизации записи в лог
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

// Функция для записи в лог-файл и в logcat одновременно
void WriteToLog(const char* tag, const char* fmt, ...)
{
    pthread_mutex_lock(&log_mutex);
    
    char buffer[4096];
    char timestamp[64];
    time_t now;
    struct tm* timeinfo;
    
    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", timeinfo);
    
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    // Запись в logcat Android
    __android_log_write(ANDROID_LOG_INFO, tag, buffer);
    
    // Запись в файл в папке Downloads
    FILE* logFile = fopen(LOG_FILE_PATH, "a");
    if (logFile) {
        fprintf(logFile, "%s [%s] %s\n", timestamp, tag, buffer);
        fflush(logFile);
        fclose(logFile);
    } else {
        // Если не удалось открыть файл, попробуем создать директорию
        mkdir("/storage/emulated/0/Download", 0777);
        logFile = fopen(LOG_FILE_PATH, "a");
        if (logFile) {
            fprintf(logFile, "%s [%s] %s\n", timestamp, tag, buffer);
            fflush(logFile);
            fclose(logFile);
        }
    }
    
    pthread_mutex_unlock(&log_mutex);
}

// Макросы для удобного логирования
#define LOG_INFO(fmt, ...) WriteToLog("SAMP-INFO", fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) WriteToLog("SAMP-ERROR", fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) WriteToLog("SAMP-DEBUG", fmt, ##__VA_ARGS__)
#define LOG_CONNECTION(fmt, ...) WriteToLog("SAMP-CONN", fmt, ##__VA_ARGS__)

// Функция для очистки лог-файла при запуске
void ClearLogFile()
{
    FILE* logFile = fopen(LOG_FILE_PATH, "w");
    if (logFile) {
        time_t now;
        time(&now);
        struct tm* timeinfo = localtime(&now);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
        
        fprintf(logFile, "=== SAMP Log started at %s ===\n\n", timestamp);
        fclose(logFile);
    }
}

// Обработчик сигналов
void signal_handler(int signum, siginfo_t* info, void* context)
{
    ucontext_t* uc = (ucontext_t*)context;
    
    const char* sig_desc = "Unknown";
    switch(signum) {
        case SIGSEGV: sig_desc = "SIGSEGV - Segmentation Fault"; break;
        case SIGABRT: sig_desc = "SIGABRT - Abort"; break;
        case SIGFPE: sig_desc = "SIGFPE - Floating Point Exception"; break;
        case SIGBUS: sig_desc = "SIGBUS - Bus Error"; break;
    }
    
    LOG_ERROR("========================================");
    LOG_ERROR("CRASH DETECTED: %s", sig_desc);
    LOG_ERROR("Signal: %d", signum);
    LOG_ERROR("Fault address: 0x%X", info->si_addr);
    LOG_ERROR("PC: 0x%X", uc->uc_mcontext.arm_pc);
    LOG_ERROR("LR: 0x%X", uc->uc_mcontext.arm_lr);
    LOG_ERROR("SP: 0x%X", uc->uc_mcontext.arm_sp);
    LOG_ERROR("libGTASA base: 0x%X", g_libGTASA);
    LOG_ERROR("libSAMP base: 0x%X", g_libSAMP);
    LOG_ERROR("PC offset in libGTASA: 0x%X", uc->uc_mcontext.arm_pc - g_libGTASA);
    LOG_ERROR("PC offset in libSAMP: 0x%X", uc->uc_mcontext.arm_pc - g_libSAMP);
    LOG_ERROR("========================================");
}

// Установка обработчиков сигналов
void SetupSignalHandlers()
{
    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    
    LOG_INFO("Signal handlers installed");
}

// ХУКИ
#ifdef VERSION_2
RakClientInterface* myInterface;

RakClientInterface* (*old_GetRakClientInterface)();
RakClientInterface* GetRakClientInterface_hook()
{
    LOG_INFO("[RakClient] Creating RakClient instance");
    myInterface = RakNetworkFactory::GetRakClientInterface();
    
    LOG_CONNECTION("[RakClient] Connecting to %s:%d", SERVER_IP, SERVER_PORT);
    myInterface->Connect(OBFUSCATE(SERVER_IP), SERVER_PORT, 0, 0, 5);
    
    return myInterface;
}
#endif

#ifdef BR
// Основной хук конструктора CNetGame
void (*CNetGame__CNetGame)(char*, const char*, int, const char*, int);
void CNetGame__CNetGame_hook(char* a1, const char* a2, int a3, const char* a4, int a5)
{
    LOG_CONNECTION("========================================");
    LOG_CONNECTION("[CNetGame] Constructor called");
    LOG_CONNECTION("[CNetGame] Original server: %s:%d", a2, a3);
    LOG_CONNECTION("[CNetGame] Player name: %s", a4);
    
    // Перенаправляем на наш сервер
    const char* newAddress = OBFUSCATE(SERVER_IP);
    int newPort = SERVER_PORT;
    
    LOG_CONNECTION("[CNetGame] Redirecting to: %s:%d", newAddress, newPort);
    LOG_CONNECTION("========================================");
    
    // Вызываем оригинальную функцию
    CNetGame__CNetGame(a1, newAddress, newPort, a4, a5);
}

// Хук для InitGame RPC
void (*CNetGame__InitGame)(uintptr_t* a1);
void CNetGame__InitGame_hook(uintptr_t* a1)
{
    LOG_CONNECTION("[RPC] InitGame called - Connection successful!");
    LOG_CONNECTION("[RPC] Game is initializing...");
    CNetGame__InitGame(a1);
}

// Хук для обработки событий
void (*CNetGame__ProcessEvents)(uintptr_t* a1);
void CNetGame__ProcessEvents_hook(uintptr_t* a1)
{
    LOG_DEBUG("[Event] Processing incoming event");
    CNetGame__ProcessEvents(a1);
}

// Хук для ConnectionRejected
void (*CNetGame__ConnectionRejected)(uintptr_t* a1, const char* reason);
void CNetGame__ConnectionRejected_hook(uintptr_t* a1, const char* reason)
{
    LOG_ERROR("[CONNECTION REJECTED] Reason: %s", reason);
    CNetGame__ConnectionRejected(a1, reason);
}

// Заглушки
void (*a)(uintptr_t*);
void a_hook(uintptr_t* param) { return; }

void (*b)(uintptr_t*);
void b_hook(uintptr_t* param) { return; }
#endif

void initSamp() 
{
    LOG_INFO("========================================");
    LOG_INFO("[SAMP] Initializing hooks...");
    LOG_INFO("[SAMP] Library base: 0x%X", g_libSAMP);
    
#ifdef VERSION_2
    if(CHook::InlineHook(g_libSAMP, 0x075B10, (uintptr_t)GetRakClientInterface_hook, (uintptr_t*)&old_GetRakClientInterface))
        LOG_INFO("[SAMP] ✓ Hooked GetRakClientInterface at 0x075B10");
    else
        LOG_ERROR("[SAMP] ✗ Failed to hook GetRakClientInterface");
#endif

#ifdef BR
    // Основной хук для подключения
    if(CHook::InlineHook(g_libSAMP, OFFSET_CNETGAME_CONSTRUCTOR, 
                         (uintptr_t)CNetGame__CNetGame_hook, 
                         (uintptr_t*)&CNetGame__CNetGame))
        LOG_INFO("[SAMP] ✓ Hooked CNetGame constructor at 0x%X", OFFSET_CNETGAME_CONSTRUCTOR);
    else
        LOG_ERROR("[SAMP] ✗ Failed to hook CNetGame constructor");
    
    // Хук для InitGame RPC
    if(CHook::InlineHook(g_libSAMP, OFFSET_INIT_GAME_RPC, 
                         (uintptr_t)CNetGame__InitGame_hook, 
                         (uintptr_t*)&CNetGame__InitGame))
        LOG_INFO("[SAMP] ✓ Hooked InitGame RPC at 0x%X", OFFSET_INIT_GAME_RPC);
    else
        LOG_ERROR("[SAMP] ✗ Failed to hook InitGame RPC");
    
    // Хук для обработки событий
    if(CHook::InlineHook(g_libSAMP, OFFSET_PROCESS_EVENTS, 
                         (uintptr_t)CNetGame__ProcessEvents_hook, 
                         (uintptr_t*)&CNetGame__ProcessEvents))
        LOG_INFO("[SAMP] ✓ Hooked ProcessEvents at 0x%X", OFFSET_PROCESS_EVENTS);
    else
        LOG_ERROR("[SAMP] ✗ Failed to hook ProcessEvents");
    
    // Хук для ConnectionRejected
    if(CHook::InlineHook(g_libSAMP, OFFSET_CONNECTION_REJECTED, 
                         (uintptr_t)CNetGame__ConnectionRejected_hook, 
                         (uintptr_t*)&CNetGame__ConnectionRejected))
        LOG_INFO("[SAMP] ✓ Hooked ConnectionRejected at 0x%X", OFFSET_CONNECTION_REJECTED);
    else
        LOG_ERROR("[SAMP] ✗ Failed to hook ConnectionRejected");
    
    // Исходные хуки из вашего кода
    if(CHook::InlineHook(g_libSAMP, 0x07DE34, (uintptr_t)a_hook, (uintptr_t*)&a))
        LOG_INFO("[SAMP] ✓ Hooked function at 0x07DE34");
    else
        LOG_ERROR("[SAMP] ✗ Failed to hook 0x07DE34");
    
    if(CHook::InlineHook(g_libSAMP, 0x0883D0, (uintptr_t)b_hook, (uintptr_t*)&b))
        LOG_INFO("[SAMP] ✓ Hooked function at 0x0883D0");
    else
        LOG_ERROR("[SAMP] ✗ Failed to hook 0x0883D0");
    
    LOG_INFO("[SAMP] Target server: %s:%d", SERVER_IP, SERVER_PORT);
    LOG_INFO("========================================");
#endif
}

void Main() 
{
    LOG_INFO("[GTASA] Main thread started");
    LOG_INFO("[GTASA] Game is running...");
}

void *InitialiseThread(void *p)
{
    LOG_INFO("[GTASA] Initialization thread started");
    Main();
    pthread_exit(0);
    return 0;
}          

JNIEnv *getEnv() 
{
    JNIEnv* env = 0;
    JavaVM* javaVM = mVm;
    int getEnvStat = javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);

    if (getEnvStat == JNI_EDETACHED)
        if (javaVM->AttachCurrentThread(&env, 0) != 0)
            return 0;

    if (getEnvStat == JNI_EVERSION)
        return 0;

    if (getEnvStat == JNI_ERR)
        return 0;

    return env;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    mVm = vm;
    mEnv = getEnv();

    // Очищаем лог-файл при запуске
    ClearLogFile();
    
    LOG_INFO("========================================");
    LOG_INFO("SAMP Mod starting...");
    LOG_INFO("Build date: %s %s", __DATE__, __TIME__);
    LOG_INFO("========================================");
    
    // Устанавливаем обработчики сигналов
    SetupSignalHandlers();

    appContext = GetGlobalActivity(mEnv);
    
    // Ищем libGTASA.so
    g_libGTASA = ARMHook::getLibraryAddress(OBFUSCATE("libGTASA.so"));
    if(g_libGTASA)
    {
        LOG_INFO("[GTASA] Found at 0x%X", g_libGTASA);
        srand(time(0));

        // Инициализация трамплинов
        uintptr_t memlib_start = (g_libGTASA + 0x174D4);
        uintptr_t size = 0x1234A;
        
        ARMHook::InitialiseTrampolines(memlib_start, size);

        // Создаем поток инициализации
        pthread_t thread;
        pthread_create(&thread, 0, InitialiseThread, 0);
    } else {
        LOG_ERROR("[GTASA] Library not found!");
    }

    // Ищем libsamp.so
    g_libSAMP = FindLibrary(OBFUSCATE("libsamp.so"));
    if(g_libSAMP) 
    {
        LOG_INFO("[SAMP] Found at 0x%X", g_libSAMP);
        initSamp();
    } else {
        LOG_ERROR("[SAMP] Library not found!");
    }

    LOG_INFO("========================================");
    LOG_INFO("SAMP Mod initialized successfully");
    LOG_INFO("Log file: %s", LOG_FILE_PATH);
    LOG_INFO("========================================");

    return JNI_VERSION_1_6;
}

uint32_t GetTickCount()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}