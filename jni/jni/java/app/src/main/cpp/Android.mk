LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := messenger
LOCAL_SRC_FILES := native-lib.cpp chat_client.cpp server_config.cpp

LOCAL_LDLIBS := -llog

LOCAL_CPPFLAGS := -std=c++17 -Wall -O2
LOCAL_CPP_FEATURES := exceptions rtti
LOCAL_C_INCLUDES := $(LOCAL_PATH)

include $(BUILD_SHARED_LIBRARY)