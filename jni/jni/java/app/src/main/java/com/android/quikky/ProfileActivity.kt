package com.android.quikky

import android.content.Intent
import android.content.SharedPreferences
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.text.Editable
import android.text.TextWatcher
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.android.quikky.databinding.ActivityProfileBinding
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import com.squareup.moshi.*

class ProfileActivity : AppCompatActivity() {
    private lateinit var binding: ActivityProfileBinding
    private lateinit var prefs: SharedPreferences
    private val maxDescriptionLength = 500
    private val handler = Handler(Looper.getMainLooper())

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityProfileBinding.inflate(layoutInflater)
        setContentView(binding.root)

        prefs = getSharedPreferences("messenger_prefs", MODE_PRIVATE)

        setupViews()
        loadVerifiedStatus()
    }

    private fun setupViews() {
        val username = prefs.getString("username", "") ?: ""
        binding.tvUsername.text = username

        val description = prefs.getString("user_description", "Расскажите о себе...") ?: "Расскажите о себе..."
        binding.etDescription.setText(description)

        updateCharacterCount(description.length)

        binding.etDescription.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable?) {
                updateCharacterCount(s?.length ?: 0)
            }
        })

        binding.btnSaveDescription.setOnClickListener {
            saveDescription()
        }

        binding.btnBack.setOnClickListener {
            finish()
        }
    }

    private fun loadVerifiedStatus() {
        val username = prefs.getString("username", "") ?: ""
        if (username.isNotEmpty()) {
            CoroutineScope(Dispatchers.IO).launch {
                try {
                    // Используем поиск пользователей для получения статуса верификации
                    val searchResults = ChatHelper.searchUsersSafe(username)
                    val moshi = Moshi.Builder().add(KotlinJsonAdapterFactory()).build()
                    val listType = Types.newParameterizedType(List::class.java, UserSearchResponse::class.java)
                    val adapter = moshi.adapter<List<UserSearchResponse>>(listType)
                    val users = adapter.fromJson(searchResults) ?: emptyList()

                    val isVerified = users.find { it.username == username }?.isVerified ?: false

                    handler.post {
                        binding.ivVerified.visibility = if (isVerified) android.view.View.VISIBLE else android.view.View.GONE
                    }
                } catch (e: Exception) {
                    e.printStackTrace()
                    handler.post {
                        binding.ivVerified.visibility = android.view.View.GONE
                    }
                }
            }
        }
    }

    private fun updateCharacterCount(currentLength: Int) {
        // ИСПРАВЛЕНО: Простой способ обновления счетчика символов
        // Если у вас есть TextView для счетчика в layout, используйте его
        // Если нет, можно добавить или просто выводить в лог
        val counterText = "$currentLength/$maxDescriptionLength"

        // Если в вашем layout есть TextView с id tv_char_count, раскомментируйте:
        // binding.tvCharCount.text = counterText

        // Временное решение: выводим в Toast при достижении лимита
        if (currentLength >= maxDescriptionLength) {
            Toast.makeText(this, "Достигнут лимит символов: $counterText", Toast.LENGTH_SHORT).show()
        }
    }

    private fun saveDescription() {
        val description = binding.etDescription.text.toString().trim()
        if (description.length > maxDescriptionLength) {
            Toast.makeText(this, "Описание слишком длинное", Toast.LENGTH_SHORT).show()
            return
        }

        prefs.edit().putString("user_description", description).apply()
        Toast.makeText(this, "Описание сохранено", Toast.LENGTH_SHORT).show()
    }
}