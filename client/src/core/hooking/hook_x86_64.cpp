#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <sys/mman.h>
#include <cstdint>
#include <cstring>

// This file contains the implementation for x86_64 architecture.

namespace hooking::arch {

// A simple helper to change memory protection.
bool set_memory_protection(void* addr, size_t size, vm_prot_t prot) {
    kern_return_t kr = mach_vm_protect(mach_task_self(), (mach_vm_address_t)addr, size, FALSE, prot);
    return kr == KERN_SUCCESS;
}

bool install_hook(void* target, void* detour, void** trampoline, uint8_t* original_prologue, size_t prologue_size) {
    // --- 1. Prepare the JMP instruction ---
    // On x86_64, a relative JMP is 5 bytes: 0xE9 [32-bit relative offset]
    if (prologue_size < 5) return false; // Not enough space for the hook

    intptr_t relative_offset = (intptr_t)detour - ((intptr_t)target + 5);

    uint8_t jmp_instruction[5];
    jmp_instruction[0] = 0xE9; // JMP opcode
    memcpy(jmp_instruction + 1, &relative_offset, sizeof(int32_t));

    // --- 2. Create the trampoline ---
    // The trampoline will execute the original bytes and then jump back.
    // Allocate executable memory for it.
    mach_vm_address_t trampoline_addr;
    kern_return_t kr = mach_vm_allocate(mach_task_self(), &trampoline_addr, prologue_size + 5, VM_FLAGS_ANYWHERE);
    if (kr != KERN_SUCCESS) return false;

    *trampoline = (void*)trampoline_addr;

    // --- 3. Write to memory ---
    // Make the target function writable
    if (!set_memory_protection(target, prologue_size, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY)) {
        mach_vm_deallocate(mach_task_self(), trampoline_addr, prologue_size + 5);
        return false;
    }

    // Save original bytes
    memcpy(original_prologue, target, prologue_size);

    // Write the JMP hook
    memcpy(target, jmp_instruction, 5);

    // Restore original protection
    set_memory_protection(target, prologue_size, VM_PROT_READ | VM_PROT_EXECUTE);


    // --- 4. Write the trampoline ---
    // Make the trampoline memory writable
    set_memory_protection(*trampoline, prologue_size + 5, VM_PROT_READ | VM_PROT_WRITE);

    // Copy original bytes to trampoline
    memcpy(*trampoline, original_prologue, prologue_size);

    // Write a JMP from the trampoline back to the original function, after our hook
    intptr_t trampoline_jmp_offset = ((intptr_t)target + 5) - ((intptr_t)*trampoline + prologue_size + 5);
    uint8_t* trampoline_jmp_addr = (uint8_t*)*trampoline + prologue_size;
    trampoline_jmp_addr[0] = 0xE9;
    memcpy(trampoline_jmp_addr + 1, &trampoline_jmp_offset, sizeof(int32_t));

    // Make the trampoline executable
    set_memory_protection(*trampoline, prologue_size + 5, VM_PROT_READ | VM_PROT_EXECUTE);

    return true;
}

bool remove_hook(void* target, uint8_t* original_prologue, size_t prologue_size) {
    if (!set_memory_protection(target, prologue_size, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY)) {
        return false;
    }

    // Write the original bytes back
    memcpy(target, original_prologue, prologue_size);

    // Restore protection
    set_memory_protection(target, prologue_size, VM_PROT_READ | VM_PROT_EXECUTE);

    return true;
}

} // namespace hooking::arch
