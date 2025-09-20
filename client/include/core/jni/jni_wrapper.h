#ifndef GHOSTCLIENT_JNI_WRAPPER_H
#define GHOSTCLIENT_JNI_WRAPPER_H

#include "jni.h"
#include <utility>

namespace jni {

// Forward declaration
class JNIHelper;

/// @brief A RAII wrapper for JNI local references (jobject, jclass, etc.).
///
/// This class automatically calls DeleteLocalRef when it goes out of scope,
/// preventing memory leaks which are common in JNI programming.
template <typename T>
class LocalRef {
public:
    LocalRef(JNIEnv* env, T ref) : env(env), ref(ref) {}

    // Destructor is the key to RAII. It cleans up the JNI reference.
    ~LocalRef() {
        if (env && ref) {
            env->functions->DeleteLocalRef(env, ref);
        }
    }

    // Non-copyable to prevent double-freeing the reference.
    LocalRef(const LocalRef&) = delete;
    LocalRef& operator=(const LocalRef&) = delete;

    // Movable to allow transferring ownership.
    LocalRef(LocalRef&& other) noexcept : env(other.env), ref(other.ref) {
        other.env = nullptr;
        other.ref = nullptr;
    }
    LocalRef& operator=(LocalRef&& other) noexcept {
        if (this != &other) {
            if (env && ref) {
                env->functions->DeleteLocalRef(env, ref);
            }
            env = other.env;
            ref = other.ref;
            other.env = nullptr;
            other.ref = nullptr;
        }
        return *this;
    }

    /// @brief Gets the underlying JNI reference.
    T get() const { return ref; }

    /// @brief Checks if the reference is valid (not null).
    explicit operator bool() const { return ref != nullptr; }

private:
    JNIEnv* env;
    T ref;
};

} // namespace jni

#endif // GHOSTCLIENT_JNI_WRAPPER_H
