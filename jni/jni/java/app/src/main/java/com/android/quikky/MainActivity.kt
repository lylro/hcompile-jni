package com.android.quikky

import android.content.Intent
import android.content.SharedPreferences
import android.os.Bundle
import android.util.Log
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import com.android.quikky.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding
    private lateinit var prefs: SharedPreferences

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        try {
            WindowCompat.setDecorFitsSystemWindows(window, false)

            binding = ActivityMainBinding.inflate(layoutInflater)
            setContentView(binding.root)

            prefs = getSharedPreferences("messenger_prefs", MODE_PRIVATE)

            // Предварительная инициализация ChatHelper
            try {
                ChatHelper.updateServerConfigSafe(ChatHelper.getServerUrl())
            } catch (e: Exception) {
                Log.e("MainActivity", "Failed to initialize ChatHelper", e)
            }

            if (!isUserLoggedIn()) {
                startActivity(Intent(this, AuthActivity::class.java))
                finish()
                return
            }

            setupViews()
        } catch (e: Exception) {
            Log.e("MainActivity", "Critical error in onCreate", e)
            Toast.makeText(this, "Ошибка запуска приложения", Toast.LENGTH_LONG).show()
            finish()
        }
    }

    private fun setupViews() {
        binding.tvWelcome.text = "Добро пожаловать, ${prefs.getString("username", "")}!"

        binding.btnEnterChat.setOnClickListener {
            startActivity(Intent(this, ChatActivity::class.java))
        }
        binding.btnPrivateChats.setOnClickListener {
            startActivity(Intent(this, PrivateChatsActivity::class.java))
        }
        // Добавляем кнопку перехода в профиль
        binding.btnProfile.setOnClickListener {
            startActivity(Intent(this, ProfileActivity::class.java))
        }
        binding.btnLogout.setOnClickListener {
            prefs.edit().remove("username").remove("is_logged_in").apply()
            Toast.makeText(this, "Вы вышли из аккаунта", Toast.LENGTH_SHORT).show()
            startActivity(Intent(this, AuthActivity::class.java))
            finish()
        }
    }

    private fun isUserLoggedIn(): Boolean {
        return prefs.getBoolean("is_logged_in", false) &&
                prefs.getString("username", "")?.isNotEmpty() == true
    }
}