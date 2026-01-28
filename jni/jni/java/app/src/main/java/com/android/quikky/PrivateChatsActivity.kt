package com.android.quikky

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.text.Editable
import android.text.TextWatcher
import android.widget.EditText
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.squareup.moshi.Moshi
import com.squareup.moshi.Types
import com.squareup.moshi.kotlin.reflect.KotlinJsonAdapterFactory
import java.net.HttpURLConnection
import java.net.URL

class PrivateChatsActivity : AppCompatActivity() {

    private lateinit var rvUsers: RecyclerView
    private lateinit var etSearch: EditText
    private lateinit var tvEmpty: TextView

    private val userList = mutableListOf<UserSearchResponse>()
    private lateinit var adapter: UserAdapter
    private val handler = Handler(Looper.getMainLooper())
    private val prefs by lazy {
        getSharedPreferences("messenger_prefs", Context.MODE_PRIVATE)
    }

    private val moshi: Moshi by lazy {
        Moshi.Builder()
            .add(KotlinJsonAdapterFactory())
            .build()
    }

    private val searchRunnable = Runnable {
        performSearch(etSearch.text.toString())
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_private_chats)

        initViews()
        setupRecyclerView()
        setupSearch()
        loadUsers("")
    }

    private fun initViews() {
        rvUsers = findViewById(R.id.rv_users)
        etSearch = findViewById(R.id.et_search)
        tvEmpty = findViewById(R.id.tv_empty)

        etSearch.filters = arrayOf(android.text.InputFilter { source, _, _, _, _, _ ->
            source.filter { it != '\n' }
        })
    }

    private fun setupRecyclerView() {
        adapter = UserAdapter(userList) { user ->
            openPrivateChat(user.username)
        }
        rvUsers.adapter = adapter
        rvUsers.layoutManager = LinearLayoutManager(this)
    }

    private fun setupSearch() {
        etSearch.addTextChangedListener(object : TextWatcher {
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {}
            override fun afterTextChanged(s: Editable?) {
                handler.removeCallbacks(searchRunnable)
                handler.postDelayed(searchRunnable, 300)
            }
        })
    }

    private fun loadUsers(query: String) {
        Thread {
            try {
                val cleanQuery = query.trim()
                // ИСПРАВЛЕНИЕ: Используем безопасный метод
                val baseUrl = ChatHelper.getServerUrl()
                val urlString = if (cleanQuery.isNotEmpty()) {
                    "$baseUrl/users/search?query=${java.net.URLEncoder.encode(cleanQuery, "UTF-8")}"
                } else {
                    "$baseUrl/users/search"
                }

                val url = URL(urlString)
                val connection = url.openConnection() as HttpURLConnection
                connection.apply {
                    connectTimeout = 10000
                    readTimeout = 10000
                    requestMethod = "GET"
                    setRequestProperty("Content-Type", "application/json")
                    setRequestProperty("Accept", "application/json")
                }

                val responseCode = connection.responseCode
                var jsonString: String? = null

                if (responseCode == HttpURLConnection.HTTP_OK) {
                    val inputStream = connection.inputStream
                    jsonString = inputStream.bufferedReader().use { it.readText() }
                    inputStream.close()
                }

                connection.disconnect()

                handler.post {
                    if (jsonString != null) {
                        parseAndDisplayUsers(jsonString) // Передаем строку JSON
                    } else {
                        showErrorState()
                    }
                }

            } catch (e: Exception) {
                handler.post {
                    showErrorState()
                }
            }
        }.start()
    }

    private fun performSearch(query: String) {
        loadUsers(query)
    }

    private fun parseAndDisplayUsers(jsonString: String) {
        try {
            val listType = Types.newParameterizedType(List::class.java, UserSearchResponse::class.java)
            val jsonAdapter = moshi.adapter<List<UserSearchResponse>>(listType)
            val searchResults = jsonAdapter.fromJson(jsonString) ?: emptyList()

            val currentUser = prefs.getString("username", "") ?: ""
            val filteredUsers = searchResults.filter { it.username != currentUser }

            userList.clear()
            userList.addAll(filteredUsers)
            adapter.notifyDataSetChanged()

            tvEmpty.visibility = if (userList.isEmpty()) TextView.VISIBLE else TextView.GONE

        } catch (e: Exception) {
            showErrorState()
        }
    }

    private fun showErrorState() {
        userList.clear()
        adapter.notifyDataSetChanged()
        tvEmpty.visibility = TextView.VISIBLE
        tvEmpty.text = "Ошибка загрузки пользователей"
    }

    private fun openPrivateChat(targetUsername: String) {
        val intent = Intent(this, PrivateChatActivity::class.java).apply {
            putExtra("target_username", targetUsername)
        }
        startActivity(intent)
    }
}