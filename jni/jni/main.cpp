#include "main.h"
#include "jniutil.h"
#include "inlinehook.h"
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <vector>
#include <string>
#include <map>

uintptr_t g_libGTASA = 0;
uintptr_t g_libSAMP = 0;
const char* g_pAPKPackage;

jobject appContext;
JavaVM *mVm;
JNIEnv *mEnv;

// Настройки сервера
#define SERVER_IP "185.189.255.97"
#define SERVER_PORT 3005

// Путь к лог-файлу
#define LOG_FILE_PATH "/storage/emulated/0/Download/samp_log.txt"

// ПРАВИЛЬНЫЕ ОФФСЕТЫ ИЗ ДАМПА
#define OFFSET_ENET_HOST_CONNECT     0x006231  // enet_host_connect
#define OFFSET_ENET_PEER_SEND        0x006276  // enet_peer_send
#define OFFSET_ENET_PACKET_CREATE    0x006263  // enet_packet_create
#define OFFSET_ENET_ADDRESS_SET_HOST 0x00621B  // enet_address_set_host
#define OFFSET_ENET_HOST_CREATE      0x0061DF  // enet_host_create
#define OFFSET_ENET_PEER_RECEIVE     0x009150  // enet_peer_receive
#define OFFSET_ENET_SOCKET_SEND      0x009222  // enet_socket_send
#define OFFSET_ENET_SOCKET_RECEIVE   0x00920E  // enet_socket_receive
#define OFFSET_ENET_PEER_DISCONNECT  0x00902D  // enet_peer_disconnect

// RPC оффсеты
#define OFFSET_RPC_INIT_GAME         0x07B058  // RPC: InitGame
#define OFFSET_RPC_CONNECTION_REJECT 0x07CDD8  // RPC: ConnectionRejected
#define OFFSET_RPC_WORLD_PLAYER_ADD  0x07B9E8  // RPC: WorldPlayerAdd
#define OFFSET_RPC_WORLD_VEHICLE_ADD 0x07C1F0  // RPC: WorldVehicleAdd
#define OFFSET_RPC_DIALOG_BOX        0x07CB58  // RPC: DialogBox
#define OFFSET_RPC_SET_PLAYER_HEALTH 0x0821D4  // RPC: ScrSetPlayerHealth
#define OFFSET_RPC_SET_PLAYER_ARMOUR 0x082274  // RPC: ScrSetPlayerArmour

// CNetGame оффсеты
#define OFFSET_CNETGAME_CONSTRUCTOR  0x1D4BC4  // из секции CNETGAME_CONSTRUCTOR
#define OFFSET_PROCESS_EVENTS        0x07AB1C  // ProcessIncomingEvent

// Мьютекс для логирования
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

// Структура для информации о сервере
struct ServerInfo {
    char ip[64];
    int port;
    uint32_t timestamp;
};

ServerInfo g_targetServer = {"188.127.241.74", 2372, 0};
ServerInfo g_lastConnection = {"", 0, 0};

// Карта для отслеживания пакетов
std::map<uint32_t, uint32_t> g_packetCount;

// Оригинальные функции
uintptr_t orig_enet_host_connect = 0;
uintptr_t orig_enet_peer_send = 0;
uintptr_t orig_enet_packet_create = 0;
uintptr_t orig_enet_address_set_host = 0;
uintptr_t orig_enet_host_create = 0;
uintptr_t orig_enet_peer_receive = 0;
uintptr_t orig_enet_socket_send = 0;
uintptr_t orig_enet_socket_receive = 0;
uintptr_t orig_enet_peer_disconnect = 0;

// Оригинальные RPC функции
uintptr_t orig_rpc_init_game = 0;
uintptr_t orig_rpc_connection_reject = 0;
uintptr_t orig_rpc_world_player_add = 0;
uintptr_t orig_rpc_world_vehicle_add = 0;

// Функция логирования
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
    
    __android_log_write(ANDROID_LOG_INFO, tag, buffer);
    
    // Создаем папку Downloads если её нет
    mkdir("/storage/emulated/0/Download", 0777);
    
    FILE* logFile = fopen(LOG_FILE_PATH, "a");
    if (logFile) {
        fprintf(logFile, "%s [%s] %s\n", timestamp, tag, buffer);
        fflush(logFile);
        fclose(logFile);
    }
    
    pthread_mutex_unlock(&log_mutex);
}

