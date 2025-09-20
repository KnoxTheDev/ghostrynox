#ifndef GHOSTCLIENT_SETTINGS_H
#define GHOSTCLIENT_SETTINGS_H

#include <string>
#include <vector>
#include <functional>

namespace core {

/// @brief Base class for all settings.
/// Provides common properties like name and description.
class Setting {
public:
    Setting(std::string name, std::string description)
        : name(std::move(name)), description(std::move(description)) {}
    virtual ~Setting() = default;

    const std::string& get_name() const { return name; }
    const std::string& get_description() const { return description; }

private:
    std::string name;
    std::string description;
};

/// @brief A setting that holds a boolean value (true/false).
class BoolSetting : public Setting {
public:
    BoolSetting(const std::string& name, const std::string& description, bool default_value)
        : Setting(name, description), value(default_value) {}

    bool get_value() const { return value; }
    void set_value(bool new_value) { value = new_value; }
    // Overload for easy toggling
    bool operator-() { value = !value; return value; }

private:
    std::atomic<bool> value;
};

/// @brief A setting that holds a value from a predefined list (enum).
template<typename T>
class EnumSetting : public Setting {
public:
    EnumSetting(const std::string& name, const std::string& description, T default_value, std::vector<T> options)
        : Setting(name, description), value(default_value), options(std::move(options)) {}

    T get_value() const { return value; }
    void set_value(T new_value) { value = new_value; }
    const std::vector<T>& get_options() const { return options; }

private:
    std::atomic<T> value;
    std::vector<T> options;
};

} // namespace core

#endif // GHOSTCLIENT_SETTINGS_H
