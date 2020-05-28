#ifdef __ANDROID__

#include <iostream>
#include <android/log.h>

#define LOG_TAG "StormView"
#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

#endif