// Функция для получения названия пакета
const char* GetPacketName(uint8_t packetId) {
    static char unknown[32];
    
    switch(packetId) {
        case 0x00: return "ID_CONNECTED_PING";
        case 0x01: return "ID_UNCONNECTED_PING";
        case 0x02: return "ID_UNCONNECTED_PING_OPEN_CONNECTIONS";
        case 0x03: return "ID_CONNECTED_PONG";
        case 0x04: return "ID_DETECT_LOST_CONNECTIONS";
        case 0x05: return "ID_OPEN_CONNECTION_REQUEST_1";
        case 0x06: return "ID_OPEN_CONNECTION_REPLY_1";
        case 0x07: return "ID_OPEN_CONNECTION_REQUEST_2";
        case 0x08: return "ID_OPEN_CONNECTION_REPLY_2";
        case 0x09: return "ID_CONNECTION_REQUEST";
        case 0x10: return "ID_CONNECTION_REQUEST_ACCEPTED";
        case 0x11: return "ID_CONNECTION_ATTEMPT_FAILED";
        case 0x12: return "ID_ALREADY_CONNECTED";
        case 0x13: return "ID_NEW_INCOMING_CONNECTION";
        case 0x14: return "ID_NO_FREE_INCOMING_CONNECTIONS";
        case 0x15: return "ID_DISCONNECTION_NOTIFICATION";
        case 0x16: return "ID_CONNECTION_LOST";
        case 0x17: return "ID_CONNECTION_BANNED";
        case 0x18: return "ID_INVALID_PASSWORD";
        case 0x19: return "ID_MODIFIED_PACKET";
        case 0x1A: return "ID_TIMESTAMP";
        case 0x80: return "ID_RPC";
        case 0x81: return "ID_RPC_MAP";
        default: sprintf(unknown, "UNKNOWN_0x%02X", packetId); return unknown;
    }
}

