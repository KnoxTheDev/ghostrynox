#ifndef GHOSTCLIENT_HOOK_H
#define GHOSTCLIENT_HOOK_H

#include <cstdint>
#include <functional>

namespace hooking {

/// @brief An object representing a hook on a single function.
///
/// This class provides a high-level, type-safe interface for installing
/// and removing function hooks. It uses a trampoline to allow calling the
/// original function.
///
/// @tparam Ret The return type of the function to hook.
/// @tparam Args The argument types of the function to hook.
template <typename Ret, typename... Args>
class Hook {
public:
    using OriginalFn = Ret(*)(Args...);

    /// @param target_address The memory address of the function to hook.
    explicit Hook(void* target_address) : target_(target_address), installed_(false) {}
    ~Hook() {
        if (installed_) {
            remove();
        }
    }

    // Non-copyable, non-movable for simplicity. A hook is tied to an address.
    Hook(const Hook&) = delete;
    Hook& operator=(const Hook&) = delete;

    /// @brief Installs the hook, redirecting the target function to the detour.
    /// @param detour The function that will be called instead of the original.
    /// @return True if the hook was installed successfully, false otherwise.
    bool install(OriginalFn detour);

    /// @brief Removes the hook, restoring the original function.
    /// @return True if the hook was removed successfully, false otherwise.
    bool remove();

    /// @brief Calls the original, un-hooked function via a trampoline.
    Ret call_original(Args... args);

    bool is_installed() const { return installed_; }

private:
    void* target_ = nullptr;
    void* trampoline_ = nullptr; // A small piece of executable memory that calls the original function
    bool installed_ = false;

    // The raw bytes of the original function prologue that we overwrote.
    // We need these to restore the function when we remove the hook.
    // Let's assume a max size for the prologue patch for now.
    static constexpr int PROLOGUE_SIZE = 16;
    uint8_t original_prologue_[PROLOGUE_SIZE];
};

namespace arch {
    // Forward declare the arch-specific implementations.
    bool install_hook(void* target, void* detour, void** trampoline, uint8_t* original_prologue, size_t prologue_size);
    bool remove_hook(void* target, uint8_t* original_prologue, size_t prologue_size);
}

template <typename Ret, typename... Args>
bool Hook<Ret, Args...>::install(OriginalFn detour) {
    if (installed_ || !target_) return false;

    // Call the architecture-specific implementation.
    if (arch::install_hook(target_, (void*)detour, &trampoline_, original_prologue_, PROLOGUE_SIZE)) {
        installed_ = true;
        return true;
    }
    return false;
}

template <typename Ret, typename... Args>
bool Hook<Ret, Args...>::remove() {
    if (!installed_ || !target_) return false;

    if (arch::remove_hook(target_, original_prologue_, PROLOGUE_SIZE)) {
        installed_ = false;
        // In a real implementation, we would also deallocate the trampoline memory.
        return true;
    }
    return false;
}

template <typename Ret, typename... Args>
Ret Hook<Ret, Args...>::call_original(Args... args) {
    if (!installed_ || !trampoline_) {
        // This is a fatal error. We should probably throw or log.
        // For now, let's just return a default-constructed value.
        return Ret();
    }
    // Cast the trampoline to the correct function type and call it.
    return ((OriginalFn)trampoline_)(args...);
}

} // namespace hooking

#endif // GHOSTCLIENT_HOOK_H
