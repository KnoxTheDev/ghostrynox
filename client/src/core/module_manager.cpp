#include "core/module_manager.h"
#include <algorithm>

namespace core {

ModuleManager& ModuleManager::get() {
    // Meyers' Singleton: thread-safe initialization guaranteed by the standard.
    static ModuleManager instance;
    return instance;
}

void ModuleManager::add_module(std::unique_ptr<Module> module) {
    modules.push_back(std::move(module));
}

Module* ModuleManager::get_module(const std::string& name) {
    auto it = std::find_if(modules.begin(), modules.end(), [&](const auto& module) {
        return module->get_name() == name;
    });

    if (it != modules.end()) {
        return it->get();
    }

    return nullptr;
}

#include "core/command_queue.h"

void ModuleManager::on_tick() {
    // Process any commands from the GUI thread first.
    core::CommandQueue::get().process_commands();

    // Then, tick all the active modules.
    for (const auto& module : modules) {
        if (module->is_enabled()) {
            module->on_tick();
        }
    }
}

} // namespace core
