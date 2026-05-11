#include "java_helper.h"
#include <cstdio>
#include <cassert>
#include <cstdint>
#include <jni.h>

static const jint JNI_VERSION = JNI_VERSION_24;

static JavaVM *g_vm = nullptr;

static JNIEnv* getEnv(bool *didAttach) {
    JNIEnv *env;
    jint state = g_vm->GetEnv((void**)&env, JNI_VERSION);
    if (state == JNI_EVERSION) {
        fprintf(stderr, "JNI version not supported!\n");
        return nullptr;
    }
    if (state == JNI_EDETACHED) {
        if (g_vm->AttachCurrentThread((void**)&env, NULL) != JNI_OK) {
            fprintf(stderr, "Could not attach current thread to JVM!\n");
            return nullptr;
        }
        *didAttach = true;
        return nullptr;
    }
    assert(state == JNI_OK);
    return env;
}

static jclass g_blockDataManagerCls = nullptr;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    g_vm = vm;
    fprintf(stderr, "[bedrock-seed] Cached JVM pointer! This is not an error.\n");

    bool didAttach = false;
    JNIEnv *env = getEnv(&didAttach);
    assert(!didAttach);
    if (env == nullptr) {
        return (jint)-1;
    }
    jclass cls = env->FindClass("bedrock/BlockDataManager");
    if (cls == NULL) {
        fprintf(stderr, "Class not found!\n");
        return (jint)-1;
    } else {
        g_blockDataManagerCls = (jclass)env->NewGlobalRef(cls);
    }

    return JNI_VERSION;
}

static jobject boxArg(JNIEnv* env, uint64_t arg) {
    jclass longCls = env->FindClass("java/lang/Long");
    jmethodID valueOf = env->GetStaticMethodID(longCls, "valueOf", "(J)Ljava/lang/Long;");
    jobject obj = env->CallStaticObjectMethod(longCls, valueOf, (jlong)arg);
    env->DeleteLocalRef(longCls);
    return obj;
}

template<typename T>
static jobject boxArg(JNIEnv* env, T) {
    static_assert(sizeof(T) == 0, "boxArg not implemented for this type!");
    return nullptr; // dummy return value
}

template<typename... Args>
void sendUpdate(const char *update, bool onlyLog, Args... args) {
    bool didAttach = false;
    JNIEnv *env = getEnv(&didAttach);
    if (env == nullptr) {
        return;
    }

    jmethodID mid = env->GetStaticMethodID(g_blockDataManagerCls, "sendUpdate", "(Ljava/lang/String;Z[Ljava/lang/Object;)V");
    if (mid == NULL) {
        fprintf(stderr, "Method not found!\n");
        if (didAttach) {
            g_vm->DetachCurrentThread();
        }
        return;
    }

    jstring j_update = env->NewStringUTF(update);
    jboolean j_onlyLog = static_cast<jboolean>(onlyLog);

    const int n = sizeof...(args);
    jclass objectClass = env->FindClass("java/lang/Object");
    jobjectArray array = env->NewObjectArray(n, objectClass, nullptr);
    jobject objs[] = {boxArg(env, args)...};
    for (int i = 0; i < n; ++i) {
        env->SetObjectArrayElement(array, i, objs[i]);
        env->DeleteLocalRef(objs[i]);
    }

    env->CallStaticVoidMethod(g_blockDataManagerCls, mid, j_update, j_onlyLog, array);

    // clean up
    env->DeleteLocalRef(j_update);
    env->DeleteLocalRef(objectClass);
    env->DeleteLocalRef(array);

    if (didAttach) {
        g_vm->DetachCurrentThread();
    }
}

// some stupid shit
template void sendUpdate<>(const char*, bool);
template void sendUpdate<uint64_t>(const char*, bool, uint64_t);
template void sendUpdate<uint64_t, uint64_t>(const char*, bool, uint64_t, uint64_t);
template void sendUpdate<uint64_t, uint64_t, uint64_t>(const char*, bool, uint64_t, uint64_t, uint64_t);
