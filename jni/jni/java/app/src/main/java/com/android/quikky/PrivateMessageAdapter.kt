package com.android.quikky

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView

class PrivateMessageAdapter(
    private val messages: List<PrivateMessage>,
    private val currentUsername: String,
    private val onMessageLongClick: (PrivateMessage) -> Unit,
    private val onReplyClick: (PrivateMessage) -> Unit
) : RecyclerView.Adapter<PrivateMessageAdapter.MessageViewHolder>() {

    class MessageViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        val tvMessage: TextView = itemView.findViewById(R.id.tv_message)
        val layoutReply: View = itemView.findViewById(R.id.layout_reply)
        val tvReplyUsername: TextView = itemView.findViewById(R.id.tv_reply_username)
        val tvReplyMessage: TextView = itemView.findViewById(R.id.tv_reply_message)
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MessageViewHolder {
        val layoutId = if (viewType == 0) R.layout.item_private_message_own else R.layout.item_private_message_other
        val view = LayoutInflater.from(parent.context).inflate(layoutId, parent, false)
        return MessageViewHolder(view)
    }

    override fun onBindViewHolder(holder: MessageViewHolder, position: Int) {
        val message = messages[position]

        val displayText = if (message.fromUsername == currentUsername) {
            "Вы: ${message.message}"
        } else {
            "${message.fromUsername}: ${message.message}"
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
            onReplyClick(message)
        }
    }

    override fun getItemViewType(position: Int): Int {
        return if (messages[position].fromUsername == currentUsername) 0 else 1
    }

    override fun getItemCount(): Int = messages.size
}