#ifndef GHOSTCLIENT_SPRINT_H
#define GHOSTCLIENT_SPRINT_H

#include "core/module.h"
#include "core/settings.h"
#include "core/hooking/hook.h"

namespace modules {

/// @brief The C++ implementation of the Sprint module.
/// This module automatically makes the player sprint, with configurable behavior.
class Sprint : public core::Module {
public:
    /// @brief Defines the sprinting behavior.
    enum class Mode {
        Strict,
        Rage
    };

    // --- Settings ---
    core::EnumSetting<Mode> mode;
    core::BoolSetting keep_sprint;
    core::BoolSetting unsprint_on_hit;
    core::BoolSetting unsprint_in_water;
    core::BoolSetting perma_sprint;

    Sprint();

    // --- Module Lifecycle ---
    void on_enable() override;
    void on_disable() override;

private:
    // --- JNI Logic ---
    // This function replicates the logic from shouldSprint() in the Java code using JNI.
    bool should_sprint_jni();

    // --- Hooks & Detours ---
    // These are the functions that will be called by our hooks.
    static void tick_detour(void* this_ptr);

    // The hook objects themselves.
    // We need to find a way to get the target address. For now, we assume a hypothetical function.
    // e.g., void* tick_address = find_address_of("ClientPlayerEntity::tick");
    // std::unique_ptr<hooking::Hook<void, void*>> tick_hook_;
};

} // namespace modules

#endif // GHOSTCLIENT_SPRINT_H
