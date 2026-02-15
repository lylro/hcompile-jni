LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := wnrpc
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2

# path
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/arm/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/SAMP/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_CPPFLAGS := -w -s -fvisibility=hidden -pthread -Wall -fpack-struct=1 -O2 -std=c++14 -fexceptions -D_GNU_SOURCE
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2

# Добавляем флаги для inline hook
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS += -D_GNU_SOURCE

include $(BUILD_SHARED_LIBRARY)