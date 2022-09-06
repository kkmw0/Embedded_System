LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS := -mfloat-abi=softfp -mfpu=neon
LOCAL_MODULE := recognition

LOCAL_SRC_FILES := hap.c

LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)