package com.android.quikky

import android.content.Context
import android.net.ConnectivityManager
import android.net.NetworkCapabilities
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.net.HttpURLConnection
import java.net.URL

class ServerConnectionChecker(private val context: Context) {

    suspend fun checkServerAvailability(): Boolean = try {
        isNetworkAvailable() && withContext(Dispatchers.IO) {
            listOf("/health", "/messages", "/").any { endpoint ->
                try {
                    URL("${ChatHelper.getServerUrl()}$endpoint").openConnection().let {
                        it as HttpURLConnection
                        it.connectTimeout = 5000
                        it.readTimeout = 5000
                        it.requestMethod = "GET"
                        it.responseCode in 200..399
                    }
                } catch (e: Exception) {
                    false
                }
            }
        }
    } catch (e: Exception) {
        false
    }

    private fun isNetworkAvailable(): Boolean = try {
        (context.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager).run {
            activeNetwork?.let { getNetworkCapabilities(it) }?.run {
                hasTransport(NetworkCapabilities.TRANSPORT_WIFI) ||
                        hasTransport(NetworkCapabilities.TRANSPORT_CELLULAR) ||
                        hasTransport(NetworkCapabilities.TRANSPORT_ETHERNET)
            } ?: false
        }
    } catch (e: Exception) {
        false
    }

    // ИСПРАВЛЕНО: Убран повторный вызов loadLibrary
    companion object {
        // Библиотека больше не загружается здесь
    }
}