package com.android.quikky

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.LayoutInflater
import android.view.View
import android.widget.*
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.squareup.moshi.Moshi
import com.squareup.moshi.Types
import com.squareup.moshi.kotlin.reflect.KotlinJsonAdapterFactory
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

class PrivateChatActivity : AppCompatActivity() {

    private lateinit var adapter: PrivateMessageAdapter
    private val messageList = mutableListOf<PrivateMessage>()
    private val prefs by lazy { getSharedPreferences("messenger_prefs", Context.MODE_PRIVATE) }
    private val deletedPrefs by lazy { getSharedPreferences("deleted_private_messages", Context.MODE_PRIVATE) }

    private lateinit var tvTitle: TextView
    private lateinit var rvMessages: RecyclerView
    private lateinit var btnSend: Button
    private lateinit var etMessage: EditText
    private lateinit var ivVerified: ImageView

    // Переменные для ответа на сообщение
    private var replyingToMessage: PrivateMessage? = null
    private lateinit var layoutReplyPreview: LinearLayout
    private lateinit var tvReplyPreviewUsername: TextView
    private lateinit var tvReplyPreviewMessage: TextView
    private lateinit var ivCancelReply: ImageView

    private var targetUsername: String = ""
    private val handler = Handler(Looper.getMainLooper())
    private var isRefreshing = false

    private val refreshRunnable = object : Runnable {
        override fun run() {
            if (!isRefreshing) loadMessages()
            handler.postDelayed(this, 2000)
        }
    }

    private val moshi: Moshi by lazy {
        Moshi.Builder().add(KotlinJsonAdapterFactory()).build()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_private_chat)

        targetUsername = intent.getStringExtra("target_username") ?: ""

