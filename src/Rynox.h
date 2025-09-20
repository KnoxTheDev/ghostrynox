#ifndef RYNOX_H
#define RYNOX_H

#include <jni.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <pthread.h>

namespace Client {
    inline std::atomic_bool isRunning = false;
    inline pthread_t clientThread;
    inline JavaVM* jvm = nullptr;
    inline JNIEnv* env = nullptr;
}

void* runClient(void* arg);

void initializeRynoxClient();
void shutdownRynoxClient();

#endif //RYNOX_H
