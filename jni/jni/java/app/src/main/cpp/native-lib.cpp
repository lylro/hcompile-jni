#include <jni.h>
#include <iostream>

extern "C" JNIEXPORT jstring JNICALL
Java_com_android_quikky_MainActivity_stringFromJNI(JNIEnv* env, jobject) {
    return env->NewStringUTF("");
}