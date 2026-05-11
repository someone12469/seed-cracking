#pragma once

#include <jni.h>

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved);

template<typename... Args>
void sendUpdate(const char *update, bool onlyLog, Args... args);