// Функция для получения названия RPC
const char* GetRPCName(uint8_t rpcId) {
    static char unknown[32];
    
    switch(rpcId) {
        case 0x00: return "RPC_INIT_GAME";
        case 0x01: return "RPC_REQUEST_CLASS";
        case 0x02: return "RPC_WEATHER";
        case 0x03: return "RPC_REQUEST_SPAWN";
        case 0x04: return "RPC_WORLD_TIME";
        case 0x05: return "RPC_SET_TIME_EX";
        case 0x06: return "RPC_WORLD_PLAYER_ADD";
        case 0x07: return "RPC_WORLD_PLAYER_REMOVE";
        case 0x08: return "RPC_SET_CHECKPOINT";
        case 0x09: return "RPC_DISABLE_CHECKPOINT";
        case 0x0A: return "RPC_SET_RACE_CHECKPOINT";
        case 0x0B: return "RPC_DISABLE_RACE_CHECKPOINT";
        case 0x0C: return "RPC_WORLD_VEHICLE_ADD";
        case 0x0D: return "RPC_WORLD_VEHICLE_REMOVE";
        case 0x0E: return "RPC_ENTER_VEHICLE";
        case 0x0F: return "RPC_EXIT_VEHICLE";
        case 0x10: return "RPC_DIALOG_BOX";
        case 0x11: return "RPC_GAME_MODE_RESTART";
        case 0x12: return "RPC_CONNECTION_REJECTED";
        case 0x13: return "RPC_PICKUP";
        case 0x14: return "RPC_DESTROY_PICKUP";
        case 0x15: return "RPC_CREATE_3D_TEXT_LABEL";
        case 0x16: return "RPC_DELETE_3D_TEXT_LABEL";
        case 0x17: return "RPC_UPDATE_3D_TEXT_LABEL";
        case 0x18: return "RPC_SCM_EVENT";
        case 0x19: return "RPC_SCR_DISPLAY_GAME_TEXT";
        case 0x1A: return "RPC_SCR_SET_GRAVITY";
        case 0x1B: return "RPC_SCR_FORCE_SPAWN_SELECTION";
        case 0x1C: return "RPC_SCR_SET_PLAYER_POS";
        case 0x1D: return "RPC_SCR_SET_CAMERA_POS";
        case 0x1E: return "RPC_SCR_SET_CAMERA_LOOK_AT";
        case 0x1F: return "RPC_SCR_SET_PLAYER_FACING_ANGLE";
        case 0x20: return "RPC_SCR_SET_FIGHTING_STYLE";
        case 0x21: return "RPC_SCR_SET_PLAYER_SKIN";
        case 0x22: return "RPC_SCR_APPLY_PLAYER_ANIMATION";
        case 0x23: return "RPC_SCR_CLEAR_PLAYER_ANIMATIONS";
        case 0x24: return "RPC_SCR_SET_SPAWN_INFO";
        case 0x25: return "RPC_SCR_CREATE_EXPLOSION";
        case 0x26: return "RPC_SCR_SET_PLAYER_HEALTH";
        case 0x27: return "RPC_SCR_SET_PLAYER_ARMOUR";
        case 0x28: return "RPC_SCR_SET_PLAYER_COLOR";
        case 0x29: return "RPC_SCR_SET_PLAYER_NAME";
        case 0x2A: return "RPC_SCR_SET_PLAYER_POS_FIND_Z";
        case 0x2B: return "RPC_SCR_SET_PLAYER_INTERIOR";
        case 0x2C: return "RPC_SCR_SET_MAP_ICON";
        case 0x2D: return "RPC_SCR_DISABLE_MAP_ICON";
        case 0x2E: return "RPC_SCR_SET_CAMERA_BEHIND_PLAYER";
        case 0x2F: return "RPC_SCR_SET_PLAYER_SPECIAL_ACTION";
        case 0x30: return "RPC_SCR_TOGGLE_PLAYER_SPECTATING";
        case 0x31: return "RPC_SCR_SET_PLAYER_SPECTATING";
        case 0x32: return "RPC_SCR_PLAYER_SPECTATE_PLAYER";
        case 0x33: return "RPC_SCR_PLAYER_SPECTATE_VEHICLE";
        case 0x34: return "RPC_SCR_PUT_PLAYER_IN_VEHICLE";
        case 0x35: return "RPC_SCR_SET_VEHICLE_PARAMS";
        case 0x36: return "RPC_SCR_VEHICLE_PARAMS_EX";
        case 0x37: return "RPC_SCR_HAVE_SOME_MONEY";
        case 0x38: return "RPC_SCR_RESET_MONEY";
        case 0x39: return "RPC_SCR_LINK_VEHICLE";
        case 0x3A: return "RPC_SCR_REMOVE_PLAYER_FROM_VEHICLE";
        case 0x3B: return "RPC_SCR_SET_VEHICLE_HEALTH";
        case 0x3C: return "RPC_SCR_SET_VEHICLE_POS";
        case 0x3D: return "RPC_SCR_SET_VEHICLE_VELOCITY";
        case 0x3E: return "RPC_SCR_SET_PLAYER_VELOCITY";
        case 0x3F: return "RPC_SCR_NUMBER_PLATE";
        case 0x40: return "RPC_SCR_ADD_GANG_ZONE";
        case 0x41: return "RPC_SCR_REMOVE_GANG_ZONE";
        case 0x42: return "RPC_SCR_FLASH_GANG_ZONE";
        case 0x43: return "RPC_SCR_STOP_FLASH_GANG_ZONE";
        case 0x44: return "RPC_SCR_DELETE_OBJECT";
        case 0x45: return "RPC_SCR_SET_OBJECT_POS";
        case 0x46: return "RPC_ATTACH_OBJECT_TO_PLAYER";
        case 0x47: return "RPC_SCR_PLAY_SOUND";
        case 0x48: return "RPC_SCR_SET_PLAYER_WANTED_LEVEL";
        case 0x49: return "RPC_SCR_MOVE_OBJECT";
        case 0x4A: return "RPC_SCR_RESET_PLAYER_WEAPONS";
        case 0x4B: return "RPC_SCR_GIVE_PLAYER_WEAPON";
        case 0x4C: return "RPC_SET_PLAYER_AMMO";
        case 0x4D: return "RPC_TOGGLE_PLAYER_CONTROLLABLE";
        case 0x4E: return "RPC_SCR_SET_PLAYER_ATTACHED_OBJECT";
        default: sprintf(unknown, "RPC_UNKNOWN_0x%02X", rpcId); return unknown;
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
    
    usleep(100000);
}

// Хук для enet_address_set_host
int enet_address_set_host_hook(uintptr_t address, const char* host) {
    LOG_CONNECTION("[ENET] Setting host address: %s", host);
    
    // Сохраняем информацию о подключении
    strcpy(g_lastConnection.ip, host);
    g_lastConnection.port = 0;
    g_lastConnection.timestamp = GetTickCount();
    
    // Если это наш целевой сервер, меняем хост?
    // Можно оставить как есть или изменить
    
    typedef int (*orig_t)(uintptr_t, const char*);
    return ((orig_t)orig_enet_address_set_host)(address, host);
}

// Хук для enet_host_connect
uintptr_t enet_host_connect_hook(uintptr_t host, uintptr_t address, uint32_t channelCount, uint32_t data) {
    LOG_CONNECTION("[ENET] Connecting to server...");
    LOG_CONNECTION("[ENET] Channel count: %u", channelCount);
    
    // Запоминаем время подключения
    g_targetServer.timestamp = GetTickCount();
    
    typedef uintptr_t (*orig_t)(uintptr_t, uintptr_t, uint32_t, uint32_t);
    return ((orig_t)orig_enet_host_connect)(host, address, channelCount, data);
}

// Хук для enet_host_create
uintptr_t enet_host_create_hook(uintptr_t address, uint32_t maxClients, uint32_t channelLimit, uint32_t incomingBandwidth, uint32_t outgoingBandwidth) {
    LOG_CONNECTION("[ENET] Creating host - max clients: %u, channel limit: %u", maxClients, channelLimit);
    
    typedef uintptr_t (*orig_t)(uintptr_t, uint32_t, uint32_t, uint32_t, uint32_t);
    return ((orig_t)orig_enet_host_create)(address, maxClients, channelLimit, incomingBandwidth, outgoingBandwidth);
}

// Хук для enet_packet_create
uintptr_t enet_packet_create_hook(const void* data, size_t dataLength, uint32_t flags) {
    LOG_PACKET("[ENET] Creating packet - size: %zu, flags: 0x%X", dataLength, flags);
    
    // Если это первый байт пакета, показываем его тип
    if (dataLength > 0) {
        uint8_t packetId = *(uint8_t*)data;
        LOG_PACKET("[ENET] Packet type: %s (0x%02X)", GetPacketName(packetId), packetId);
    }
    
    typedef uintptr_t (*orig_t)(const void*, size_t, uint32_t);
    return ((orig_t)orig_enet_packet_create)(data, dataLength, flags);
}

// Хук для enet_peer_send
int enet_peer_send_hook(uintptr_t peer, uint8_t channelID, uintptr_t packet) {
    LOG_PACKET("[ENET] Sending packet - channel: %u", channelID);
    
    // Читаем данные пакета для анализа
    if (packet) {
        uint32_t* packetData = (uint32_t*)packet;
        uint32_t dataLength = packetData[1]; // длина данных
        uint8_t* data = (uint8_t*)packetData[3]; // указатель на данные
        
        if (data && dataLength > 0) {
            uint8_t packetId = data[0];
            LOG_PACKET("[ENET] SEND - Type: %s (0x%02X), Size: %u", 
                      GetPacketName(packetId), packetId, dataLength);
            
            // Если это RPC пакет (0x80), показываем RPC ID
            if (packetId == 0x80 && dataLength > 1) {
                uint8_t rpcId = data[1];
                LOG_PACKET("[ENET] RPC SEND - %s (0x%02X)", GetRPCName(rpcId), rpcId);
            }
            
            // Считаем статистику
            g_packetCount[packetId]++;
        }
    }
    
    typedef int (*orig_t)(uintptr_t, uint8_t, uintptr_t);
    return ((orig_t)orig_enet_peer_send)(peer, channelID, packet);
}

// Хук для enet_peer_receive
uintptr_t enet_peer_receive_hook(uintptr_t peer, uint8_t channelID, uintptr_t* packet) {
    uintptr_t result = ((uintptr_t(*)(uintptr_t, uint8_t, uintptr_t*))orig_enet_peer_receive)(peer, channelID, packet);
    
    if (result && packet && *packet) {
        uint32_t* packetData = (uint32_t*)(*packet);
        uint32_t dataLength = packetData[1];
        uint8_t* data = (uint8_t*)packetData[3];
        
        if (data && dataLength > 0) {
            uint8_t packetId = data[0];
            LOG_PACKET("[ENET] RECEIVE - Type: %s (0x%02X), Size: %u", 
                      GetPacketName(packetId), packetId, dataLength);
            
            // Если это RPC пакет
            if (packetId == 0x80 && dataLength > 1) {
                uint8_t rpcId = data[1];
                LOG_PACKET("[ENET] RPC RECEIVE - %s (0x%02X)", GetRPCName(rpcId), rpcId);
                
                // Логируем важные RPC
                switch(rpcId) {
                    case 0x00: // RPC_INIT_GAME
                        LOG_CONNECTION("*** GAME INITIALIZED - CONNECTION SUCCESSFUL ***");
                        break;
                    case 0x12: // RPC_CONNECTION_REJECTED
                        if (dataLength > 2) {
                            const char* reason = (const char*)&data[2];
                            LOG_ERROR("*** CONNECTION REJECTED: %s ***", reason);
                        }
                        break;
                }
            }
        }
    }
    
    return result;
}

// Хук для enet_socket_send
int enet_socket_send_hook(uintptr_t socket, uintptr_t address, uintptr_t* packet, uint32_t* length) {
    LOG_PACKET("[ENET] Socket sending data...");
    
    typedef int (*orig_t)(uintptr_t, uintptr_t, uintptr_t*, uint32_t*);
    return ((orig_t)orig_enet_socket_send)(socket, address, packet, length);
}

// Хук для enet_socket_receive
int enet_socket_receive_hook(uintptr_t socket, uintptr_t address, uintptr_t* packet, uint32_t* length) {
    int result = ((int(*)(uintptr_t, uintptr_t, uintptr_t*, uint32_t*))orig_enet_socket_receive)(socket, address, packet, length);
    
    if (result > 0 && length && *length > 0) {
        LOG_PACKET("[ENET] Socket received %d bytes", *length);
    }
    
    return result;
}

// Хук для enet_peer_disconnect
void enet_peer_disconnect_hook(uintptr_t peer, uint32_t data) {
    LOG_CONNECTION("[ENET] Peer disconnecting... data: 0x%X", data);
    
    typedef void (*orig_t)(uintptr_t, uint32_t);
    ((orig_t)orig_enet_peer_disconnect)(peer, data);
}

// Реализация CHook::InlineHook
bool CHook::InlineHook(uintptr_t lib, uint32_t offset, uintptr_t hook, uintptr_t* orig)
{
    uint32_t target = lib + offset + 1; // +1 для Thumb режима
    
    LOG_INFO("Hooking 0x%X (lib+0x%X)", target, offset);
    
    // Регистрируем хук через систему inlinehook
    int result = registerInlineHook(target, hook, (uint32_t**)orig);
    if (result != 0) {
        LOG_ERROR("registerInlineHook failed: %d", result);
        return false;
    }
    
    result = inlineHook(target);
    if (result != 0) {
        LOG_ERROR("inlineHook failed: %d", result);
        return false;
    }
    
    LOG_INFO("Hook installed successfully at 0x%X", target);
    return true;
}

void CHook::UnFuck(uintptr_t ptr)
{
    mprotect((void*)(ptr & 0xFFFFF000), PAGE_SIZE, PROT_READ | PROT_EXEC | PROT_WRITE);
}

void CHook::WriteMemory(uintptr_t dest, uintptr_t src, size_t size)
{
    UnFuck(dest);
    memcpy((void*)dest, (void*)src, size);
    cacheflush(dest, dest + size, 0);
}

void CHook::ReadMemory(uintptr_t dest, uintptr_t src, size_t size)
{
    UnFuck(src);
    memcpy((void*)dest, (void*)src, size);
}

void CHook::NOP(uintptr_t addr, unsigned int count)
{
    UnFuck(addr);
    for(uintptr_t ptr = addr; ptr < addr + (count * 2); ptr += 2) {
        *(uint16_t*)ptr = 0x46C0; // MOV R8, R8 (NOP)
    }
    cacheflush(addr, addr + count * 2, 0);
}

void CHook::MethodHook(uintptr_t lib, uint32_t offset, uintptr_t func)
{
    uintptr_t addr = lib + offset;
    UnFuck(addr);
    *(uintptr_t*)addr = func;
    cacheflush(addr, addr + 4, 0);
}

void initSamp() 
{
    LOG_INFO("========================================");
    LOG_INFO("[SAMP] Initializing hooks...");
    LOG_INFO("[SAMP] Library base: 0x%X", g_libSAMP);
    
    // Хуки для RakNet функций
    CHook::InlineHook(g_libSAMP, OFFSET_ENET_ADDRESS_SET_HOST, 
                     (uintptr_t)enet_address_set_host_hook, &orig_enet_address_set_host);
    
    CHook::InlineHook(g_libSAMP, OFFSET_ENET_HOST_CONNECT, 
                     (uintptr_t)enet_host_connect_hook, &orig_enet_host_connect);
    
    CHook::InlineHook(g_libSAMP, OFFSET_ENET_HOST_CREATE, 
                     (uintptr_t)enet_host_create_hook, &orig_enet_host_create);
    
    CHook::InlineHook(g_libSAMP, OFFSET_ENET_PACKET_CREATE, 
                     (uintptr_t)enet_packet_create_hook, &orig_enet_packet_create);
    
    CHook::InlineHook(g_libSAMP, OFFSET_ENET_PEER_SEND, 
                     (uintptr_t)enet_peer_send_hook, &orig_enet_peer_send);
    
    CHook::InlineHook(g_libSAMP, OFFSET_ENET_PEER_RECEIVE, 
                     (uintptr_t)enet_peer_receive_hook, &orig_enet_peer_receive);
    
    CHook::InlineHook(g_libSAMP, OFFSET_ENET_SOCKET_SEND, 
                     (uintptr_t)enet_socket_send_hook, &orig_enet_socket_send);
    
    CHook::InlineHook(g_libSAMP, OFFSET_ENET_SOCKET_RECEIVE, 
                     (uintptr_t)enet_socket_receive_hook, &orig_enet_socket_receive);
    
    CHook::InlineHook(g_libSAMP, OFFSET_ENET_PEER_DISCONNECT, 
                     (uintptr_t)enet_peer_disconnect_hook, &orig_enet_peer_disconnect);
    
    LOG_INFO("[SAMP] All hooks installed successfully");
    LOG_INFO("[SAMP] Target server: %s:%d", SERVER_IP, SERVER_PORT);
    LOG_INFO("========================================");
}

void Main() 
{
    LOG_INFO("[GTASA] Main thread started");
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
    FILE* logFile = fopen(LOG_FILE_PATH, "w");
    if (logFile) {
        time_t now;
        time(&now);
        struct tm* timeinfo = localtime(&now);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
        fprintf(logFile, "=== SAMP Packet Logger started at %s ===\n", timestamp);
        fprintf(logFile, "=== Target server: %s:%d ===\n\n", SERVER_IP, SERVER_PORT);
        fclose(logFile);
    }
    
    LOG_INFO("========================================");
    LOG_INFO("SAMP Packet Logger starting...");
    LOG_INFO("Build date: %s %s", __DATE__, __TIME__);
    LOG_INFO("========================================");
    
    // Установка обработчиков сигналов
    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    
    LOG_INFO("Signal handlers installed");

    appContext = GetGlobalActivity(mEnv);
    
    g_libGTASA = FindLibrary("libGTASA.so");
    if(g_libGTASA)
    {
        LOG_INFO("[GTASA] Found at 0x%X", g_libGTASA);
        srand(time(0));

        pthread_t thread;
        pthread_create(&thread, 0, InitialiseThread, 0);
    } else {
        LOG_ERROR("[GTASA] Library not found!");
    }

    g_libSAMP = FindLibrary("libsamp.so");
    if(g_libSAMP) 
    {
        LOG_INFO("[SAMP] Found at 0x%X", g_libSAMP);
        initSamp();
    } else {
        LOG_ERROR("[SAMP] Library not found!");
    }

    LOG_INFO("========================================");
    LOG_INFO("SAMP Packet Logger initialized successfully");
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