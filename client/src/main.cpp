#include "core/jni/jni.h"
#include "core/jni/jni_helper.h"
#include "core/module_manager.h"
#include "modules/sprint.h"
#include "gui/gui_controller.h"
#include <iostream>
#include <memory>
#include <thread>

// This is a separate function to keep the client's logic separate from the JNI/IPC bootstrapping.
void initialize_client() {
    auto& mm = core::ModuleManager::get();
    mm.add_module(std::make_unique<modules::Sprint>());
    gui::init();
}

/// @brief The standard entry point for a JNI library.
/// The JVM calls this function when the library is loaded.
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    jni::JNIHelper::get().init(vm);
    initialize_client();
    return JNI_VERSION_1_8;
}

#include "core/self_destruct.h"

/// @brief Called by the JVM when the library is unloaded.
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
    self_destruct::trigger();
}
