#include <android/log.h>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstddef>
#include <string>
#include <fstream>
#include "../security/obfuscate.h"
#include "log.h"
#include "../../common.h"

void Log::print(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int len = vsnprintf(nullptr, 0, format, args);
    va_end(args);

    if (len < 0) {
        return;
    }

    std::string result(len, '\0');

    va_start(args, format);
    vsnprintf(&result[0], len + 1, format, args);
    va_end(args);

    __android_log_write(ANDROID_LOG_INFO, OBFUSCATE("KOLSON"), result.c_str());
}

std::string lastPrintTag;
std::string lastPrintBuff;
void Log::printn(const char* tag, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int len = vsnprintf(nullptr, 0, format, args);
    va_end(args);

    if (len < 0) {
        return;
    }

    std::string result(len, '\0');

    va_start(args, format);
    vsnprintf(&result[0], len + 1, format, args);
    va_end(args);

    lastPrintTag = std::string(tag);
    lastPrintBuff = result;

    // __android_log_write(ANDROID_LOG_DEBUG, prefix.c_str(), result.c_str());
}

void Log::fprint(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int len = vsnprintf(nullptr, 0, format, args);
    va_end(args);

    if (len < 0) {
        return;
    }

    std::string result(len, '\0');

    va_start(args, format);
    vsnprintf(&result[0], len + 1, format, args);
    va_end(args);

    __android_log_write(ANDROID_LOG_INFO, OBFUSCATE("KOLSON"), result.c_str());

    // Append the log message to a file
	char buffer[255];

    sprintf(buffer, OBFUSCATE("%s/kolson_log.txt"), g_pszStorage);
    FILE* logfile = fopen(buffer, OBFUSCATE("a"));

    if (logfile != nullptr) 
    {
        fwrite(result.c_str(), sizeof(char), result.size(), logfile);
        fputs(OBFUSCATE("\n"), logfile);
        fclose(logfile);
    }
}