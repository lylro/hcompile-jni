#include <jni.h>

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_test_NativeLib_test(JNIEnv *env, jobject thiz) {
    return 1337;
}
