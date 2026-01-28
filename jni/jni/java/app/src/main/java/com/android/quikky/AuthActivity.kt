package com.android.quikky

import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.os.Bundle
import android.view.inputmethod.EditorInfo
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import com.android.quikky.databinding.ActivityAuthBinding

class AuthActivity : AppCompatActivity() {
    private lateinit var binding: ActivityAuthBinding
    private lateinit var prefs: SharedPreferences

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        setupSimpleStatusBar()
        
        binding = ActivityAuthBinding.inflate(layoutInflater)
        setContentView(binding.root)
        
        prefs = getSharedPreferences("messenger_prefs", Context.MODE_PRIVATE)
        
        if (isUserLoggedIn()) {
            navigateToMain()
            return
        }
        
        setupViews()
    }
    
    private fun setupSimpleStatusBar() {
        WindowCompat.setDecorFitsSystemWindows(window, false)
    }

    private fun setupViews() {
        binding.btnLogin.setOnClickListener {
            performLogin()
        }

        binding.btnRegister.setOnClickListener {
            performRegister()
        }

        binding.etPassword.setOnEditorActionListener { _, actionId, _ ->
            if (actionId == EditorInfo.IME_ACTION_DONE) {
                performLogin()
                true
            } else {
                false
            }
        }
    }

    private fun performLogin() {
        val username = binding.etUsername.text.toString().trim()
        val password = binding.etPassword.text.toString().trim()

        if (validateInput(username, password)) {
            loginUser(username, password)
        }
    }

    private fun performRegister() {
        val username = binding.etUsername.text.toString().trim()
        val password = binding.etPassword.text.toString().trim()

        if (validateInput(username, password)) {
            registerUser(username, password)
        }
    }

    private fun validateInput(username: String, password: String): Boolean {
        return when {
            username.isEmpty() -> {
                Toast.makeText(this, "Введите имя пользователя", Toast.LENGTH_SHORT).show()
                false
            }
            password.isEmpty() -> {
                Toast.makeText(this, "Введите пароль", Toast.LENGTH_SHORT).show()
                false
            }
            username.length < 3 -> {
                Toast.makeText(this, "Имя пользователя должно быть не менее 3 символов", Toast.LENGTH_SHORT).show()
                false
            }
            password.length < 4 -> {
                Toast.makeText(this, "Пароль должен быть не менее 4 символов", Toast.LENGTH_SHORT).show()
                false
            }
            else -> true
        }
    }

    private fun loginUser(username: String, password: String) {
        Thread {
            try {
                val success = AuthHelper.login(username, password)
                handler.post {
                    if (success) {
                        saveUserSession(username)
                        navigateToMain()
                        Toast.makeText(this, "Вход выполнен успешно", Toast.LENGTH_SHORT).show()
                    } else {
                        Toast.makeText(this, "Неверное имя пользователя или пароль", Toast.LENGTH_SHORT).show()
                    }
                }
            } catch (e: Exception) {
                e.printStackTrace()
                handler.post {
                    Toast.makeText(this, "Ошибка подключения к серверу", Toast.LENGTH_SHORT).show()
                }
            }
        }.start()
    }

    private fun registerUser(username: String, password: String) {
        Thread {
            try {
                val success = AuthHelper.register(username, password)
                handler.post {
                    if (success) {
                        saveUserSession(username)
                        navigateToMain()
                        Toast.makeText(this, "Регистрация выполнена успешно", Toast.LENGTH_SHORT).show()
                    } else {
                        Toast.makeText(this, "Имя пользователя уже занято", Toast.LENGTH_SHORT).show()
                    }
                }
            } catch (e: Exception) {
                e.printStackTrace()
                handler.post {
                    Toast.makeText(this, "Ошибка подключения к серверу", Toast.LENGTH_SHORT).show()
                }
            }
        }.start()
    }

    private fun saveUserSession(username: String) {
        prefs.edit().apply {
            putString("username", username)
            putBoolean("is_logged_in", true)
            apply()
        }
    }

    private fun isUserLoggedIn(): Boolean {
        return prefs.getBoolean("is_logged_in", false) && 
               prefs.getString("username", "")?.isNotEmpty() == true
    }

    private fun navigateToMain() {
        startActivity(Intent(this, MainActivity::class.java))
        finish()
    }

    private val handler = android.os.Handler(android.os.Looper.getMainLooper())
}