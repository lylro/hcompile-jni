package com.android.quikky

import com.squareup.moshi.Moshi
import com.squareup.moshi.kotlin.reflect.KotlinJsonAdapterFactory
import java.net.HttpURLConnection
import java.net.URL

object ChatHelper {
    private var isInitialized = false

    private fun ensureInitialized() {
        if (!isInitialized) {
            try {
                System.loadLibrary("messenger")
                initClient()
                isInitialized = true
            } catch (e: UnsatisfiedLinkError) {
                e.printStackTrace()
                throw RuntimeException("Failed to load native library", e)
            }
        }
    }

    private external fun initClient()
    external fun updateServerConfig(serverUrl: String)
    external fun sendMessage(username: String, message: String): Boolean
    external fun getMessages(): String
    external fun searchUsers(query: String): String
    external fun sendPrivateMessage(fromUsername: String, toUsername: String, message: String): Boolean
    external fun getPrivateMessages(username1: String, username2: String): String

    // Новые методы для ответов на сообщения
    external fun sendMessageReply(
        username: String,
        message: String,
        replyToId: String,
        replyToUsername: String,
        replyToMessage: String
    ): Boolean

    external fun sendPrivateMessageReply(
        fromUsername: String,
        toUsername: String,
        message: String,
        replyToId: String,
        replyToUsername: String,
        replyToMessage: String
    ): Boolean

    // ИСПРАВЛЕНО: Используем прямую нативную функцию
    fun getServerUrl(): String {
        ensureInitialized()
        return "http://${getServerHost()}:${getServerPort()}"
    }

    // ДОБАВЛЕНО: Объявляем как external функции в companion object
    private external fun getServerHost(): String
    private external fun getServerPort(): Int

    fun updateServerConfigSafe(serverUrl: String): Boolean {
        return try {
            ensureInitialized()
            updateServerConfig(serverUrl)
            true
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }

    fun sendMessageSafe(username: String, message: String): Boolean {
        return try {
            ensureInitialized()
            sendMessage(username, message)
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }

    fun sendMessageReplySafe(
        username: String,
        message: String,
        replyToId: String,
        replyToUsername: String,
        replyToMessage: String
    ): Boolean {
        return try {
            ensureInitialized()
            sendMessageReply(username, message, replyToId, replyToUsername, replyToMessage)
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }

    fun getMessagesSafe(): String {
        return try {
            ensureInitialized()
            getMessages()
        } catch (e: Exception) {
            e.printStackTrace()
            "[]"
        }
    }

    fun searchUsersSafe(query: String): String {
        return try {
            ensureInitialized()
            searchUsers(query)
        } catch (e: Exception) {
            e.printStackTrace()
            "[]"
        }
    }

    fun sendPrivateMessageSafe(fromUsername: String, toUsername: String, message: String): Boolean {
        return try {
            ensureInitialized()
            sendPrivateMessage(fromUsername, toUsername, message)
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }

    fun sendPrivateMessageReplySafe(
        fromUsername: String,
        toUsername: String,
        message: String,
        replyToId: String,
        replyToUsername: String,
        replyToMessage: String
    ): Boolean {
        return try {
            ensureInitialized()
            sendPrivateMessageReply(fromUsername, toUsername, message, replyToId, replyToUsername, replyToMessage)
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }

    fun getPrivateMessagesSafe(username1: String, username2: String): String {
        return try {
            ensureInitialized()
            getPrivateMessages(username1, username2)
        } catch (e: Exception) {
            e.printStackTrace()
            "[]"
        }
    }

    suspend fun isUserVerifiedSafe(username: String): Boolean {
        return try {
            ensureInitialized()
            val url = URL("${getServerUrl()}/users/${username}/verified")
            val connection = url.openConnection() as HttpURLConnection
            connection.connectTimeout = 5000
            connection.readTimeout = 5000
            connection.requestMethod = "GET"

            if (connection.responseCode == 200) {
                val response = connection.inputStream.bufferedReader().use { it.readText() }
                val moshi = Moshi.Builder().add(KotlinJsonAdapterFactory()).build()
                val adapter = moshi.adapter<Map<String, Any>>(Map::class.java)
                val json = adapter.fromJson(response)
                json?.get("is_verified") as? Boolean ?: false
            } else {
                false
            }
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }
}