#ifndef GHOSTCLIENT_MODULE_H
#define GHOSTCLIENT_MODULE_H

#include "categories.h"
#include <string>
#include <vector>
#include <atomic>

namespace core {

class Setting; // Forward declaration

/// @brief The base class for all client modules (features).
///
/// A module is a self-contained feature like Sprint, Aimbot, etc.
/// It has a name, description, category, and can be enabled or disabled.
/// It also holds a list of its own settings.
class Module {
public:
    Module(std::string name, std::string description, Category category);
    virtual ~Module() = default;

    // Non-copyable and non-movable to prevent slicing and ownership issues.
    Module(const Module&) = delete;
    Module& operator=(const Module&) = delete;

    /// @brief Called once when the module is enabled.
    virtual void on_enable() {}

    /// @brief Called once when the module is disabled.
    virtual void on_disable() {}

    /// @brief Called on every game tick when the module is enabled.
    virtual void on_tick() {}

    // Getters for module properties.
    const std::string& get_name() const { return name; }
    const std::string& get_description() const { return description; }
    Category get_category() const { return category; }
    bool is_enabled() const { return enabled; }
    const std::vector<Setting*>& get_settings() const { return settings; }

    /// @brief Toggles the module's state and calls on_enable/on_disable.
    void toggle();

protected:
    /// @brief A simple, "Jules-friendly" way for a derived module to register a setting.
    void add_setting(Setting& setting);

private:
    std::string name;
    std::string description;
    Category category;
    std::atomic<bool> enabled = false;
    std::vector<Setting*> settings;
};

} // namespace core

#endif // GHOSTCLIENT_MODULE_H
