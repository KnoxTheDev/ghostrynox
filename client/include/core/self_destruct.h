#ifndef GHOSTCLIENT_SELF_DESTRUCT_H
#define GHOSTCLIENT_SELF_DESTRUCT_H

namespace self_destruct {

/// @brief Triggers the self-destruct sequence.
///
/// This function is the main entry point for cleanly unloading the client.
/// It disables all modules, removes all hooks, cleans up JNI references,
/// and shuts down all client threads. This makes the client completely inert.
/// This function is thread-safe.
void trigger();

} // namespace self_destruct

#endif // GHOSTCLIENT_SELF_DESTRUCT_H
