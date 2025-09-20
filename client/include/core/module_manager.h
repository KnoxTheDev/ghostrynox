#ifndef GHOSTCLIENT_MODULE_MANAGER_H
#define GHOSTCLIENT_MODULE_MANAGER_H

#include "module.h"
#include <vector>
#include <memory>
#include <string>

namespace core {

/// @brief A singleton class to manage all modules in the client.
///
/// This class is responsible for storing, initializing, and providing
/// access to all modules.
class ModuleManager {
public:
    /// @brief Gets the singleton instance of the ModuleManager.
    static ModuleManager& get();

    // Delete copy/move constructors to enforce singleton pattern.
    ModuleManager(const ModuleManager&) = delete;
    ModuleManager& operator=(const ModuleManager&) = delete;

    /// @brief Adds a new module to the manager.
    /// The manager takes ownership of the module.
    void add_module(std::unique_ptr<Module> module);

    /// @brief Retrieves a module by its name.
    /// @param name The case-sensitive name of the module.
    /// @return A pointer to the module, or nullptr if not found.
    Module* get_module(const std::string& name);

    /// @brief Gets a list of all registered modules.
    const std::vector<std::unique_ptr<Module>>& get_modules() const { return modules; }

    /// @brief Called periodically to update all active modules.
    void on_tick();

private:
    ModuleManager() = default;
    ~ModuleManager() = default;

    std::vector<std::unique_ptr<Module>> modules;
};

} // namespace core

#endif // GHOSTCLIENT_MODULE_MANAGER_H
