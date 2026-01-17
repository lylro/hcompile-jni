#include "Obfuscation.h"

Obfuscation::Obfuscation() {}

Obfuscation::~Obfuscation() {}

std::string Obfuscation::obfuscate(const std::string &input) {
    std::string output = input;
    for (size_t i = 0; i < output.size(); ++i) {
        output[i] = (output[i] ^ 0xAA) + (i % 7);
    }
    return output;
}

std::string Obfuscation::deobfuscate(const std::string &input) {
    std::string output = input;
    for (size_t i = 0; i < output.size(); ++i) {
        output[i] = (output[i] - (i % 7)) ^ 0xAA;
    }
    return output;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_yourapp_MainActivity_obfuscateString(JNIEnv *env, jobject instance, jstring input) {
    const char *nativeString = env->GetStringUTFChars(input, 0);
    Obfuscation obfuscator;
    std::string obfuscated = obfuscator.obfuscate(nativeString);
    env->ReleaseStringUTFChars(input, nativeString);
    return env->NewStringUTF(obfuscated.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_yourapp_MainActivity_deobfuscateString(JNIEnv *env, jobject instance, jstring input) {
    const char *nativeString = env->GetStringUTFChars(input, 0);
    Obfuscation obfuscator;
    std::string deobfuscated = obfuscator.deobfuscate(nativeString);
    env->ReleaseStringUTFChars(input, nativeString);
    return env->NewStringUTF(deobfuscated.c_str());
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