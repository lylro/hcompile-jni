#ifndef PLAYER_SYNC_H
#define PLAYER_SYNC_H

#include <jni.h>
#include <string>
#include <unordered_map>

class PlayerSync {
public:
    PlayerSync();
    ~PlayerSync();

    void syncPlayerPosition(int playerId, float x, float y, float z);
    void syncPlayerAnimation(int playerId, const std::string& animName, float speed);
    
private:
    struct PlayerData {
        float x, y, z;
        std::string animation;
        float animSpeed;
    };

    std::unordered_map<int, PlayerData> playerStates;
};

#endif // PLAYER_SYNC_H
