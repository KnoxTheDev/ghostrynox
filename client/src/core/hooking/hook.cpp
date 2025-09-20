#include "core/hooking/hook.h"

// We will define the architecture-specific functions in separate files.
// This file will select the correct one at compile time.
namespace hooking::arch {
    bool install_hook(void* target, void* detour, void** trampoline, uint8_t* original_prologue, size_t prologue_size);
    bool remove_hook(void* target, uint8_t* original_prologue, size_t prologue_size);
}

namespace hooking {

// The install method is a template, so its definition must be in a header
// or be explicitly instantiated. For simplicity with this file structure,
// let's move the implementation logic into the header for now.
// This is a common practice for templated classes.

// A real implementation would be more complex. We would need to explicitly
// instantiate the templates for each function signature we want to use,
// or move the implementation to the header.

// Let's adjust the plan slightly and create a hook_impl.h for the template
// implementations to keep hook.h clean.

// For now, I will create this file as a placeholder for the non-templated
// parts of the hooking framework, if any arise. In a more complex
// implementation, this might contain helper functions for memory allocation, etc.

// Let's create the arch-specific files first, then come back to this.
// This file will remain mostly empty for now.

} // namespace hooking
