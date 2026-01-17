#include "PlayerSync.h"

PlayerSync::PlayerSync() {}

PlayerSync::~PlayerSync() {}

void PlayerSync::syncPlayerPosition(int playerId, float x, float y, float z) {
    PlayerData& data = playerStates[playerId];
    data.x = x;
    data.y = y;
    data.z = z;
}

void PlayerSync::syncPlayerAnimation(int playerId, const std::string& animName, float speed) {
    PlayerData& data = playerStates[playerId];
    data.animation = animName;
    data.animSpeed = speed;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_yourapp_MainActivity_syncPlayerPosition(JNIEnv *env, jobject instance, jint playerId, jfloat x, jfloat y, jfloat z) {
    PlayerSync playerSync;
    playerSync.syncPlayerPosition(playerId, x, y, z);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_yourapp_MainActivity_syncPlayerAnimation(JNIEnv *env, jobject instance, jint playerId, jstring animName, jfloat speed) {
    const char *nativeString = env->GetStringUTFChars(animName, 0);
    PlayerSync playerSync;
    playerSync.syncPlayerAnimation(playerId, nativeString, speed);
    env->ReleaseStringUTFChars(animName, nativeString);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_yourapp_MainActivity_getArchitecture(JNIEnv *env, jobject instance) {
#ifdef __x86_64__
    return env->NewStringUTF("64-bit");
#elif __i386__
    return env->NewStringUTF("32-bit");
#else
    return env->NewStringUTF("Unknown architecture");
#endif
}