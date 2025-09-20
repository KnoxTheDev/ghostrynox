/*
 * Copyright (c) 2003, 2011, Oracle and/or its affiliates. All rights reserved.
 * ORACLE PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

#ifndef _JAVA_JVMTI_H_
#define _JAVA_JVMTI_H_

#include "jni.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    JVMTI_VERSION_1   = 0x30010000,
    JVMTI_VERSION_1_0 = 0x30010000,
    JVMTI_VERSION_1_1 = 0x30010100,
    JVMTI_VERSION_1_2 = 0x30010200
};

typedef struct jvmtiEnv jvmtiEnv;

typedef struct {
    unsigned int can_tag_objects : 1;
    unsigned int can_generate_field_modification_events : 1;
    // ... many other capabilities
} jvmtiCapabilities;

typedef struct {
    jmethodID method;
    jint code_size;
    const void* code_addr;
    // ... other fields
} jvmtiCompiledMethodLoadRecord;

typedef jlong jlocation;

typedef enum {
    JVMTI_ERROR_NONE = 0,
    JVMTI_ERROR_INVALID_THREAD = 10,
    JVMTI_ERROR_INVALID_THREAD_GROUP = 11,
    JVMTI_ERROR_INVALID_PRIORITY = 12,
    JVMTI_ERROR_THREAD_NOT_SUSPENDED = 13,
    JVMTI_ERROR_THREAD_SUSPENDED = 14,
    JVMTI_ERROR_THREAD_NOT_ALIVE = 15,
    JVMTI_ERROR_INVALID_OBJECT = 20,
    JVMTI_ERROR_INVALID_CLASS = 21,
    JVMTI_ERROR_CLASS_NOT_PREPARED = 22,
    JVMTI_ERROR_INVALID_METHODID = 23,
    JVMTI_ERROR_INVALID_LOCATION = 24,
    JVMTI_ERROR_INVALID_FIELDID = 25,
    JVMTI_ERROR_NO_MORE_FRAMES = 31,
    JVMTI_ERROR_OPAQUE_FRAME = 32,
    JVMTI_ERROR_TYPE_MISMATCH = 34,
    JVMTI_ERROR_INVALID_SLOT = 35,
    JVMTI_ERROR_DUPLICATE = 40,
    JVMTI_ERROR_NOT_FOUND = 41,
    JVMTI_ERROR_INVALID_MONITOR = 50,
    JVMTI_ERROR_NOT_MONITOR_OWNER = 51,
    JVMTI_ERROR_INTERRUPT = 52,
    JVMTI_ERROR_INVALID_CLASS_FORMAT = 60,
    JVMTI_ERROR_CIRCULAR_CLASS_DEFINITION = 61,
    JVMTI_ERROR_FAILS_VERIFICATION = 62,
    JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_ADDED = 63,
    JVMTI_ERROR_UNSUPPORTED_REDEFINITION_SCHEMA_CHANGED = 64,
    JVMTI_ERROR_INVALID_TYPESTATE = 65,
    JVMTI_ERROR_UNSUPPORTED_REDEFINITION_HIERARCHY_CHANGED = 66,
    JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_DELETED = 67,
    JVMTI_ERROR_UNSUPPORTED_VERSION = 98,
    JVMTI_ERROR_NAMES_DONT_MATCH = 99,
    JVMTI_ERROR_UNSUPPORTED_REDEFINITION_CLASS_MODIFIERS_CHANGED = 100,
    JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_MODIFIERS_CHANGED = 101,
    JVMTI_ERROR_UNMODIFIABLE_CLASS = 102,
    JVMTI_ERROR_NOT_AVAILABLE = 98,
    JVMTI_ERROR_MUST_POSSESS_CAPABILITY = 99,
    JVMTI_ERROR_NULL_POINTER = 100,
    JVMTI_ERROR_ABSENT_INFORMATION = 101,
    JVMTI_ERROR_INVALID_EVENT_TYPE = 102,
    JVMTI_ERROR_ILLEGAL_ARGUMENT = 103,
    JVMTI_ERROR_NATIVE_METHOD = 104,
    JVMTI_ERROR_OUT_OF_MEMORY = 110,
    JVMTI_ERROR_ACCESS_DENIED = 111,
    JVMTI_ERROR_WRONG_PHASE = 112,
    JVMTI_ERROR_INTERNAL = 113
} jvmtiError;

struct _jvmti_interface_1 {
    void* reserved1;
    void* reserved2;
    // ... many reserved fields
    jint (*GetCompiledMethodLoadRecord)(jvmtiEnv* env, jmethodID method, jint max_records, jint* record_count, jvmtiCompiledMethodLoadRecord* records, jint* code_size);
    jint (*GetMethodLocation)(jvmtiEnv* env, jmethodID method, jlocation* start_location_ptr, jlocation* end_location_ptr);
    // ... many other functions
};


struct jvmtiEnv {
    const struct _jvmti_interface_1 *functions;
#ifdef __cplusplus
    // Dummy implementation for C++
    jint GetCompiledMethodLoadRecord(jmethodID method, jint max_records, jint* record_count, jvmtiCompiledMethodLoadRecord* records, jint* code_size) {
        return functions->GetCompiledMethodLoadRecord(this, method, max_records, record_count, records, code_size);
    }
#endif
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _JAVA_JVMTI_H_ */
