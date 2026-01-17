#pragma once
#include <jni.h>
#include <vector>
#include <mutex>
#include <atomic>
#include <queue>
#include <unordered_map>
#include "offsets.h"

#define MAX_PLAYERS 1000
#define SYNC_INTERVAL_MS 33  // 30 FPS

class AdvancedPlayerSync {
public:
    static AdvancedPlayerSync& GetInstance();
    
    void Initialize(JNIEnv* env, jobject context);
    void StartSyncThread();
    void StopSyncThread();
    
    // JNI методы
    void UpdatePlayerData(int playerId, float x, float y, float z, 
                        float speedX, float speedY, float speedZ,
                        float rotation, uint32_t timestamp, bool isOnFoot);
    
    void SetLocalPlayerData(float x, float y, float z, 
                          float speedX, float speedY, float speedZ,
                          bool isOnFoot);

private:
    AdvancedPlayerSync();
    ~AdvancedPlayerSync();
    
    struct PlayerMovementData {
        int id;
        float x, y, z;
        float speedX, speedY, speedZ;
        float rotation;
        uint32_t serverTimestamp;
        uint32_t localTimestamp;
        bool isActive;
        bool isOnFoot;
        uintptr_t memoryAddress;
    };
    
    struct LocalPlayerData {
        float x, y, z;
        float speedX, speedY, speedZ;
        std::atomic<uint32_t> lastUpdate;
        bool isOnFoot;
    };
    
    void SyncThreadFunc();
    void ProcessMovement();
    void CorrectPlayerPosition(int playerId, const PlayerMovementData& serverData);
    void PredictPosition(PlayerMovementData& data, uint32_t currentTime) const;
    bool IsPositionValid(const PlayerMovementData& serverData, 
                       const PlayerMovementData& clientData) const;
    uintptr_t GetPlayerAddress(int playerId) const;
    uintptr_t GetLocalPlayerAddress() const;
    void ReadPlayerMemory(uintptr_t address, PlayerMovementData& data) const;
    void WritePlayerMemory(uintptr_t address, const PlayerMovementData& data) const;
    
    PlayerMovementData m_players[MAX_PLAYERS];
    LocalPlayerData m_localPlayer;
    std::mutex m_dataMutex;
    std::atomic<bool> m_running;
    std::thread m_syncThread;
    JNIEnv* m_env;
    jobject m_context;
    
    // Архитектурно-зависимые параметры
    struct {
        size_t playerStructSize;
        size_t positionOffset;
        size_t speedOffset;
        size_t rotationOffset;
        size_t animationOffset;
    } m_archParams;
    
    // Оффсеты
    uintptr_t m_playerPoolOffset;
};