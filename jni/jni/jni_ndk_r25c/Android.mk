LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := client
LOCAL_SRC_FILES := client.cpp
LOCAL_LDLIBS    := -llog

include $(BUILD_SHARED_LIBRARY)
