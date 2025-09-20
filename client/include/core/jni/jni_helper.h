#ifndef GHOSTCLIENT_JNI_HELPER_H
#define GHOSTCLIENT_JNI_HELPER_H

#include "jni.h"
#include "jni_wrapper.h"
#include <string>
#include <unordered_map>
#include <mutex>

namespace jni {

/// @brief A singleton helper class for managing JNI interactions.
///
/// This class provides a high-level API for common JNI operations, including:
/// - Caching class, method, and field IDs for performance.
/// - Managing JNIEnv* pointers for multi-threading.
/// - RAII-based memory management for JNI objects via LocalRef.
/// - Basic exception checking.
class JNIHelper {
public:
    /// @brief Gets the singleton instance of the JNIHelper.
    static JNIHelper& get();

    // Delete copy/move constructors to enforce singleton pattern.
    JNIHelper(const JNIHelper&) = delete;
    JNIHelper& operator=(const JNIHelper&) = delete;

    /// @brief Initializes the helper with the JavaVM*. Must be called from JNI_OnLoad.
    void init(JavaVM* vm);

    /// @brief Gets the raw JavaVM* pointer.
    JavaVM* get_vm() { return vm_; }

    /// @brief Gets the JVMTI environment pointer.
    jvmtiEnv* get_jvmti_env();

    /// @brief Cleans up all cached JNI global references.
    void cleanup_global_refs();

    /// @brief Gets the JNIEnv* for the current thread. Attaches the thread if necessary.
    JNIEnv* get_env();

    /// @brief Finds a Java class by its name.
    /// @param name The fully qualified class name (e.g., "java/lang/String").
    /// @return A LocalRef-wrapped jclass, or an invalid ref if not found.
    LocalRef<jclass> find_class(const char* name);

    /// @brief Gets the ID of an instance method.
    jmethodID get_method_id(jclass clazz, const char* name, const char* signature);

    /// @brief Gets the ID of a static method.
    jmethodID get_static_method_id(jclass clazz, const char* name, const char* signature);

    /// @brief Gets the ID of an instance field.
    jfieldID get_field_id(jclass clazz, const char* name, const char* signature);

    /// @brief Gets the ID of a static field.
    jfieldID get_static_field_id(jclass clazz, const char* name, const char* signature);

    /// @brief Checks if a JNI exception has occurred.
    bool check_exception();

    /// @brief Gets the start and end memory location of a compiled method.
    /// @return True on success, false otherwise.
    bool get_method_location(jmethodID method, void** start_addr, void** end_addr);

private:
    JNIHelper() = default;
    ~JNIHelper() = default;

    JavaVM* vm_ = nullptr;
    std::mutex cache_mutex_;

    // Caches for performance
    std::unordered_map<std::string, jclass> class_cache_;
    std::unordered_map<std::string, jmethodID> method_cache_;
    std::unordered_map<std::string, jfieldID> field_cache_;
};

} // namespace jni

#endif // GHOSTCLIENT_JNI_HELPER_H
