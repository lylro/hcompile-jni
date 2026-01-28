package com.android.quikky

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.LayoutInflater
import android.widget.*
import android.view.*
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.squareup.moshi.Moshi
import com.squareup.moshi.Types
import com.squareup.moshi.kotlin.reflect.KotlinJsonAdapterFactory

class ChatActivity : AppCompatActivity() {
    private lateinit var adapter: MessageAdapter
    private val messageList = mutableListOf<Message>()
    private val prefs by lazy { getSharedPreferences("messenger_prefs", Context.MODE_PRIVATE) }
    private val deletedMessagesPrefs by lazy { getSharedPreferences("deleted_messages", Context.MODE_PRIVATE) }
    private val handler = Handler(Looper.getMainLooper())
    private var isRefreshing = false

    // Переменные для ответа на сообщение
    private var replyingToMessage: Message? = null
    private lateinit var layoutReplyPreview: LinearLayout
    private lateinit var tvReplyPreviewUsername: TextView
    private lateinit var tvReplyPreviewMessage: TextView
    private lateinit var ivCancelReply: ImageView

    private val moshi: Moshi by lazy {
        Moshi.Builder().add(KotlinJsonAdapterFactory()).build()
    }

    private val refreshRunnable = object : Runnable {
        override fun run() {
            if (!isRefreshing) loadMessages()
            handler.postDelayed(this, 2000)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_chat)
        window.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_PAN)

        setupUI()
        setupReplyPreview()
        loadMessages()
        handler.post(refreshRunnable)
    }

    override fun onDestroy() {
        super.onDestroy()
        handler.removeCallbacks(refreshRunnable)
    }

    private fun setupUI() {
        adapter = MessageAdapter(
            messageList,
            getCurrentUsername(),
            { showDeleteDialog(it) },
            { startReply(it) }
        )

        findViewById<RecyclerView>(R.id.rv_messages).apply {
            adapter = this@ChatActivity.adapter
            layoutManager = LinearLayoutManager(this@ChatActivity)
        }

        findViewById<TextView>(R.id.tv_title).text = "Общий чат"

        findViewById<Button>(R.id.btn_send).setOnClickListener { sendMessage() }
        findViewById<Button>(R.id.btn_exit).setOnClickListener {
            startActivity(Intent(this, MainActivity::class.java).apply {
                flags = Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_SINGLE_TOP
            })
            finish()
        }
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

    private fun startReply(message: Message) {
        replyingToMessage = message

        // Показываем превью ответа
        layoutReplyPreview.visibility = View.VISIBLE
        val username = if (message.username == getCurrentUsername()) "Вы" else message.username
        tvReplyPreviewUsername.text = username
        tvReplyPreviewMessage.text = message.message

        // Фокусируемся на поле ввода
        findViewById<EditText>(R.id.et_message).requestFocus()

        // Прокручиваем к нижней части списка
        if (messageList.isNotEmpty()) {
            findViewById<RecyclerView>(R.id.rv_messages).scrollToPosition(messageList.size - 1)
        }
    }

    private fun cancelReply() {
        replyingToMessage = null
        layoutReplyPreview.visibility = View.GONE
    }

    private fun getCurrentUsername() = prefs.getString("username", "") ?: ""

    private fun sendMessage() {
        val messageText = findViewById<EditText>(R.id.et_message).text.toString().trim()
        if (messageText.isNotEmpty()) {
            Thread {
                val success = if (replyingToMessage != null) {
                    // Отправляем сообщение с ответом
                    ChatHelper.sendMessageReplySafe(
                        getCurrentUsername(),
                        messageText,
                        replyingToMessage!!.id,
                        replyingToMessage!!.username,
                        replyingToMessage!!.message
                    )
                } else {
                    // Обычное сообщение
                    ChatHelper.sendMessageSafe(getCurrentUsername(), messageText)
                }

                handler.post {
                    if (success) {
                        findViewById<EditText>(R.id.et_message).text.clear()
                        cancelReply()
                        loadMessages()
                    } else {
                        Toast.makeText(this, "Сервер отключен. Обратитесь\nв Telegram: @bycage", Toast.LENGTH_SHORT).show()
                    }
                }
            }.start()
        }
    }

    private fun loadMessages() {
        if (isRefreshing) return
        isRefreshing = true
        Thread {
            try {
                val jsonString = ChatHelper.getMessagesSafe()
                handler.post { parseMessages(jsonString) }
            } catch (e: Exception) {
                handler.post {
                    isRefreshing = false
                    Toast.makeText(this, "Ошибка загрузки сообщений", Toast.LENGTH_SHORT).show()
                }
            }
        }.start()
    }

    private fun parseMessages(jsonString: String) {
        try {
            isRefreshing = false
            if (jsonString.isEmpty() || jsonString == "[]") {
                messageList.clear()
                adapter.notifyDataSetChanged()
                return
            }

            val listType = Types.newParameterizedType(List::class.java, Message::class.java)
            val newMessages = moshi.adapter<List<Message>>(listType).fromJson(jsonString) ?: emptyList()
            val filteredMessages = newMessages.sortedBy { it.timestamp }.filter { !isMessageDeleted(it.id) }

            if (filteredMessages != messageList) {
                messageList.clear()
                messageList.addAll(filteredMessages)
                adapter.notifyDataSetChanged()
                if (messageList.isNotEmpty()) {
                    findViewById<RecyclerView>(R.id.rv_messages).scrollToPosition(messageList.size - 1)
                }
            }
        } catch (e: Exception) {
            Toast.makeText(this, "Ошибка формата данных", Toast.LENGTH_SHORT).show()
        }
    }

    private fun isMessageDeleted(messageId: String) = deletedMessagesPrefs.getBoolean(messageId, false)

    private fun markMessageAsDeleted(messageId: String) {
        deletedMessagesPrefs.edit().putBoolean(messageId, true).apply()
    }

    private fun showDeleteDialog(message: Message) {
        val dialogView = LayoutInflater.from(this).inflate(R.layout.dialog_delete_message, null)
        dialogView.findViewById<TextView>(R.id.tv_message_preview).text = message.message

        // Объявляем dialog до создания
        val dialog = AlertDialog.Builder(this)
            .setView(dialogView)
            .setNeutralButton("Ответить") { _, _ ->
                startReply(message)
            }
            .create()

        // Удаляем кнопку, которая была создана вручную (лишняя)
        // val btnReply = Button(this).apply {
        //     text = "Ответить"
        //     setOnClickListener {
        //         startReply(message)
        //         dialog.dismiss()
        //     }
        // }

        dialogView.findViewById<Button>(R.id.btn_delete_local).setOnClickListener {
            markMessageAsDeleted(message.id)
            loadMessages()
            dialog.dismiss()
            Toast.makeText(this, "Сообщение удалено", Toast.LENGTH_SHORT).show()
        }
        dialogView.findViewById<Button>(R.id.btn_cancel).setOnClickListener { dialog.dismiss() }
        dialog.show()
    }
}