package com.android.quikky

import com.squareup.moshi.Moshi
import com.squareup.moshi.kotlin.reflect.KotlinJsonAdapterFactory

object AuthHelper {
    private val moshi: Moshi = Moshi.Builder()
        .add(KotlinJsonAdapterFactory())
        .build()

    data class AuthRequest(
        val username: String,
        val password: String
    )

    data class AuthResponse(
        val status: String,
        val message: String,
        val username: String? = null,
        val user_id: String? = null
    )

    fun login(username: String, password: String): Boolean {
        return try {
            val response = NativeAuth.login(username, password)
            val responseAdapter = moshi.adapter(AuthResponse::class.java)
            val authResponse = responseAdapter.fromJson(response)
            
            authResponse?.status == "success"
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }

    fun register(username: String, password: String): Boolean {
        return try {
            val response = NativeAuth.register(username, password)
            val responseAdapter = moshi.adapter(AuthResponse::class.java)
            val authResponse = responseAdapter.fromJson(response)
            
            authResponse?.status == "success"
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }
}