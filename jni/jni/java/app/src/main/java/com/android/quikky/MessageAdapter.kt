package com.android.quikky

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView

class MessageAdapter(
    private val messages: List<Message>,
    private val currentUsername: String,
    private val onMessageLongClick: (Message) -> Unit,
    private val onReplyClick: (Message) -> Unit
) : RecyclerView.Adapter<MessageAdapter.MessageViewHolder>() {

    class MessageViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        val tvMessage: TextView = itemView.findViewById(R.id.tv_message)
        val layoutReply: View = itemView.findViewById(R.id.layout_reply)
        val tvReplyUsername: TextView = itemView.findViewById(R.id.tv_reply_username)
        val tvReplyMessage: TextView = itemView.findViewById(R.id.tv_reply_message)
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MessageViewHolder {
        val layoutId = if (viewType == 0) R.layout.item_message_own else R.layout.item_message_other
        val view = LayoutInflater.from(parent.context).inflate(layoutId, parent, false)
        return MessageViewHolder(view)
    }

    override fun onBindViewHolder(holder: MessageViewHolder, position: Int) {
        val message = messages[position]
        val displayText = if (message.username == currentUsername) {
            "Вы: ${message.message}"
        } else {
            "${message.username}: ${message.message}"
        }

        holder.tvMessage.text = displayText

        // Настройка блока цитаты (ответа)
        if (message.replyToMessage != null && message.replyToUsername != null) {
            holder.layoutReply.visibility = View.VISIBLE
            val replyUsername = if (message.replyToUsername == currentUsername) {
                "Вы"
            } else {
                message.replyToUsername
            }
            holder.tvReplyUsername.text = replyUsername
            holder.tvReplyMessage.text = message.replyToMessage
        } else {
            holder.layoutReply.visibility = View.GONE
        }

        holder.itemView.setOnLongClickListener {
            onMessageLongClick(message)
            true
        }

        holder.itemView.setOnClickListener {
            holder.itemView.isPressed = true
            holder.itemView.postDelayed({ holder.itemView.isPressed = false }, 100)
            onReplyClick(message)
        }
    }

    override fun getItemViewType(position: Int): Int {
        return if (messages[position].username == currentUsername) 0 else 1
    }

    override fun getItemCount(): Int = messages.size
}