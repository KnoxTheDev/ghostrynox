#include "core/jni/jni_helper.h"
#include <iostream>

namespace jni {

JNIHelper& JNIHelper::get() {
    static JNIHelper instance;
    return instance;
}

void JNIHelper::init(JavaVM* vm) {
    vm_ = vm;
}

JNIEnv* JNIHelper::get_env() {
    if (!vm_) {
        // This should not happen if init() is called correctly from JNI_OnLoad.
        return nullptr;
    }

    JNIEnv* env = nullptr;
    jint result = vm_->functions->GetEnv(vm_, (void**)&env, JNI_VERSION_1_8);

    if (result == JNI_EDETACHED) {
        // The current thread is not attached to the JVM. Attach it.
        vm_->functions->AttachCurrentThread(vm_, (void**)&env, nullptr);
    }

    return env;
}

LocalRef<jclass> JNIHelper::find_class(const char* name) {
    JNIEnv* env = get_env();
    if (!env) return LocalRef<jclass>(nullptr, nullptr);

    // Check cache first
    {
        std::lock_guard<std::mutex> lock(cache_mutex_);
        if (class_cache_.count(name)) {
            // Create a new local reference from the cached global reference.
            jclass cached_class = class_cache_[name];
            return LocalRef<jclass>(env, (jclass)env->functions->NewLocalRef(env, cached_class));
        }
    }

    // If not in cache, find the class and add it to the cache as a global reference.
    jclass found_class = env->functions->FindClass(env, name);
    if (!found_class) {
        check_exception();
        return LocalRef<jclass>(nullptr, nullptr);
    }

    // Create a global reference for caching, as jclass is a local ref by default.
    jclass global_ref = (jclass)env->functions->NewGlobalRef(env, found_class);
    env->functions->DeleteLocalRef(env, found_class); // Clean up the local ref

    {
        std::lock_guard<std::mutex> lock(cache_mutex_);
        class_cache_[name] = global_ref;
    }

    return LocalRef<jclass>(env, (jclass)env->functions->NewLocalRef(env, global_ref));
}

jmethodID JNIHelper::get_method_id(jclass clazz, const char* name, const char* signature) {
    if (!clazz) return nullptr;
    // Simple caching for now. A more robust key would involve the class name.
    std::string key = std::string(name) + signature;
    std::lock_guard<std::mutex> lock(cache_mutex_);
    if (method_cache_.count(key)) {
        return method_cache_[key];
    }

    JNIEnv* env = get_env();
    if (!env) return nullptr;

    jmethodID id = env->functions->GetMethodID(env, clazz, name, signature);
    if (!id) {
        check_exception();
        return nullptr;
    }

    method_cache_[key] = id;
    return id;
}

jmethodID JNIHelper::get_static_method_id(jclass clazz, const char* name, const char* signature) {
    if (!clazz) return nullptr;
    std::string key = std::string(name) + signature; // Simplified key
    std::lock_guard<std::mutex> lock(cache_mutex_);
    if (method_cache_.count(key)) {
        return method_cache_[key];
    }

    JNIEnv* env = get_env();
    if (!env) return nullptr;

    jmethodID id = env->functions->GetStaticMethodID(env, clazz, name, signature);
    if (!id) {
        check_exception();
        return nullptr;
    }

    method_cache_[key] = id;
    return id;
}

jfieldID JNIHelper::get_field_id(jclass clazz, const char* name, const char* signature) {
    if (!clazz) return nullptr;
    std::string key = std::string(name) + signature; // Simplified key
    std::lock_guard<std::mutex> lock(cache_mutex_);
    if (field_cache_.count(key)) {
        return field_cache_[key];
    }

    JNIEnv* env = get_env();
    if (!env) return nullptr;

    jfieldID id = env->functions->GetFieldID(env, clazz, name, signature);
    if (!id) {
        check_exception();
        return nullptr;
    }

    field_cache_[key] = id;
    return id;
}

jfieldID JNIHelper::get_static_field_id(jclass clazz, const char* name, const char* signature) {
    if (!clazz) return nullptr;
    std::string key = std::string(name) + signature; // Simplified key
    std::lock_guard<std::mutex> lock(cache_mutex_);
    if (field_cache_.count(key)) {
        return field_cache_[key];
    }

    JNIEnv* env = get_env();
    if (!env) return nullptr;

    jfieldID id = env->functions->GetStaticFieldID(env, clazz, name, signature);
    if (!id) {
        check_exception();
        return nullptr;
    }

    field_cache_[key] = id;
    return id;
}


bool JNIHelper::check_exception() {
    JNIEnv* env = get_env();
    if (!env) return false;

    if (env->functions->ExceptionCheck(env)) {
        env->functions->ExceptionDescribe(env);
        env->functions->ExceptionClear(env);
        return true;
    }
    return false;
}

bool JNIHelper::get_method_location(jmethodID method, void** start_addr, void** end_addr) {
    jvmtiEnv* jvmti = get_jvmti_env(); // Assumes get_jvmti_env() is available
    if (!jvmti || !method) return false;

    jlocation start, end;
    jvmtiError err = jvmti->functions->GetMethodLocation(jvmti, method, &start, &end);

    if (err == JVMTI_ERROR_NONE) {
        *start_addr = (void*)start;
        *end_addr = (void*)end;
        return true;
    }

    // Silently fail if location not found.

    return false;
}

void JNIHelper::cleanup_global_refs() {
    JNIEnv* env = get_env();
    if (!env) return;

    std::lock_guard<std::mutex> lock(cache_mutex_);
    for (auto const& [key, val] : class_cache_) {
        env->functions->DeleteGlobalRef(env, val);
    }
    class_cache_.clear();
    method_cache_.clear(); // Method/field IDs don't need cleanup, but the cache should be cleared.
    field_cache_.clear();
}

jvmtiEnv* JNIHelper::get_jvmti_env() {
    if (!vm_) return nullptr;

    jvmtiEnv* jvmti = nullptr;
    vm_->functions->GetEnv(vm_, (void**)&jvmti, JVMTI_VERSION_1_8);

    return jvmti;
}

} // namespace jni
