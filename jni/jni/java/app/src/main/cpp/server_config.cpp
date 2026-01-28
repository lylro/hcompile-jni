#include <string>
#include <jni.h>

namespace server_config {
    // ЕДИНСТВЕННОЕ место во всем проекте где указан адрес сервера
    std::string SERVER_HOST = "37.46.130.204";
    int SERVER_PORT = 80;

    void set_server_config(const std::string& host, int port) {
        SERVER_HOST = host;
        SERVER_PORT = port;
    }

    std::string get_server_host() {
        return SERVER_HOST;
    }

    int get_server_port() {
        return SERVER_PORT;
    }

    std::string get_server_url() {
        return "http://" + SERVER_HOST + ":" + std::to_string(SERVER_PORT);
    }

    // Внутри namespace server_config, после других функций добавьте:

    bool send_message_reply(const std::string& username, const std::string& message,
                            const std::string& reply_to_id, const std::string& reply_to_username,
                            const std::string& reply_to_message) {
        // TODO: Реализовать отправку сообщения с ответом на сервер
        return false; // Заглушка
    }

    bool send_private_message_reply(const std::string& from_username, const std::string& to_username,
                                    const std::string& message, const std::string& reply_to_id,
                                    const std::string& reply_to_username, const std::string& reply_to_message) {
        // TODO: Реализовать отправку приватного сообщения с ответом
        return false; // Заглушка
    }
}

extern "C" {

JNIEXPORT void JNICALL Java_com_android_quikky_ChatHelper_initClient(JNIEnv *env, jobject) {
    // Инициализация клиента
}

// ИСПРАВЛЕНО: Правильные сигнатуры для приватных функций объекта ChatHelper
JNIEXPORT jstring JNICALL Java_com_android_quikky_ChatHelper_getServerHost(JNIEnv *env, jobject) {
    return env->NewStringUTF(server_config::get_server_host().c_str());
}

JNIEXPORT jint JNICALL Java_com_android_quikky_ChatHelper_getServerPort(JNIEnv *env, jobject) {
    return server_config::get_server_port();
}

JNIEXPORT void JNICALL Java_com_android_quikky_ChatHelper_updateServerConfig(JNIEnv* env, jobject, jstring serverUrl) {
    const char* url = env->GetStringUTFChars(serverUrl, nullptr);
    std::string full_url = url;
    env->ReleaseStringUTFChars(serverUrl, url);

    // Удаляем протокол если есть
    size_t protocol_pos = full_url.find("://");
    if (protocol_pos != std::string::npos) {
        full_url = full_url.substr(protocol_pos + 3);
    }

    // Удаляем путь если есть
    size_t path_pos = full_url.find("/");
    if (path_pos != std::string::npos) {
        full_url = full_url.substr(0, path_pos);
    }

    size_t port_pos = full_url.find(":");
    if (port_pos != std::string::npos) {
        std::string host = full_url.substr(0, port_pos);
        std::string port_str = full_url.substr(port_pos + 1);
        try {
            int port = std::stoi(port_str);
            server_config::set_server_config(host, port);
        } catch (...) {
            server_config::set_server_config(host, 8000);
        }
    } else {
        server_config::set_server_config(full_url, 8000);
    }
}
}