#include "plugin.h"
#include "xorstr.h"
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <android/log.h>

#define LOG_TAG "NativePlugin"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

uintptr_t CGameAPI::m_address = 0;
std::once_flag m_address_once;
std::recursive_mutex m_address_mutex;

// Кэш оффсетов для ускорения
static std::unordered_map<std::string, uintptr_t> g_offsetCache;
static std::mutex g_cacheMutex;

bool IsValidAddress(uintptr_t address) {
    if (address < 0x1000) return false;
    
#ifdef __LP64__
    if (address > 0x7FFFFFFFFFFF) return false;
#else
    if (address > 0x7FFFFFFF) return false;
#endif
    
    if (address % 4 != 0) return false;
    return true;
}

bool ValidateLibrarySegments(struct dl_phdr_info *info) {
    if (!info || !info->dlpi_phdr) return false;
    
    for (int i = 0; i < info->dlpi_phnum; i++) {
        if (info->dlpi_phdr[i].p_type == PT_LOAD) {
            if (info->dlpi_phdr[i].p_flags & PF_X) {
                return true;
            }
        }
    }
    return false;
}

int callback(struct dl_phdr_info *info, size_t size, void *data) {
    if (!info || !info->dlpi_name) return 0;
    
    const char* name = info->dlpi_name;
    const char* basename = strrchr(name, '/');
    basename = basename ? basename + 1 : name;
    
    if (strstr(name, xorstr("libblackrussia-client.so")) || 
        strcmp(basename, xorstr("libblackrussia-client.so")) == 0) {
        
        if (ValidateLibrarySegments(info) && info->dlpi_phnum > 0) {
            std::lock_guard<std::recursive_mutex> lock(m_address_mutex);
            
            if (CGameAPI::m_address == 0) {
                CGameAPI::m_address = info->dlpi_addr;
                
                if (IsValidAddress(CGameAPI::m_address)) {
                    LOGD("Base address: 0x%lx", CGameAPI::m_address);
                    return 1;
                } else {
                    CGameAPI::m_address = 0;
                }
            }
        }
    }
    return 0;
}

uintptr_t GetCachedOffset(const std::string& offsetName) {
    std::lock_guard<std::mutex> lock(g_cacheMutex);
    
    auto it = g_offsetCache.find(offsetName);
    if (it != g_offsetCache.end()) {
        return it->second;
    }
    
    uintptr_t offset = COffset::Get(offsetName.c_str());
    if (offset != 0) {
        g_offsetCache[offsetName] = offset;
    }
    
    return offset;
}

uintptr_t CGameAPI::GetBase(const char* offsetName)
{
    std::call_once(m_address_once, []() {
        dl_iterate_phdr(callback, nullptr);
    });
    
    if (CGameAPI::m_address == 0) {
        LOGE("Base address not initialized");
        return 0;
    }
    
    if (!IsValidAddress(CGameAPI::m_address)) {
        LOGE("Invalid base address: 0x%lx", CGameAPI::m_address);
        return 0;
    }
    
    if (offsetName == nullptr) {
        return CGameAPI::m_address;
    }
    
    uintptr_t offset = GetCachedOffset(offsetName);
    
    if (offset == 0) {
        LOGW("Offset not found: %s", offsetName);
        return 0;
    }
    
    uintptr_t result = CGameAPI::m_address + offset;
    
    if (!IsValidAddress(result)) {
        LOGE("Invalid result address: 0x%lx", result);
        return 0;
    }
    
    return result;
}

std::string jbyteArrayToString(JNIEnv* env, jbyteArray byteArray) {
    if (byteArray == nullptr) return "";
    
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        return "";
    }
    
    jsize length = env->GetArrayLength(byteArray);
    if (length == 0) return "";
    
    if (length > 1024 * 1024) {
        LOGE("ByteArray too large: %d", length);
        return "";
    }
    
    jbyte* byteElements = env->GetByteArrayElements(byteArray, nullptr);
    if (byteElements == nullptr) return "";
    
    std::string result(reinterpret_cast<char*>(byteElements), length);
    env->ReleaseByteArrayElements(byteArray, byteElements, JNI_ABORT);
    
    return result;
}

const char* jbyteArrayToCharArray(JNIEnv* env, jbyteArray byteArray) {
    static thread_local std::string buffer;
    buffer = jbyteArrayToString(env, byteArray);
    return buffer.c_str();
}

