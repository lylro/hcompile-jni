#include "logger.h"
#include <android/log.h>

#define LOG_TAG "BR_PLUGIN"

namespace Logger {

void Init() {
    Log("Logger initialized");
}

void Log(const char* text) {
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", text);
}

}