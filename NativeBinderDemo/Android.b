LOCAL_PATH := $(call my-dir)
 
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libbinder       
LOCAL_LDLIBS += -landroid -llog
LOCAL_MODULE    := libPolySpatialServerS
LOCAL_CFLAGS += -Wno-error -Wnon-virtual-dtor -Wunused-parameter
LOCAL_SRC_FILES := \
    sharedmemory/sharedmemory_android.cpp \
    socket/sharedmemory_android_server.cpp
include $(BUILD_EXECUTABLE)
# include $(BUILD_SHARED_LIBRARY)
  

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libbinder
LOCAL_LDLIBS += -landroid -llog 
LOCAL_MODULE    := libPolySpatialClientS
LOCAL_CFLAGS += -Wno-error -Wnon-virtual-dtor -Wunused-parameter
LOCAL_SRC_FILES := \
    sharedmemory/sharedmemory_android.cpp \
    socket/sharedmemory_android_client.cpp
# LOCAL_MULTILIB := 64
include $(BUILD_EXECUTABLE)
# include $(BUILD_SHARED_LIBRARY)
