package com.android.quikky

import com.squareup.moshi.Json
import com.squareup.moshi.JsonClass

@JsonClass(generateAdapter = true)
data class User(
    @Json(name = "username") val username: String,
    @Json(name = "is_online") val isOnline: Boolean = false
)