void FreeJByteArrayChars(const char* str) {
    // Empty - memory managed by std::string
}

bool cp1251_to_utf8_safe(char *out, size_t out_size, const char *in, unsigned int len) {
    if (!out || !in || out_size == 0) return false;
    
    static const int table[128] = {
        0x82D0,0x83D0,0x9A80E2,0x93D1,0x9E80E2,0xA680E2,0xA080E2,0xA180E2,
        0xAC82E2,0xB080E2,0x89D0,0xB980E2,0x8AD0,0x8CD0,0x8BD0,0x8FD0,
        0x92D1,0x9880E2,0x9980E2,0x9C80E2,0x9D80E2,0xA280E2,0x9380E2,0x9480E2,
        0,0xA284E2,0x99D1,0xBA80E2,0x9AD1,0x9CD1,0x9BD1,0x9FD1,
        0xA0C2,0x8ED0,0x9ED1,0x88D0,0xA4C2,0x90D2,0xA6C2,0xA7C2,
        0x81D0,0xA9C2,0x84D0,0xABC2,0xACC2,0xADC2,0xAEC2,0x87D0,
        0xB0C2,0xB1C2,0x86D0,0x96D1,0x91D2,0xB5C2,0xB6C2,0xB7C2,
        0x91D1,0x9684E2,0x94D1,0xBBC2,0x98D1,0x85D0,0x95D1,0x97D1,
        0x90D0,0x91D0,0x92D0,0x93D0,0x94D0,0x95D0,0x96D0,0x97D0,
        0x98D0,0x99D0,0x9AD0,0x9BD0,0x9CD0,0x9DD0,0x9ED0,0x9FD0,
        0xA0D0,0xA1D0,0xA2D0,0xA3D0,0xA4D0,0xA5D0,0xA6D0,0xA7D0,
        0xA8D0,0xA9D0,0xAAD0,0xABD0,0xACD0,0xADD0,0xAED0,0xAFD0,
        0xB0D0,0xB1D0,0xB2D0,0xB3D0,0xB4D0,0xB5D0,0xB6D0,0xB7D0,
        0xB8D0,0xB9D0,0xBAD0,0xBBD0,0xBCD0,0xBDD0,0xBED0,0xBFD0,
        0x80D1,0x81D1,0x82D1,0x83D1,0x84D1,0x85D1,0x86D1,0x87D1,
        0x88D1,0x89D1,0x8AD1,0x8BD1,0x8CD1,0x8DD1,0x8ED1,0x8FD1
    };

    char *original_out = out;
    int count = 0;
    
    while (*in && count < (int)(out_size - 4)) {
        if(len && (count >= (int)len)) break;

        if (*in & 0x80) {
            int v = table[(int)(0x7f & *in++)];
            if (!v) continue;
            
            int bytes_needed = (v >> 16) ? 3 : 2;
            if (out - original_out + bytes_needed >= (int)out_size - 1) break;
            
            *out++ = (char)v;
            *out++ = (char)(v >> 8);
            if (v >>= 16) *out++ = (char)v;
        }
        else {
            if (out - original_out >= (int)out_size - 1) break;
            *out++ = *in++;
        }
        count++;
    }

    *out = '\0';
    return true;
}

void cp1251_to_utf8(char *out, const char *in, unsigned int len) {
    const size_t buffer_size = 4096;
    static thread_local char buffer[buffer_size];
    
    if (cp1251_to_utf8_safe(buffer, buffer_size, in, len)) {
        strncpy(out, buffer, buffer_size - 1);
        out[buffer_size - 1] = '\0';
    } else {
        strncpy(out, in, buffer_size - 1);
        out[buffer_size - 1] = '\0';
    }
}

JNIEnv* SafeGetJNIEnv(JavaVM* vm) {
    if (!vm) return nullptr;
    
    JNIEnv* env = nullptr;
    jint result = vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    
    if (result == JNI_EDETACHED) {
        result = vm->AttachCurrentThread(&env, nullptr);
        if (result != JNI_OK || !env) return nullptr;
    } else if (result != JNI_OK || !env) {
        return nullptr;
    }
    
    return env;
}

bool IsLibraryLoaded() {
    uintptr_t base = CGameAPI::GetBase(nullptr);
    return (base && IsValidAddress(base));
}
