#include "plugin.h"
#include <android/log.h>

#define LOG_TAG "BR_PLUGIN"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

void PluginInit() {
    LOGI("PluginInit executed");
}