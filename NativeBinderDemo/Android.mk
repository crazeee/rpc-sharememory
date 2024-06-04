LOCAL_PATH := $(call my-dir)
 
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libbinder       
LOCAL_LDLIBS += -landroid -llog
LOCAL_MODULE    := libPolySpatialServer
LOCAL_CFLAGS += -Wno-error -Wnon-virtual-dtor -Wunused-parameter
LOCAL_SRC_FILES := \
    sharedmemory/sharedmemory_android.cpp \
    binder/IPolySpatialService.cpp \
    binder/PolySpatialServer.cpp
include $(BUILD_EXECUTABLE)
# include $(BUILD_SHARED_LIBRARY)
  

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libbinder
LOCAL_LDLIBS += -landroid -llog 
LOCAL_MODULE    := libPolySpatialClient
LOCAL_CFLAGS += -Wno-error -Wnon-virtual-dtor -Wunused-parameter
LOCAL_SRC_FILES := \
    sharedmemory/sharedmemory_android.cpp \
    binder/IPolySpatialService.cpp \
    binder/PolySpatialClient.cpp
# LOCAL_MULTILIB := 64
include $(BUILD_EXECUTABLE)
# include $(BUILD_SHARED_LIBRARY)
