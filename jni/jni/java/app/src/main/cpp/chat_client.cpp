#include <jni.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "json.hpp"
#include "server_config.h"

using json = nlohmann::json;

namespace {
    const int BUFFER_SIZE = 8192;
    const int CONNECT_TIMEOUT = 10;

    int create_socket() {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) return -1;

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(server_config::get_server_port());

        if (inet_pton(AF_INET, server_config::get_server_host().c_str(), &server_addr.sin_addr) <= 0) {
            close(sockfd);
            return -1;
        }

        struct timeval timeout = {CONNECT_TIMEOUT, 0};
        setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            close(sockfd);
            return -1;
        }

        return sockfd;
    }

    std::string read_http_response(int sockfd) {
        std::string full_response;
        char buffer[BUFFER_SIZE];
        int content_length = -1;

        while (true) {
            int bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_received <= 0) break;

            buffer[bytes_received] = '\0';
            full_response.append(buffer, bytes_received);

            if (content_length == -1) {
                size_t header_end = full_response.find("\r\n\r\n");
                if (header_end != std::string::npos) {
                    std::string headers = full_response.substr(0, header_end);
                    size_t cl_pos = headers.find("Content-Length:");
                    if (cl_pos != std::string::npos) {
                        try {
                            content_length = std::stoi(headers.substr(cl_pos + 15));
                        } catch (...) {}
                    }
                }
            }

            if (content_length > 0) {
                size_t body_start = full_response.find("\r\n\r\n") + 4;
                if (full_response.length() - body_start >= content_length) break;
            }
        }

        return full_response;
    }

    std::string extract_http_body(const std::string& response) {
        size_t header_end = response.find("\r\n\r\n");
        if (header_end == std::string::npos) return "[]";

        std::string body = response.substr(header_end + 4);
        if (body.empty() || body == "[]") return "[]";

        try {
            json test_json = json::parse(body);
            return body;
        } catch (...) {
            return "[]";
        }
    }

    std::string http_request(const std::string& method, const std::string& path, const std::string& data = "") {
        int sockfd = create_socket();
        if (sockfd < 0) return method == "POST" && path == "/send" ? "[]" : "{\"status\":\"error\"}";

        std::string request = method + " " + path + " HTTP/1.1\r\n"
                                                    "Host: " + server_config::get_server_host() + ":" + std::to_string(server_config::get_server_port()) + "\r\n"
                                                                                                                                                           "Content-Type: application/json\r\n"
                                                                                                                                                           "Content-Length: " + std::to_string(data.length()) + "\r\n"
                                                                                                                                                                                                                "Connection: close\r\n\r\n" + data;

        if (send(sockfd, request.c_str(), request.length(), 0) < 0) {
            close(sockfd);
            return method == "POST" && path == "/send" ? "[]" : "{\"status\":\"error\"}";
        }

        std::string response = read_http_response(sockfd);
        close(sockfd);
        return extract_http_body(response);
    }
}

