#pragma once

#include <cstdint>

// #include "IUnityInterface.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <type_traits>
#include <vector>

// Preprocessor Macro Definitions
#define ENABLE_POLYSPATIAL_IPC_LOGGING 1

#if ENABLE_POLYSPATIAL_IPC_LOGGING
    #include <cstdio>
    #define POLYSPATIAL_IPC_LOG(__tag__, ...) printf("PolySpatial IPC [" __tag__ "]: " __VA_ARGS__)
#else
    #define POLYSPATIAL_IPC_LOG(...)
#endif

#define LOG_DEBUG(...) POLYSPATIAL_IPC_LOG("DEBUG", __VA_ARGS__)

#define LOG_VERBOSE(...) POLYSPATIAL_IPC_LOG("VERBOSE", __VA_ARGS__)

#define LOG_WARNING(...) POLYSPATIAL_IPC_LOG("WARNING", __VA_ARGS__)

#define LOG_ERROR(...) POLYSPATIAL_IPC_LOG("ERROR", __VA_ARGS__)

#define LOG_FATAL(...) POLYSPATIAL_IPC_LOG("FATAL", __VA_ARGS__)

#ifdef ANDROID
#include <android/log.h>
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, "polyspatial_sharedmemory", __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, "polyspatial_sharedmemory", __VA_ARGS__)
#ifdef NDEBUG
#define ALOGV(...) 
#define ALOGI(...) 
#else
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "polyspatial_sharedmemory", __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, "polyspatial_sharedmemory", __VA_ARGS__)
#endif
#endif//ANDROID

/**
 * Provides a simpler way to generate Managed to Native interface definitions.
 */
#define EXPORT(RETURN_TYPE) extern "C" RETURN_TYPE UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
