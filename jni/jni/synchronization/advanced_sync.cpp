#include "advanced_sync.h"
#include <chrono>
#include <cmath>
#include <unistd.h>

using namespace std::chrono;

AdvancedPlayerSync& AdvancedPlayerSync::GetInstance() {
    static AdvancedPlayerSync instance;
    return instance;
}

AdvancedPlayerSync::AdvancedPlayerSync() : m_running(false) {
    // Инициализация параметров для разных архитектур
    auto& offsetMgr = AdvancedOffsetSystem::OffsetManager::GetInstance();
    offsetMgr.Initialize();
    
    if (offsetMgr.Is64Bit()) {
        // 64-битные параметры
        m_archParams.playerStructSize = 0x2C0;
        m_archParams.positionOffset = 0x30;
        m_archParams.speedOffset = 0x40;
        m_archParams.rotationOffset = 0x58;
        m_archParams.animationOffset = 0x1A4;
    } else {
        // 32-битные параметры
        m_archParams.playerStructSize = 0x198;
        m_archParams.positionOffset = 0x20;
        m_archParams.speedOffset = 0x30;
        m_archParams.rotationOffset = 0x48;
        m_archParams.animationOffset = 0x124;
    }
    
    m_playerPoolOffset = offsetMgr.Get("CNetGame_m_pPlayerPool");
}

void AdvancedPlayerSync::Initialize(JNIEnv* env, jobject context) {
    m_env = env;
    m_context = context;
    
    for (auto& player : m_players) {
        player.isActive = false;
        player.memoryAddress = 0;
    }
    
    StartSyncThread();
}

void AdvancedPlayerSync::StartSyncThread() {
    m_running = true;
    m_syncThread = std::thread(&AdvancedPlayerSync::SyncThreadFunc, this);
}

void AdvancedPlayerSync::SyncThreadFunc() {
    while (m_running) {
        auto start = steady_clock::now();
        
        {
            std::lock_guard<std::mutex> lock(m_dataMutex);
            ProcessMovement();
        }
        
        auto end = steady_clock::now();
        auto elapsed = duration_cast<milliseconds>(end - start).count();
        
        if (elapsed < SYNC_INTERVAL_MS) {
            usleep((SYNC_INTERVAL_MS - elapsed) * 1000);
        }
    }
}

uintptr_t AdvancedPlayerSync::GetPlayerAddress(int playerId) const {
    if (m_playerPoolOffset == 0) return 0;
    
    uintptr_t playerList = 0;
    // Чтение указателя на список игроков
    // Реализация зависит от платформы (32/64 бита)
    
    if (playerList == 0) return 0;
    
    return playerList + (playerId * m_archParams.playerStructSize);
}

void AdvancedPlayerSync::ReadPlayerMemory(uintptr_t address, PlayerMovementData& data) const {
    if (address == 0) return;
    
    // Чтение позиции
    data.x = *reinterpret_cast<float*>(address + m_archParams.positionOffset);
    data.y = *reinterpret_cast<float*>(address + m_archParams.positionOffset + 4);
    data.z = *reinterpret_cast<float*>(address + m_archParams.positionOffset + 8);
    
    // Чтение скорости
    data.speedX = *reinterpret_cast<float*>(address + m_archParams.speedOffset);
    data.speedY = *reinterpret_cast<float*>(address + m_archParams.speedOffset + 4);
    data.speedZ = *reinterpret_cast<float*>(address + m_archParams.speedOffset + 8);
    
    // Чтение поворота
    data.rotation = *reinterpret_cast<float*>(address + m_archParams.rotationOffset);
}

void AdvancedPlayerSync::ProcessMovement() {
    uint32_t currentTime = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()).count();
    
    // Обновление локального игрока
    uintptr_t localPlayerAddr = GetLocalPlayerAddress();
    if (localPlayerAddr) {
        PlayerMovementData localData;
        ReadPlayerMemory(localPlayerAddr, localData);
        
        // Сохраняем данные для предсказания
        m_localPlayer.x = localData.x;
        m_localPlayer.y = localData.y;
        m_localPlayer.z = localData.z;
        m_localPlayer.speedX = localData.speedX;
        m_localPlayer.speedY = localData.speedY;
        m_localPlayer.speedZ = localData.speedZ;
        m_localPlayer.lastUpdate = currentTime;
    }
    
    // Обработка других игроков
    for (auto& player : m_players) {
        if (!player.isActive) continue;
        
        // Получаем адрес в памяти, если ещё не получили
        if (player.memoryAddress == 0) {
            player.memoryAddress = GetPlayerAddress(player.id);
            if (player.memoryAddress == 0) continue;
        }
        
        PlayerMovementData predicted = player;
        PredictPosition(predicted, currentTime);
        
        PlayerMovementData clientData;
        ReadPlayerMemory(player.memoryAddress, clientData);
        
        if (!IsPositionValid(predicted, clientData)) {
            CorrectPlayerPosition(player.id, predicted);
        }
    }
}

void AdvancedPlayerSync::CorrectPlayerPosition(int playerId, const PlayerMovementData& serverData) {
    uintptr_t address = m_players[playerId].memoryAddress;
    if (address == 0) return;
    
    // Плавная коррекция позиции
    PlayerMovementData current;
    ReadPlayerMemory(address, current);
    
    // Интерполяция
    float blendFactor = 0.3f; // Коэффициент плавности
    
    PlayerMovementData corrected;
    corrected.x = current.x * (1 - blendFactor) + serverData.x * blendFactor;
    corrected.y = current.y * (1 - blendFactor) + serverData.y * blendFactor;
    corrected.z = current.z * (1 - blendFactor) + serverData.z * blendFactor;
    
    // Запись в память
    *reinterpret_cast<float*>(address + m_archParams.positionOffset) = corrected.x;
    *reinterpret_cast<float*>(address + m_archParams.positionOffset + 4) = corrected.y;
    *reinterpret_cast<float*>(address + m_archParams.positionOffset + 8) = corrected.z;
    
    // Если игрок стоит, сбрасываем анимацию бега
    if (serverData.speedX == 0 && serverData.speedY == 0 && serverData.speedZ == 0) {
        *reinterpret_cast<int*>(address + m_archParams.animationOffset) = ANIM_STAND;
    }
}