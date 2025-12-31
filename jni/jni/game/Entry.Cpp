#include <jni.h>
#include <android/log.h>

#define LOG_TAG "BR_PLUGIN"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    LOGI("libplugin.so loaded (BR 16.50 JNI template)");
    return JNI_VERSION_1_6;
}