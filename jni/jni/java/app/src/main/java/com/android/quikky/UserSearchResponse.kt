package com.android.quikky

import com.squareup.moshi.Json
import com.squareup.moshi.JsonClass

@JsonClass(generateAdapter = true)
data class UserSearchResponse(
    @Json(name = "username") val username: String,
    @Json(name = "is_online") val isOnline: Boolean = false,
    @Json(name = "is_verified") val isVerified: Boolean = false
)