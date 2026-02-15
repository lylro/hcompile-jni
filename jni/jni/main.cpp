#include "main.h"
#include "jniutil.h"
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

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

// Оффсеты из дампа libsamp.so (Oct 30 2022)
#define OFFSET_CNETGAME_CONSTRUCTOR 0x07706C  // CNetGame::CNetGame constructor
#define OFFSET_INIT_GAME_RPC        0x07B058  // RPC: InitGame
#define OFFSET_CONNECTION_REJECTED  0x07CDD8  // RPC: ConnectionRejected
#define OFFSET_PROCESS_EVENTS       0x07AB1C  // ProcessIncomingEvent
#define OFFSET_AUTH_HANDLER         0x07828C  // [AUTH] handler

// Дополнительные оффсеты из дампа
#define OFFSET_WORLD_PLAYER_ADD      0x07B9E8  // RPC: WorldPlayerAdd
#define OFFSET_WORLD_PLAYER_REMOVE   0x07BA94  // RPC: WorldPlayerRemove
#define OFFSET_WORLD_VEHICLE_ADD     0x07C1F0  // RPC: WorldVehicleAdd
#define OFFSET_WORLD_VEHICLE_REMOVE  0x07C2FC  // RPC: WorldVehicleRemove
#define OFFSET_ENTER_VEHICLE         0x07C418  // RPC: EnterVehicle
#define OFFSET_EXIT_VEHICLE          0x07C4D8  // RPC: ExitVehicle
#define OFFSET_DIALOG_BOX            0x07CB58  // RPC: DialogBox
#define OFFSET_SET_PLAYER_POS        0x081890  // RPC: ScrSetPlayerPos
#define OFFSET_SET_PLAYER_HEALTH     0x0821D4  // RPC: ScrSetPlayerHealth
#define OFFSET_SET_PLAYER_ARMOUR     0x082274  // RPC: ScrSetPlayerArmour
#define OFFSET_SET_PLAYER_NAME       0x082480  // RPC: ScrSetPlayerName
#define OFFSET_SET_PLAYER_INTERIOR   0x082648  // RPC: ScrSetPlayerInterior
#define OFFSET_GIVE_PLAYER_WEAPON    0x084778  // RPC: ScrGivePlayerWeapon

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
        mkdir("/storage/emulated/0/Download", 0777); // Создаем папку если нет
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
        fprintf(logFile, "=== SAMP Log started at %s ===\n", "");
        time_t now;
        time(&now);
        struct tm* timeinfo = localtime(&now);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
        fprintf(logFile, "=== %s ===\n\n", timestamp);
        fclose(logFile);
    }
}

// Структура для хранения информации об ошибках
typedef struct {
    uintptr_t address;
    uintptr_t lr;
    uintptr_t sp;
    uintptr_t pc;
    int signum;
    char description[256];
} CrashInfo;

// Обработчик сигналов для записи краш-логов
void signal_handler(int signum, siginfo_t* info, void* context)
{
    ucontext_t* uc = (ucontext_t*)context;
    CrashInfo crash = {0};
    
    crash.signum = signum;
    crash.address = (uintptr_t)info->si_addr;
    crash.pc = uc->uc_mcontext.arm_pc;
    crash.lr = uc->uc_mcontext.arm_lr;
    crash.sp = uc->uc_mcontext.arm_sp;
    
    switch(signum) {
        case SIGSEGV:
            strcpy(crash.description, "SIGSEGV - Segmentation Fault");
            break;
        case SIGABRT:
            strcpy(crash.description, "SIGABRT - Abort");
            break;
        case SIGFPE:
            strcpy(crash.description, "SIGFPE - Floating Point Exception");
            break;
        case SIGBUS:
            strcpy(crash.description, "SIGBUS - Bus Error");
            break;
        default:
            strcpy(crash.description, "Unknown Signal");
    }
    
    LOG_ERROR("========================================");
    LOG_ERROR("CRASH DETECTED: %s", crash.description);
    LOG_ERROR("Signal: %d", signum);
    LOG_ERROR("Fault address: 0x%X", crash.address);
    LOG_ERROR("PC: 0x%X", crash.pc);
    LOG_ERROR("LR: 0x%X", crash.lr);
    LOG_ERROR("SP: 0x%X", crash.sp);
    LOG_ERROR("libGTASA base: 0x%X", g_libGTASA);
    LOG_ERROR("libSAMP base: 0x%X", g_libSAMP);
    LOG_ERROR("PC offset in libGTASA: 0x%X", crash.pc - g_libGTASA);
    LOG_ERROR("PC offset in libSAMP: 0x%X", crash.pc - g_libSAMP);
    LOG_ERROR("========================================");
    
    // Даем время на запись лога
    usleep(100000);
    
    // Передаем сигнал дальше
    struct sigaction* old_act = (struct sigaction*)signal_handler;
    if (old_act && old_act->sa_sigaction) {
        old_act->sa_sigaction(signum, info, context);
    }
}

// Установка обработчиков сигналов
void SetupSignalHandlers()
{
    struct sigaction sa;
    struct sigaction old_sa;
    
    sa.sa_sigaction = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    
    sigaction(SIGSEGV, &sa, &old_sa);
    sigaction(SIGABRT, &sa, &old_sa);
    sigaction(SIGFPE, &sa, &old_sa);
    sigaction(SIGBUS, &sa, &old_sa);
    
    LOG_INFO("Signal handlers installed");
}