        initViews()
        setupUI()
        setupReplyPreview()
        loadVerifiedStatus()
        loadMessages()
        handler.post(refreshRunnable)
    }

    override fun onDestroy() {
        super.onDestroy()
        handler.removeCallbacks(refreshRunnable)
    }

    private fun initViews() {
        tvTitle = findViewById(R.id.tv_title)
        rvMessages = findViewById(R.id.rv_messages)
        btnSend = findViewById(R.id.btn_send)
        etMessage = findViewById(R.id.et_message)
        ivVerified = findViewById(R.id.iv_verified)
        findViewById<Button>(R.id.btn_back).setOnClickListener { finish() }
    }

    private fun setupUI() {
        tvTitle.text = "Чат с $targetUsername"

        adapter = PrivateMessageAdapter(
            messageList,
            getCurrentUsername(),
            { message -> showDeleteDialog(message) },
            { message -> startReply(message) }
        )
        rvMessages.adapter = adapter
        rvMessages.layoutManager = LinearLayoutManager(this)

        btnSend.setOnClickListener { sendMessage() }
    }

    private fun setupReplyPreview() {
        layoutReplyPreview = findViewById(R.id.layout_reply_preview)
        tvReplyPreviewUsername = findViewById(R.id.tv_reply_preview_username)
        tvReplyPreviewMessage = findViewById(R.id.tv_reply_preview_message)
        ivCancelReply = findViewById(R.id.iv_cancel_reply)

        ivCancelReply.setOnClickListener {
            cancelReply()
        }
    }

    private fun startReply(message: PrivateMessage) {
        replyingToMessage = message

        // Показываем превью ответа
        layoutReplyPreview.visibility = View.VISIBLE
        val username = if (message.fromUsername == getCurrentUsername()) "Вы" else message.fromUsername
        tvReplyPreviewUsername.text = username
        tvReplyPreviewMessage.text = message.message

        // Фокусируемся на поле ввода
        etMessage.requestFocus()

        // Прокручиваем к нижней части списка
        if (messageList.isNotEmpty()) {
            rvMessages.scrollToPosition(messageList.size - 1)
        }
    }

    private fun cancelReply() {
        replyingToMessage = null
        layoutReplyPreview.visibility = View.GONE
    }

    private fun loadVerifiedStatus() {
        if (targetUsername.isNotEmpty()) {
            CoroutineScope(Dispatchers.IO).launch {
                try {
                    val searchResults = ChatHelper.searchUsersSafe(targetUsername)
                    val listType = Types.newParameterizedType(List::class.java, UserSearchResponse::class.java)
                    val jsonAdapter = moshi.adapter<List<UserSearchResponse>>(listType)
                    val users = jsonAdapter.fromJson(searchResults) ?: emptyList()

                    val isVerified = users.find { it.username == targetUsername }?.isVerified ?: false

                    handler.post {
                        ivVerified.visibility = if (isVerified) View.VISIBLE else View.GONE
                        updateTitleWithVerification(isVerified)
                    }
                } catch (e: Exception) {
                    e.printStackTrace()
                    handler.post {
                        ivVerified.visibility = View.GONE
                        updateTitleWithVerification(false)
                    }
                }
            }
        }
    }

    private fun updateTitleWithVerification(isVerified: Boolean) {
        val title = if (isVerified) {
            "Чат с $targetUsername ✓"
        } else {
            "Чат с $targetUsername"
        }
        tvTitle.text = title
    }

    private fun getCurrentUsername(): String = prefs.getString("username", "") ?: ""

    private fun sendMessage() {
        val messageText = etMessage.text.toString().trim()
        if (messageText.isNotEmpty()) {
            Thread {
                val success = if (replyingToMessage != null) {
                    // Отправляем сообщение с ответом
                    ChatHelper.sendPrivateMessageReplySafe(
                        getCurrentUsername(),
                        targetUsername,
                        messageText,
                        replyingToMessage!!.id,
                        replyingToMessage!!.fromUsername,
                        replyingToMessage!!.message
                    )
                } else {
                    // Обычное сообщение
                    ChatHelper.sendPrivateMessageSafe(getCurrentUsername(), targetUsername, messageText)
                }

                handler.post {
                    if (success) {
                        etMessage.text.clear()
                        cancelReply()
                        loadMessages()
                    } else Toast.makeText(this, "Ошибка отправки", Toast.LENGTH_SHORT).show()
                }
            }.start()
        }
    }

    private fun loadMessages() {
        if (isRefreshing) return
        isRefreshing = true
        Thread {
            try {
                val jsonString = ChatHelper.getPrivateMessagesSafe(getCurrentUsername(), targetUsername)
                handler.post {
                    parseMessages(jsonString)
                    isRefreshing = false
                }
            } catch (e: Exception) {
                e.printStackTrace()
                handler.post { isRefreshing = false }
            }
        }.start()
    }

    private fun parseMessages(jsonString: String) {
        try {
            val listType = Types.newParameterizedType(List::class.java, PrivateMessage::class.java)
            val jsonAdapter = moshi.adapter<List<PrivateMessage>>(listType)
            val newMessages = jsonAdapter.fromJson(jsonString) ?: emptyList()

            val filteredMessages = newMessages
                .sortedBy { it.timestamp }
                .filter { !isPrivateMessageDeleted(it.id) }

            if (filteredMessages != messageList) {
                messageList.clear()
                messageList.addAll(filteredMessages)
                adapter.notifyDataSetChanged()
                if (messageList.isNotEmpty()) rvMessages.scrollToPosition(messageList.size - 1)
            }
        } catch (e: Exception) { e.printStackTrace() }
    }

    private fun isPrivateMessageDeleted(messageId: String): Boolean {
        return deletedPrefs.getBoolean(messageId, false)
    }

    private fun markPrivateMessageAsDeleted(messageId: String) {
        deletedPrefs.edit().putBoolean(messageId, true).apply()
    }

    private fun showDeleteDialog(message: PrivateMessage) {
        val dialogView = LayoutInflater.from(this).inflate(R.layout.dialog_delete_message, null)
        dialogView.findViewById<TextView>(R.id.tv_message_preview).text = message.message

        val dialog = AlertDialog.Builder(this)
            .setView(dialogView)
            .setNeutralButton("Ответить") { _, _ ->
                startReply(message)
            }
            .create()

        dialogView.findViewById<Button>(R.id.btn_delete_local).setOnClickListener {
            markPrivateMessageAsDeleted(message.id)
            loadMessages()
            dialog.dismiss()
            Toast.makeText(this, "Сообщение удалено", Toast.LENGTH_SHORT).show()
        }

        dialogView.findViewById<Button>(R.id.btn_cancel).setOnClickListener { dialog.dismiss() }

        dialog.show()
    }
}