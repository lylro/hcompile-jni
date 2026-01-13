#include <jni.h>
#include <android/log.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define LOG_TAG "JNI_CLIENT"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define SERVER_IP   "188.127.241.74"
#define SERVER_PORT 1158

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_launcher_Native_connect(JNIEnv *, jobject) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        LOGE("socket() error");
        return -1;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server.sin_addr) <= 0) {
        LOGE("inet_pton error");
        close(sock);
        return -2;
    }

    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        LOGE("connect() failed");
        close(sock);
        return -3;
    }

    LOGI("Connected to %s:%d", SERVER_IP, SERVER_PORT);
    return sock;
}
