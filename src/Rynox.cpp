#include "Rynox.h"
#include <thread>
#include <chrono>

void initializeRynoxClient() {
    if (Client::isRunning) return;

    Client::isRunning = true;
    if (pthread_create(&Client::clientThread, nullptr, &runClient, nullptr) != 0) {
        std::cerr << "[Rynox] Failed to create client thread." << std::endl;
        Client::isRunning = false;
        return;
    }
    pthread_detach(Client::clientThread);
}

void shutdownRynoxClient() {
    if (!Client::isRunning) return;
    Client::isRunning = false;
}

void* runClient(void* arg) {
    jint attachResult = Client::jvm->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(&Client::env), nullptr);
    if (attachResult != JNI_OK || !Client::env) {
        std::cerr << "[Rynox] Failed to attach thread to JVM." << std::endl;
        Client::isRunning = false;
        return nullptr;
    }

    // Hook Minecraft internal Log4j
    jclass logManagerClass = Client::env->FindClass("org/apache/logging/log4j/LogManager");
    if (!logManagerClass) {
        Client::jvm->DetachCurrentThread();
        return nullptr;
    }

    jmethodID getLoggerMethod = Client::env->GetStaticMethodID(logManagerClass, "getLogger", "()Lorg/apache/logging/log4j/Logger;");
    if (!getLoggerMethod) {
        Client::jvm->DetachCurrentThread();
        return nullptr;
    }

    jobject rootLogger = Client::env->CallStaticObjectMethod(logManagerClass, getLoggerMethod);
    if (!rootLogger) {
        Client::jvm->DetachCurrentThread();
        return nullptr;
    }

    jclass loggerClass = Client::env->GetObjectClass(rootLogger);
    jmethodID infoMethod = Client::env->GetMethodID(loggerClass, "info", "(Ljava/lang/Object;)V");
    if (!infoMethod) {
        Client::jvm->DetachCurrentThread();
        return nullptr;
    }

    // Print "Rynox hook bypass" 10 times in internal logs
    for (int i = 0; i < 10; i++) {
        jstring message = Client::env->NewStringUTF("[Rynox] Rynox hook bypass");
        Client::env->CallVoidMethod(rootLogger, infoMethod, message);
        Client::env->DeleteLocalRef(message);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Original Minecraft player polling logic
    jclass minecraftClass = Client::env->FindClass("net/minecraft/client/Minecraft");
    if (!minecraftClass) {
        Client::jvm->DetachCurrentThread();
        return nullptr;
    }

    jmethodID getInstanceMethod = Client::env->GetStaticMethodID(minecraftClass, "getInstance", "()Lnet/minecraft/client/Minecraft;");
    jfieldID playerField = Client::env->GetFieldID(minecraftClass, "player", "Lnet/minecraft/client/player/LocalPlayer;");

    while (Client::isRunning) {
        jobject minecraftInstance = Client::env->CallStaticObjectMethod(minecraftClass, getInstanceMethod);
        if (minecraftInstance) {
            jobject playerInstance = Client::env->GetObjectField(minecraftInstance, playerField);
            if (playerInstance) Client::env->DeleteLocalRef(playerInstance);
            Client::env->DeleteLocalRef(minecraftInstance);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    Client::jvm->DetachCurrentThread();
    return nullptr;
}

// Agent entry points
extern "C" JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* vm, char* options, void* reserved) {
    Client::jvm = vm;
    initializeRynoxClient();
    return JNI_OK;
}

extern "C" JNIEXPORT void JNICALL Agent_OnUnload(JavaVM* vm) {
    shutdownRynoxClient();
}

// Agent_OnAttach for dynamic attachment via jattach
extern "C" JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM* vm, char* options, void* reserved) {
    Client::jvm = vm;
    initializeRynoxClient();
    return JNI_OK;
}
