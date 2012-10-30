# client model
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	main.c

LOCAL_LDLIBS += -lz

LOCAL_CFLAGS += -DDebug
LOCAL_MODULE:= joypad 

LOCAL_CFLAGS += -lpthread -Wall

include $(BUILD_EXECUTABLE)
