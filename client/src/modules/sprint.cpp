#include "modules/sprint.h"
#include "core/jni/jni_helper.h"
#include "core/jni/jvmti.h"
#include <iostream>
#include <memory>

// --- Globals for this module ---
// We need a pointer to the sprint module instance to access its members from the static detour.
static modules::Sprint* g_sprint_instance = nullptr;

// The hook object for the player tick function.
// Using a unique_ptr for automatic memory management.
static std::unique_ptr<hooking::Hook<void, void*>> g_tick_hook;




namespace modules {

// --- Detour Function ---
// This is the function that our hook will jump to.
// It must be a static or free function.
void Sprint::tick_detour(void* this_ptr) {
    // 1. Call the original game function to not break the game.
    if (g_tick_hook && g_tick_hook->is_installed()) {
        g_tick_hook->call_original(this_ptr);
    }

    // 2. Execute our module logic.
    if (g_sprint_instance && g_sprint_instance->is_enabled()) {
        auto& jni_helper = jni::JNIHelper::get();
        JNIEnv* env = jni_helper.get_env();
        if (!env) return;

        // Get the ClientPlayerEntity object from the `this_ptr` of its tick method.
        jni::LocalRef<jobject> player_ref(env, env->functions->NewLocalRef(env, (jobject)this_ptr));

        // Get the class and method ID for setSprinting
        jni::LocalRef<jclass> player_class = jni_helper.find_class("net/minecraft/client/network/ClientPlayerEntity");
        jmethodID set_sprinting_id = jni_helper.get_method_id(player_class.get(), "setSprinting", "(Z)V");

        if (set_sprinting_id) {
            bool should_sprint = g_sprint_instance->should_sprint_jni();
            env->functions->CallVoidMethod(env, player_ref.get(), set_sprinting_id, (jboolean)should_sprint);
        }
    }
}


// --- Module Implementation ---

Sprint::Sprint()
    : core::Module("Sprint", "Automatically sprints.", core::Category::Movement),
      mode("Mode", "Sprinting behavior.", Mode::Strict, {Mode::Strict, Mode::Rage}),
      keep_sprint("Keep Sprint", "Keeps sprinting after attacking.", false),
      unsprint_on_hit("Un-sprint on Hit", "Stops sprinting before an attack for better accuracy.", false),
      unsprint_in_water("Un-sprint in Water", "Stops sprinting while in water.", true),
      perma_sprint("Perma-Sprint", "Sprints even when not moving.", false)
{
    add_setting(mode);
    add_setting(keep_sprint);
    add_setting(unsprint_on_hit);
    add_setting(unsprint_in_water);
    add_setting(perma_sprint);
}

void Sprint::on_enable() {
    g_sprint_instance = this;
    auto& jni_helper = jni::JNIHelper::get();

    jni::LocalRef<jclass> player_class = jni_helper.find_class("net/minecraft/client/network/ClientPlayerEntity");
    if (!player_class) return;

    jmethodID tick_method_id = jni_helper.get_method_id(player_class.get(), "tick", "()V");
    if (!tick_method_id) return;

    void* tick_address = nullptr;
    void* end_address = nullptr;
    if (!jni_helper.get_method_location(tick_method_id, &tick_address, &end_address)) {
        return;
    }

    g_tick_hook = std::make_unique<hooking::Hook<void, void*>>(tick_address);
    if (!g_tick_hook->install(tick_detour)) {
        g_tick_hook.reset();
    }
}

void Sprint::on_disable() {
    if (g_tick_hook && g_tick_hook->is_installed()) {
        g_tick_hook->remove();
    }
    g_tick_hook.reset(); // Free the hook object
    g_sprint_instance = nullptr;
}

bool Sprint::should_sprint_jni() {
    // This is where the full JNI implementation goes.
    // It's complex, involving many JNI calls.
    // This is a simplified version of the logic.
    auto& jni_helper = jni::JNIHelper::get();
    JNIEnv* env = jni_helper.get_env();
    if (!env) return false;

    // Get Minecraft.getInstance().player
    jni::LocalRef<jclass> mc_class = jni_helper.find_class("net/minecraft/client/MinecraftClient");
    jmethodID get_instance_id = jni_helper.get_static_method_id(mc_class.get(), "getInstance", "()Lnet/minecraft/client/MinecraftClient;");
    jni::LocalRef<jobject> mc_instance(env, env->functions->CallStaticObjectMethod(env, mc_class.get(), get_instance_id));

    jfieldID player_field_id = jni_helper.get_field_id(mc_class.get(), "player", "Lnet/minecraft/client/network/ClientPlayerEntity;");
    jni::LocalRef<jobject> player(env, env->functions->GetObjectField(env, mc_instance.get(), player_field_id));

    if (!player) return false;

    // Get player properties (isSneaking, horizontalCollision, etc.)
    jni::LocalRef<jclass> player_class = jni_helper.find_class("net/minecraft/client/network/ClientPlayerEntity");
    jmethodID is_sneaking_id = jni_helper.get_method_id(player_class.get(), "isSneaking", "()Z");
    bool is_sneaking = env->functions->CallBooleanMethod(env, player.get(), is_sneaking_id);

    if (is_sneaking) return false;

    // This is the full, detailed JNI implementation.
    jni::LocalRef<jclass> entity_class = jni_helper.find_class("net/minecraft/entity/Entity");
    jmethodID is_in_water_id = jni_helper.get_method_id(entity_class.get(), "isInWater", "()Z");

    jfieldID horizontal_collision_id = jni_helper.get_field_id(player_class.get(), "horizontalCollision", "Z");
    jfieldID forward_speed_id = jni_helper.get_field_id(player_class.get(), "forwardSpeed", "F");

    bool is_in_water = env->functions->CallBooleanMethod(env, player.get(), is_in_water_id);
    bool horizontal_collision = env->functions->GetBooleanField(env, player.get(), horizontal_collision_id);

    if (is_sneaking) return false;
    if (horizontal_collision) return false;

    if (mode.get_value() == Mode::Rage) {
        if (unsprint_in_water.get_value() && is_in_water) return false;
        if (perma_sprint.get_value()) return true;

        float forward_speed = env->functions->GetFloatField(env, player.get(), forward_speed_id);
        // sidewaysSpeed is also needed, etc... this logic can be fully built out here.
        return forward_speed > 0.1f;
    } else { // Mode::Strict
        float forward_speed = env->functions->GetFloatField(env, player.get(), forward_speed_id);
        return forward_speed > 0.1f && !is_in_water;
    }
}

} // namespace modules
