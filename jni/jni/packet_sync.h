// ============================================================================
// ICE RUSSIA PROJECT - Packet Synchronization System ❄️
// Улучшенная система синхронизации пакетов с буферизацией
// ============================================================================

#ifndef ICE_RUSSIA_PACKET_SYNC_H
#define ICE_RUSSIA_PACKET_SYNC_H

#include <atomic>
#include <mutex>
#include <queue>
#include <memory>
#include <chrono>
#include <android/log.h>

// ============================================================================
// ЛОГИРОВАНИЕ
// ============================================================================
#define LOG_TAG "❄️ ICE_PACKET"
#define LOG_PACKET_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "🧊 " __VA_ARGS__)
#define LOG_PACKET_INFO(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, "❄️ " __VA_ARGS__)
#define LOG_PACKET_WARN(...)  __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, "⚠️ " __VA_ARGS__)
#define LOG_PACKET_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "🔥 " __VA_ARGS__)

// ============================================================================
// КОНСТАНТЫ
// ============================================================================
constexpr size_t MAX_PACKET_SIZE = 4096;
constexpr size_t PACKET_BUFFER_SIZE = 128;
constexpr uint32_t PACKET_TIMEOUT_MS = 5000;

// ============================================================================
// СТРУКТУРА ПАКЕТА
// ============================================================================
struct ICEPacket {
    uint8_t data[MAX_PACKET_SIZE];
    size_t size;
    uint32_t id;
    uint8_t priority;
    std::chrono::steady_clock::time_point timestamp;
    
    ICEPacket() : size(0), id(0), priority(0) {
        timestamp = std::chrono::steady_clock::now();
    }
    
    ICEPacket(const void* packetData, size_t packetSize, uint32_t packetId = 0, uint8_t packetPriority = 0) 
        : size(packetSize), id(packetId), priority(packetPriority) {
        
        if (packetSize > MAX_PACKET_SIZE) {
            LOG_PACKET_ERROR("Packet size too large: %zu", packetSize);
            size = MAX_PACKET_SIZE;
        }
        
        if (packetData && size > 0) {
            memcpy(data, packetData, size);
        }
        
        timestamp = std::chrono::steady_clock::now();
    }
    
    // Проверка истечения времени пакета
    bool IsExpired() const {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp);
        return elapsed.count() > PACKET_TIMEOUT_MS;
    }
    
    // Возраст пакета в миллисекундах
    uint32_t GetAgeMs() const {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp);
        return static_cast<uint32_t>(elapsed.count());
    }
};

// ============================================================================
// КОМПАРАТОР ДЛЯ ПРИОРИТЕТНОЙ ОЧЕРЕДИ
// ============================================================================
struct PacketPriorityCompare {
    bool operator()(const std::shared_ptr<ICEPacket>& a, const std::shared_ptr<ICEPacket>& b) const {
        // Сначала сравниваем по приоритету
        if (a->priority != b->priority) {
            return a->priority < b->priority;  // Больший приоритет = выше в очереди
        }
        // Затем по времени (старые пакеты первыми)
        return a->timestamp > b->timestamp;
    }
};

// ============================================================================
// МЕНЕДЖЕР СИНХРОНИЗАЦИИ ПАКЕТОВ
// ============================================================================
class PacketSyncManager {
private:
    std::priority_queue<
        std::shared_ptr<ICEPacket>,
        std::vector<std::shared_ptr<ICEPacket>>,
        PacketPriorityCompare
    > m_packetQueue;
    
    std::mutex m_queueMutex;
    std::atomic<uint64_t> m_totalPackets{0};
    std::atomic<uint64_t> m_droppedPackets{0};
    std::atomic<uint64_t> m_expiredPackets{0};
    std::atomic<uint32_t> m_nextPacketId{1};
    std::atomic<bool> m_isRunning{false};
    
