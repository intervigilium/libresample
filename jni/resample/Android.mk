LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := resample
LOCAL_SRC_FILES := resample.c resample-interface.c
LOCAL_C_INCLUDES := resample.h resample-interface.h
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
