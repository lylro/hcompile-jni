package com.android.quikky

import com.squareup.moshi.Json
import com.squareup.moshi.JsonClass

@JsonClass(generateAdapter = true)
data class Message(
    @Json(name = "id") val id: String,
    @Json(name = "username") val username: String,
    @Json(name = "message") val message: String,
    @Json(name = "timestamp") val timestamp: String,
    @Json(name = "reply_to_id") val replyToId: String? = null,
    @Json(name = "reply_to_username") val replyToUsername: String? = null,
    @Json(name = "reply_to_message") val replyToMessage: String? = null
)