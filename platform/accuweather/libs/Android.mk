LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# libz
LOCAL_MODULE := libz
LOCAL_CPPFLAGS := -std=c++14 -frtti -fexceptions -g -fPIC
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libz.a

include $(PREBUILT_STATIC_LIBRARY)

# libpng
include $(CLEAR_VARS)
LOCAL_MODULE := libpng
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libpng.a
LOCAL_CPPFLAGS := -std=c++14 -frtti -fexceptions -g -fPIC
LOCAL_STATIC_LIBRARIES := libz

include $(PREBUILT_STATIC_LIBRARY)


#libcurl with OpenSSL
include $(CLEAR_VARS)
LOCAL_MODULE := libcurl
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libcurl.a
LOCAL_EXPORT_C_INCLUDES := libcurl/include

include $(PREBUILT_STATIC_LIBRARY)


#libviewer
include $(CLEAR_VARS)
DIR_CXX := ../../stormview/lib/src/
LOCAL_MODULE := libstormview
LOCAL_CPPFLAGS := -std=c++14 -frtti -fexceptions -fPIC -Wall
LOCAL_C_INCLUDES += $(DIR_CXX)
LOCAL_C_INCLUDES += $(DIR_CXX)/networking
LOCAL_C_INCLUDES += $(DIR_CXX)/particles
LOCAL_C_INCLUDES += $(DIR_CXX)/radar
LOCAL_C_INCLUDES += $(DIR_CXX)/skyguard
LOCAL_C_INCLUDES += $(DIR_CXX)/glow
LOCAL_C_INCLUDES += $(DIR_CXX)/ext
LOCAL_C_INCLUDES += includes/
#LOCAL_C_INCLUDES += /develop/androidSDK/ndk-bundle/sysroot/usr/include/
LOCAL_C_INCLUDES += includes/png/
LOCAL_C_INCLUDES += includes/glm
LOCAL_C_INCLUDES += includes/z
LOCAL_SRC_FILES := $(wildcard $(DIR_CXX)/*.cxx) \
$(wildcard $(DIR_CXX)/networking/*.cxx) \
$(wildcard $(DIR_CXX)/particles/*.cxx) \
$(wildcard $(DIR_CXX)/radar/*.cxx) \
$(wildcard $(DIR_CXX)/skyguard/*.cxx) \
$(wildcard $(DIR_CXX)/glow/*.cxx) \
$(wildcard $(DIR_CXX)/ext/*.cxx) \

LOCAL_STATIC_LIBRARIES := libcurl libz libpng

include $(BUILD_STATIC_LIBRARY)