extern "C" {

JNIEXPORT jboolean JNICALL Java_com_android_quikky_ChatHelper_sendMessage(JNIEnv* env, jobject, jstring username, jstring message) {
    const char* user = env->GetStringUTFChars(username, nullptr);
    const char* msg = env->GetStringUTFChars(message, nullptr);

    json message_data = {{"username", user}, {"message", msg}};
    std::string response = http_request("POST", "/send", message_data.dump());

    env->ReleaseStringUTFChars(username, user);
    env->ReleaseStringUTFChars(message, msg);

    return (response.empty() || response == "[]") ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jstring JNICALL Java_com_android_quikky_ChatHelper_getMessages(JNIEnv* env, jobject) {
    std::string response = http_request("GET", "/messages");
    return env->NewStringUTF(response.empty() ? "[]" : response.c_str());
}

JNIEXPORT jstring JNICALL Java_com_android_quikky_NativeAuth_register(JNIEnv* env, jobject, jstring username, jstring password) {
    const char* user = env->GetStringUTFChars(username, nullptr);
    const char* pass = env->GetStringUTFChars(password, nullptr);

    json auth_data = {{"username", user}, {"password", pass}};
    std::string response = http_request("POST", "/register", auth_data.dump());

    env->ReleaseStringUTFChars(username, user);
    env->ReleaseStringUTFChars(password, pass);

    return env->NewStringUTF(response.c_str());
}

JNIEXPORT jstring JNICALL Java_com_android_quikky_NativeAuth_login(JNIEnv* env, jobject, jstring username, jstring password) {
    const char* user = env->GetStringUTFChars(username, nullptr);
    const char* pass = env->GetStringUTFChars(password, nullptr);

    json auth_data = {{"username", user}, {"password", pass}};
    std::string response = http_request("POST", "/login", auth_data.dump());

    env->ReleaseStringUTFChars(username, user);
    env->ReleaseStringUTFChars(password, pass);

    return env->NewStringUTF(response.c_str());
}

JNIEXPORT jstring JNICALL Java_com_android_quikky_ChatHelper_searchUsers(JNIEnv* env, jobject, jstring query) {
    const char* search_query = env->GetStringUTFChars(query, nullptr);
    std::string response = http_request("GET", "/users/search?query=" + std::string(search_query));
    env->ReleaseStringUTFChars(query, search_query);
    return env->NewStringUTF(response.empty() ? "[]" : response.c_str());
}

JNIEXPORT jboolean JNICALL Java_com_android_quikky_ChatHelper_sendPrivateMessage(JNIEnv* env, jobject, jstring from_username, jstring to_username, jstring message) {
    const char* from_user = env->GetStringUTFChars(from_username, nullptr);
    const char* to_user = env->GetStringUTFChars(to_username, nullptr);
    const char* msg = env->GetStringUTFChars(message, nullptr);

    json message_data = {{"from_username", from_user}, {"to_username", to_user}, {"message", msg}};
    std::string response = http_request("POST", "/send_private", message_data.dump());

    env->ReleaseStringUTFChars(from_username, from_user);
    env->ReleaseStringUTFChars(to_username, to_user);
    env->ReleaseStringUTFChars(message, msg);

    return (response.empty() || response == "[]") ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jstring JNICALL Java_com_android_quikky_ChatHelper_getPrivateMessages(JNIEnv* env, jobject, jstring username1, jstring username2) {
    const char* user1 = env->GetStringUTFChars(username1, nullptr);
    const char* user2 = env->GetStringUTFChars(username2, nullptr);

    std::string response = http_request("GET", "/private_messages?user1=" + std::string(user1) + "&user2=" + std::string(user2));

    env->ReleaseStringUTFChars(username1, user1);
    env->ReleaseStringUTFChars(username2, user2);

    return env->NewStringUTF(response.empty() ? "[]" : response.c_str());
}

// Новые функции для отправки сообщений с ответами

JNIEXPORT jboolean JNICALL Java_com_android_quikky_ChatHelper_sendMessageReply(JNIEnv* env, jobject,
                                                                               jstring username, jstring message,
                                                                               jstring reply_to_id, jstring reply_to_username,
                                                                               jstring reply_to_message) {

    const char* user = env->GetStringUTFChars(username, nullptr);
    const char* msg = env->GetStringUTFChars(message, nullptr);
    const char* reply_id = env->GetStringUTFChars(reply_to_id, nullptr);
    const char* reply_user = env->GetStringUTFChars(reply_to_username, nullptr);
    const char* reply_msg = env->GetStringUTFChars(reply_to_message, nullptr);

    json message_data = {
            {"username", user},
            {"message", msg},
            {"reply_to_id", reply_id},
            {"reply_to_username", reply_user},
            {"reply_to_message", reply_msg}
    };

    std::string response = http_request("POST", "/send_reply", message_data.dump());

    env->ReleaseStringUTFChars(username, user);
    env->ReleaseStringUTFChars(message, msg);
    env->ReleaseStringUTFChars(reply_to_id, reply_id);
    env->ReleaseStringUTFChars(reply_to_username, reply_user);
    env->ReleaseStringUTFChars(reply_to_message, reply_msg);

    // Парсим ответ чтобы проверить статус
    if (!response.empty() && response != "[]") {
        try {
            json response_json = json::parse(response);
            if (response_json.contains("status") && response_json["status"] == "success") {
                return JNI_TRUE;
            }
        } catch (...) {
            // Если парсинг не удался, возвращаем false
        }
    }

    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_com_android_quikky_ChatHelper_sendPrivateMessageReply(JNIEnv* env, jobject,
                                                                                      jstring from_username, jstring to_username,
                                                                                      jstring message, jstring reply_to_id,
                                                                                      jstring reply_to_username, jstring reply_to_message) {

    const char* from_user = env->GetStringUTFChars(from_username, nullptr);
    const char* to_user = env->GetStringUTFChars(to_username, nullptr);
    const char* msg = env->GetStringUTFChars(message, nullptr);
    const char* reply_id = env->GetStringUTFChars(reply_to_id, nullptr);
    const char* reply_user = env->GetStringUTFChars(reply_to_username, nullptr);
    const char* reply_msg = env->GetStringUTFChars(reply_to_message, nullptr);

    json message_data = {
            {"from_username", from_user},
            {"to_username", to_user},
            {"message", msg},
            {"reply_to_id", reply_id},
            {"reply_to_username", reply_user},
            {"reply_to_message", reply_msg}
    };

    std::string response = http_request("POST", "/send_private_reply", message_data.dump());

    env->ReleaseStringUTFChars(from_username, from_user);
    env->ReleaseStringUTFChars(to_username, to_user);
    env->ReleaseStringUTFChars(message, msg);
    env->ReleaseStringUTFChars(reply_to_id, reply_id);
    env->ReleaseStringUTFChars(reply_to_username, reply_user);
    env->ReleaseStringUTFChars(reply_to_message, reply_msg);

    // Парсим ответ чтобы проверить статус
    if (!response.empty() && response != "[]") {
        try {
            json response_json = json::parse(response);
            if (response_json.contains("status") && response_json["status"] == "success") {
                return JNI_TRUE;
            }
        } catch (...) {
            // Если парсинг не удался, возвращаем false
        }
    }

    return JNI_FALSE;
}

}