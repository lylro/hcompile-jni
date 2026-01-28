package com.android.quikky

import android.view.LayoutInflater
import android.view.ViewGroup
import androidx.core.content.ContextCompat
import androidx.recyclerview.widget.RecyclerView

class UserAdapter(
    private val users: List<UserSearchResponse>, // ИСПРАВЛЕНО: принимаем список пользователей
    private val onUserClick: (UserSearchResponse) -> Unit
) : RecyclerView.Adapter<UserAdapter.UserViewHolder>() {

    class UserViewHolder(itemView: android.view.View) : RecyclerView.ViewHolder(itemView) {
        val tvUsername: android.widget.TextView = itemView.findViewById(R.id.tv_username)
        val tvStatus: android.widget.TextView = itemView.findViewById(R.id.tv_status)
        val ivVerified: android.widget.ImageView = itemView.findViewById(R.id.iv_verified)
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): UserViewHolder =
        UserViewHolder(LayoutInflater.from(parent.context).inflate(R.layout.item_user, parent, false))

    override fun onBindViewHolder(holder: UserViewHolder, position: Int) {
        val user = users[position]
        holder.tvUsername.text = user.username
        holder.tvStatus.text = if (user.isOnline) "online" else "offline"
        holder.tvStatus.setTextColor(ContextCompat.getColor(holder.itemView.context,
            if (user.isOnline) android.R.color.holo_green_dark else android.R.color.darker_gray
        ))

        // Отображение значка верификации
        holder.ivVerified.visibility = if (user.isVerified) android.view.View.VISIBLE else android.view.View.GONE

        holder.itemView.setOnClickListener { onUserClick(user) }
    }

    override fun getItemCount(): Int = users.size
}