// Хуки для логирования
void (*CNetGame__InitGame)(uintptr_t* a1);
void CNetGame__InitGame_hook(uintptr_t* a1)
{
    LOG_CONNECTION("[RPC] InitGame called - Connection successful!");
    LOG_CONNECTION("[RPC] Game is initializing...");
    CNetGame__InitGame(a1);
}

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
// Хук конструктора CNetGame
void (*CNetGame__CNetGame)(char*, const char*, int, const char*, int);
void CNetGame__CNetGame_hook(char* a1, const char* a2, int a3, const char* a4, int a5)
{
    LOG_CONNECTION("========================================");
    LOG_CONNECTION("[CNetGame] Constructor called");
    LOG_CONNECTION("[CNetGame] Original server: %s:%d", a2, a3);
    LOG_CONNECTION("[CNetGame] Player name: %s", a4);
    
    const char* newAddress = OBFUSCATE(SERVER_IP);
    int newPort = SERVER_PORT;
    
    LOG_CONNECTION("[CNetGame] Redirecting to: %s:%d", newAddress, newPort);
    LOG_CONNECTION("========================================");
    
    CNetGame__CNetGame(a1, newAddress, newPort, a4, a5);
}

// Хук для обработки подключения
void (*CNetGame__ProcessIncomingEvent)(uintptr_t* a1);
void CNetGame__ProcessIncomingEvent_hook(uintptr_t* a1)
{
    LOG_DEBUG("[Event] Processing incoming event");
    CNetGame__ProcessIncomingEvent(a1);
}

// Хук для ConnectionRejected
void (*CNetGame__ConnectionRejected)(uintptr_t* a1, const char* reason);
void CNetGame__ConnectionRejected_hook(uintptr_t* a1, const char* reason)
{
    LOG_ERROR("[CONNECTION REJECTED] Reason: %s", reason);
    CNetGame__ConnectionRejected(a1, reason);
}

// Заглушки для остальных хуков
void (*a)(uintptr_t*);
void a_hook(uintptr_t* param)
{
    LOG_DEBUG("[Hook A] Called");
    return;
}

void (*b)(uintptr_t*);
void b_hook(uintptr_t* param)
{
    LOG_DEBUG("[Hook B] Called");
    return;
}
#endif

void initSamp() 
{
    LOG_INFO("========================================");
    LOG_INFO("[SAMP] Initializing hooks...");
    LOG_INFO("[SAMP] Library base: 0x%X", g_libSAMP);
    
#ifdef VERSION_2
    CHook::InlineHook(g_libSAMP, 0x075B10, (uintptr_t)GetRakClientInterface_hook, (uintptr_t*)&old_GetRakClientInterface);
    LOG_INFO("[SAMP] Hooked GetRakClientInterface at 0x075B10");
#endif

#ifdef BR
    // Основной хук для подключения
    CHook::InlineHook(g_libSAMP, OFFSET_CNETGAME_CONSTRUCTOR, 
                     (uintptr_t)CNetGame__CNetGame_hook, 
                     (uintptr_t*)&CNetGame__CNetGame);
    LOG_INFO("[SAMP] Hooked CNetGame constructor at 0x%X", OFFSET_CNETGAME_CONSTRUCTOR);
    
    // Хук для InitGame RPC
    CHook::InlineHook(g_libSAMP, OFFSET_INIT_GAME_RPC, 
                     (uintptr_t)CNetGame__InitGame_hook, 
                     (uintptr_t*)&CNetGame__InitGame);
    LOG_INFO("[SAMP] Hooked InitGame RPC at 0x%X", OFFSET_INIT_GAME_RPC);
    
    // Хук для обработки событий
    CHook::InlineHook(g_libSAMP, OFFSET_PROCESS_EVENTS, 
                     (uintptr_t)CNetGame__ProcessIncomingEvent_hook, 
                     (uintptr_t*)&CNetGame__ProcessIncomingEvent);
    LOG_INFO("[SAMP] Hooked ProcessIncomingEvent at 0x%X", OFFSET_PROCESS_EVENTS);
    
    // Хук для ConnectionRejected
    CHook::InlineHook(g_libSAMP, OFFSET_CONNECTION_REJECTED, 
                     (uintptr_t)CNetGame__ConnectionRejected_hook, 
                     (uintptr_t*)&CNetGame__ConnectionRejected);
    LOG_INFO("[SAMP] Hooked ConnectionRejected at 0x%X", OFFSET_CONNECTION_REJECTED);
    
    // Исходные хуки
    CHook::InlineHook(g_libSAMP, 0x07DE34, (uintptr_t)a_hook, (uintptr_t*)&a);
    LOG_INFO("[SAMP] Hooked function at 0x07DE34");
    
    CHook::InlineHook(g_libSAMP, 0x0883D0, (uintptr_t)b_hook, (uintptr_t*)&b);
    LOG_INFO("[SAMP] Hooked function at 0x0883D0");
    
    LOG_INFO("[SAMP] All hooks initialized successfully");
    LOG_INFO("[SAMP] Target server: %s:%d", SERVER_IP, SERVER_PORT);
    LOG_INFO("========================================");
#endif
}

void Main() 
{
    LOG_INFO("[GTASA] Main thread started");
    LOG_INFO("[GTASA] Initialization complete");
    // code for GTASA
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
    
    g_libGTASA = ARMHook::getLibraryAddress(OBFUSCATE("libGTASA.so"));
    if(g_libGTASA)
    {
        LOG_INFO("[GTASA] Found at 0x%X", g_libGTASA);
        srand(time(0));

        uintptr_t memlib_start = (g_libGTASA + 0x174D4);
        uintptr_t size = 0x1234A;
        
        ARMHook::InitialiseTrampolines(memlib_start, size);

        pthread_t thread;
        pthread_create(&thread, 0, InitialiseThread, 0);
    } else {
        LOG_ERROR("[GTASA] Library not found!");
    }

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