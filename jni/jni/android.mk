LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := plugin

LOCAL_SRC_FILES := \
    entry.cpp \
    plugin.cpp

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)

LOCAL_CPPFLAGS += -std=c++17
LOCAL_LDLIBS += -llog -landroid

include $(BUILD_SHARED_LIBRARY)