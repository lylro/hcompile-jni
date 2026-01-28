package com.android.quikky

import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import com.android.quikky.databinding.ActivitySplashBinding
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch

class SplashActivity : AppCompatActivity() {
    private lateinit var binding: ActivitySplashBinding
    private lateinit var prefs: SharedPreferences
    private lateinit var serverChecker: ServerConnectionChecker
    private var checkJob: Job? = null
    
    companion object {
        private const val CHECK_INTERVAL = 3000L
        private const val MAX_RETRY_COUNT = 10
    }
    
    private var retryCount = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        setupSimpleStatusBar()
        
        binding = ActivitySplashBinding.inflate(layoutInflater)
        setContentView(binding.root)
        
        prefs = getSharedPreferences("messenger_prefs", Context.MODE_PRIVATE)
        serverChecker = ServerConnectionChecker(this)
        
        startServerCheck()
    }
    
    private fun setupSimpleStatusBar() {
        WindowCompat.setDecorFitsSystemWindows(window, false)
    }
    
    private fun startServerCheck() {
        binding.progressBar.isIndeterminate = true
        binding.tvStatus.text = "Проверка подключения к серверу..."
        
        checkJob = CoroutineScope(Dispatchers.IO).launch {
            val isServerAvailable = serverChecker.checkServerAvailability()
            
            Handler(Looper.getMainLooper()).post {
                if (isServerAvailable) {
                    onServerAvailable()
                } else {
                    retryCount++
                    if (retryCount >= MAX_RETRY_COUNT) {
                        binding.tvStatus.text = "Сервер недоступен. Проверьте подключение."
                    } else {
                        binding.tvStatus.text = "Сервер недоступен. Попытка ${retryCount + 1}/$MAX_RETRY_COUNT"
                        Handler(Looper.getMainLooper()).postDelayed({
                            startServerCheck()
                        }, CHECK_INTERVAL)
                    }
                }
            }
        }
    }
    
    private fun onServerAvailable() {
        binding.tvStatus.text = "Сервер доступен. Загрузка..."
        
        Handler(Looper.getMainLooper()).postDelayed({
            if (isUserLoggedIn()) {
                navigateToMain()
            } else {
                navigateToAuth()
            }
        }, 1000)
    }
    
    private fun navigateToMain() {
        startActivity(Intent(this, MainActivity::class.java))
        finish()
    }
    
    private fun navigateToAuth() {
        startActivity(Intent(this, AuthActivity::class.java))
        finish()
    }
    
    private fun isUserLoggedIn(): Boolean {
        return prefs.getBoolean("is_logged_in", false) && 
               prefs.getString("username", "")?.isNotEmpty() == true
    }
    
    override fun onDestroy() {
        super.onDestroy()
        checkJob?.cancel()
    }
}