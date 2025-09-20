#include "core/self_destruct.h"
#include "core/module_manager.h"
#include "core/jni/jni_helper.h"
#include "gui/gui_controller.h"
#include <iostream>
#include <atomic>

namespace {
    std::atomic<bool> g_self_destruct_initiated = false;
}

namespace self_destruct {

void trigger() {
    // Ensure the sequence runs only once.
    if (g_self_destruct_initiated.exchange(true)) {
        return;
    }

    // 1. Disable all active modules to remove their hooks.
    auto& module_manager = core::ModuleManager::get();
    for (const auto& module : module_manager.get_modules()) {
        if (module->is_enabled()) {
            module->toggle(); // This will call on_disable()
        }
    }

    // 2. Shut down the GUI thread.
    gui::shutdown();

    // 3. Clean up all JNI global references.
    jni::JNIHelper::get().cleanup_global_refs();
}

} // namespace self_destruct
