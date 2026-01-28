package com.android.quikky

object NativeAuth {
    init {
        try {
            System.loadLibrary("messenger")
        } catch (e: UnsatisfiedLinkError) {
            e.printStackTrace()
        }
    }

    external fun login(username: String, password: String): String
    external fun register(username: String, password: String): String

    fun loginSafe(username: String, password: String): String {
        return try {
            login(username, password)
        } catch (e: Exception) {
            e.printStackTrace()
            """{"status":"error","message":"Connection failed"}"""
        }
    }

    fun registerSafe(username: String, password: String): String {
        return try {
            register(username, password)
        } catch (e: Exception) {
            e.printStackTrace()
            """{"status":"error","message":"Connection failed"}"""
        }
    }
}