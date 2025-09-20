#ifndef GHOSTCLIENT_GUI_CONTROLLER_H
#define GHOSTCLIENT_GUI_CONTROLLER_H

namespace gui {

/// @brief Initializes the GUI system and starts the dedicated GUI thread.
/// This should be called once when the client is loaded.
void init();

/// @brief Toggles the visibility of the GUI window.
/// This function is thread-safe and can be called from any thread (e.g., the game thread).
void toggle_visibility();

/// @brief Shuts down the GUI thread and releases all resources.
/// This should be called when the client is unloading.
void shutdown();

} // namespace gui

#endif // GHOSTCLIENT_GUI_CONTROLLER_H
