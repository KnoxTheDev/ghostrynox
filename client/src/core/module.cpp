#include "core/module.h"
#include "core/settings.h"

namespace core {

Module::Module(std::string name, std::string description, Category category)
    : name(std::move(name)),
      description(std::move(description)),
      category(category) {
}

void Module::toggle() {
    // Atomically toggle the enabled state.
    bool was_enabled = enabled.exchange(!enabled);

    if (!was_enabled) { // If it was disabled, it's now enabled.
        on_enable();
    } else {
        on_disable();
    }
}

void Module::add_setting(Setting& setting) {
    settings.push_back(&setting);
}

} // namespace core