    // Статистика производительности
    std::atomic<uint64_t> m_totalProcessingTimeUs{0};
    std::atomic<uint64_t> m_maxProcessingTimeUs{0};
    
public:
    PacketSyncManager() {
        LOG_PACKET_INFO("PacketSyncManager initialized");
    }
    
    ~PacketSyncManager() {
        Cleanup();
    }
    
    // ========================================================================
    // ДОБАВЛЕНИЕ ПАКЕТА В ОЧЕРЕДЬ
    // ========================================================================
    bool EnqueuePacket(const void* data, size_t size, uint8_t priority = 0) {
        if (!data || size == 0 || size > MAX_PACKET_SIZE) {
            LOG_PACKET_ERROR("Invalid packet data or size: %zu", size);
            return false;
        }
        
        std::lock_guard<std::mutex> lock(m_queueMutex);
        
        // Проверяем переполнение очереди
        if (m_packetQueue.size() >= PACKET_BUFFER_SIZE) {
            LOG_PACKET_WARN("Packet queue full, dropping oldest low-priority packet");
            
            // Удаляем пакеты с низким приоритетом
            if (!DropLowPriorityPacket()) {
                m_droppedPackets++;
                return false;
            }
        }
        
        // Создаём новый пакет
        uint32_t packetId = m_nextPacketId++;
        auto packet = std::make_shared<ICEPacket>(data, size, packetId, priority);
        
        m_packetQueue.push(packet);
        m_totalPackets++;
        
        LOG_PACKET_DEBUG("Enqueued packet #%u (priority: %u, size: %zu)", 
                        packetId, priority, size);
        
        return true;
    }
    
    // ========================================================================
    // ИЗВЛЕЧЕНИЕ ПАКЕТА ИЗ ОЧЕРЕДИ
    // ========================================================================
    std::shared_ptr<ICEPacket> DequeuePacket() {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        
        if (m_packetQueue.empty()) {
            return nullptr;
        }
        
        // Проверяем и удаляем устаревшие пакеты
        while (!m_packetQueue.empty()) {
            auto packet = m_packetQueue.top();
            
            if (packet->IsExpired()) {
                LOG_PACKET_WARN("Packet #%u expired (age: %ums)", 
                               packet->id, packet->GetAgeMs());
                m_packetQueue.pop();
                m_expiredPackets++;
                continue;
            }
            
            m_packetQueue.pop();
            
            LOG_PACKET_DEBUG("Dequeued packet #%u (age: %ums)", 
                            packet->id, packet->GetAgeMs());
            
            return packet;
        }
        
        return nullptr;
    }
    
