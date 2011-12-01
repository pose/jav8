LOCAL_PATH := $(call my-dir)

ANDROID_SOURCE		:= D:/Mobile/android-source
V8_HOME 			:= $(ANDROID_SOURCE)/external/v8

include $(CLEAR_VARS)

LOCAL_MODULE    	:= v8-prebuilt
LOCAL_SRC_FILES		:= $(TARGET_ARCH_ABI)/libv8.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE    	:= jav8
LOCAL_SRC_FILES 	:= jav8.cpp Wrapper.cpp Utils.cpp

LOCAL_CPP_FEATURES 	:= #rtti #exceptions
LOCAL_CFLAGS += \
	-Wno-endif-labels \
	-Wno-import \
	-Wno-format \
	-fno-exceptions \
	-DENABLE_DEBUGGER_SUPPORT \
	-DV8_NATIVE_REGEXP \
	-fvisibility=hidden \
	-DARM \
	-DV8_TARGET_ARCH_ARM \
	-DENABLE_LOGGING_AND_PROFILING
	
LOCAL_C_INCLUDES 		:= $(V8_HOME) $(V8_HOME)/include
LOCAL_STATIC_LIBRARIES 	:= v8-prebuilt
LOCAL_LDLIBS 			:= -llog

include $(BUILD_SHARED_LIBRARY)