    // ========================================================================
    // ОБРАБОТКА ПАКЕТА С ЗАМЕРОМ ВРЕМЕНИ
    // ========================================================================
    template<typename Handler>
    bool ProcessPacket(Handler&& handler) {
        auto packet = DequeuePacket();
        if (!packet) {
            return false;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            handler(packet);
        } catch (...) {
            LOG_PACKET_ERROR("Exception during packet processing #%u", packet->id);
            return false;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        m_totalProcessingTimeUs += duration.count();
        
        uint64_t currentMaxTime = m_maxProcessingTimeUs.load();
        while (duration.count() > currentMaxTime) {
            if (m_maxProcessingTimeUs.compare_exchange_weak(currentMaxTime, duration.count())) {
                break;
            }
        }
        
        return true;
    }
    
    // ========================================================================
    // УДАЛЕНИЕ ПАКЕТОВ С НИЗКИМ ПРИОРИТЕТОМ
    // ========================================================================
    bool DropLowPriorityPacket() {
        if (m_packetQueue.empty()) {
            return false;
        }
        
        // Временно извлекаем все пакеты для поиска минимального приоритета
        std::vector<std::shared_ptr<ICEPacket>> temp;
        uint8_t minPriority = 255;
        
        while (!m_packetQueue.empty()) {
            auto packet = m_packetQueue.top();
            m_packetQueue.pop();
            
            if (packet->priority < minPriority) {
                minPriority = packet->priority;
            }
            
            temp.push_back(packet);
        }
        
        // Возвращаем пакеты обратно, кроме первого с минимальным приоритетом
        bool dropped = false;
        for (auto& packet : temp) {
            if (!dropped && packet->priority == minPriority) {
                LOG_PACKET_WARN("Dropped low-priority packet #%u (priority: %u)", 
                               packet->id, packet->priority);
                m_droppedPackets++;
                dropped = true;
                continue;
            }
            m_packetQueue.push(packet);
        }
        
        return dropped;
    }
    
    // ========================================================================
    // ОЧИСТКА УСТАРЕВШИХ ПАКЕТОВ
    // ========================================================================
    void CleanupExpiredPackets() {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        
        std::vector<std::shared_ptr<ICEPacket>> validPackets;
        
        while (!m_packetQueue.empty()) {
            auto packet = m_packetQueue.top();
            m_packetQueue.pop();
            
            if (!packet->IsExpired()) {
                validPackets.push_back(packet);
            } else {
                m_expiredPackets++;
                LOG_PACKET_DEBUG("Cleaned up expired packet #%u", packet->id);
            }
        }
        
        for (auto& packet : validPackets) {
            m_packetQueue.push(packet);
        }
    }
    
    // ========================================================================
    // ПОЛУЧЕНИЕ СТАТИСТИКИ
    // ========================================================================
    void PrintStatistics() {
        LOG_PACKET_INFO("═══════════════════════════════════════");
        LOG_PACKET_INFO("❄️  PACKET SYNC STATISTICS  ❄️");
        LOG_PACKET_INFO("═══════════════════════════════════════");
        LOG_PACKET_INFO("Total packets: %llu", m_totalPackets.load());
        LOG_PACKET_INFO("Dropped packets: %llu", m_droppedPackets.load());
        LOG_PACKET_INFO("Expired packets: %llu", m_expiredPackets.load());
        LOG_PACKET_INFO("Queue size: %zu", GetQueueSize());
        
        uint64_t totalProcessed = m_totalPackets - m_droppedPackets - m_expiredPackets;
        if (totalProcessed > 0) {
            uint64_t avgTimeUs = m_totalProcessingTimeUs / totalProcessed;
            LOG_PACKET_INFO("Avg processing time: %llu μs", avgTimeUs);
            LOG_PACKET_INFO("Max processing time: %llu μs", m_maxProcessingTimeUs.load());
        }
        
        double dropRate = (double)m_droppedPackets * 100.0 / (m_totalPackets > 0 ? m_totalPackets : 1);
        LOG_PACKET_INFO("Drop rate: %.2f%%", dropRate);
        
        LOG_PACKET_INFO("═══════════════════════════════════════");
    }
    
    // ========================================================================
    // ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
    // ========================================================================
    size_t GetQueueSize() const {
        return m_packetQueue.size();
    }
    
    bool IsEmpty() const {
        return m_packetQueue.empty();
    }
    
    void Clear() {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        while (!m_packetQueue.empty()) {
            m_packetQueue.pop();
        }
        LOG_PACKET_INFO("Packet queue cleared");
    }
    
    void Cleanup() {
        if (m_isRunning.exchange(false)) {
            PrintStatistics();
            Clear();
            LOG_PACKET_INFO("PacketSyncManager cleaned up");
        }
    }
    
    void Start() {
        m_isRunning = true;
        LOG_PACKET_INFO("PacketSyncManager started");
    }
    
    void Stop() {
        m_isRunning = false;
        LOG_PACKET_INFO("PacketSyncManager stopped");
    }
    
    bool IsRunning() const {
        return m_isRunning;
    }
};

// ============================================================================
// ГЛОБАЛЬНЫЙ ЭКЗЕМПЛЯР
// ============================================================================
extern PacketSyncManager g_packetSyncManager;

#endif // ICE_RUSSIA_PACKET_SYNC_H
