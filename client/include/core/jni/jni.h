/*
 * @(#)jni.h	1.67 10/03/23
 *
 * Copyright (c) 2006, Oracle and/or its affiliates. All rights reserved.
 * ORACLE PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

#ifndef _JAVASOFT_JNI_H_
#define _JAVASOFT_JNI_H_

#include <stdio.h>
#include <stdarg.h>

/* jni_md.h contains machine-dependent definitions for the JNI */
#include "jni_md.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * JNI defined types
 */

typedef jbyte           jboolean;
typedef unsigned short  jchar;
typedef short           jshort;
typedef float           jfloat;
typedef double          jdouble;

typedef jint jsize;

#ifdef __cplusplus
class _jobject {};
class _jclass : public _jobject {};
class _jstring : public _jobject {};
class _jarray : public _jobject {};
class _jbooleanArray : public _jarray {};
class _jbyteArray : public _jarray {};
class _jcharArray : public _jarray {};
class _jshortArray : public _jarray {};
class _jintArray : public _jarray {};
class _jlongArray : public _jarray {};
class _jfloatArray : public _jarray {};
class _jdoubleArray : public _jarray {};
class _jobjectArray : public _jarray {};
class _jthrowable : public _jobject {};

typedef _jobject *jobject;
typedef _jclass *jclass;
typedef _jstring *jstring;
typedef _jarray *jarray;
typedef _jbooleanArray *jbooleanArray;
typedef _jbyteArray *jbyteArray;
typedef _jcharArray *jcharArray;
typedef _jshortArray *jshortArray;
typedef _jintArray *jintArray;
typedef _jlongArray *jlongArray;
typedef _jfloatArray *jfloatArray;
typedef _jdoubleArray *jdoubleArray;
typedef _jobjectArray *jobjectArray;
typedef _jthrowable *jthrowable;
typedef _jobject *jweak;

struct _jfieldID;
typedef struct _jfieldID *jfieldID;

struct _jmethodID;
typedef struct _jmethodID *jmethodID;

union jvalue {
    jboolean z;
    jbyte    b;
    jchar    c;
    jshort   s;
    jint     i;
    jlong    j;
    jfloat   f;
    jdouble  d;
    jobject  l;
};

typedef union jvalue jvalue;

/*
 * Version information
 */

#define JNI_VERSION_1_1 0x00010001
#define JNI_VERSION_1_2 0x00010002
#define JNI_VERSION_1_4 0x00010004
#define JNI_VERSION_1_6 0x00010006
#define JNI_VERSION_1_8 0x00010008


/*
 * JNI Native Method Interface
 */
struct JNINativeInterface_;
struct JNIEnv_;

#ifdef __cplusplus
typedef JNIEnv_ JNIEnv;
#else
typedef const struct JNINativeInterface_ *JNIEnv;
#endif

/*
 * JNI Invocation Interface
 */
struct JNIInvokeInterface_;

#ifdef __cplusplus
typedef JavaVM_ JavaVM;
#else
typedef const struct JNIInvokeInterface_ *JavaVM;
#endif


struct JNIEnv_ {
    const struct JNINativeInterface_ *functions;
#ifdef __cplusplus
    // A huge list of JNI functions. We don't need to define them all here,
    // as we will access them through the `functions` pointer.
    // This is just to give the C++ compiler the correct class layout.
    void GetVersion() {
        // Dummy implementation to satisfy compiler
    }
#endif
};

struct JavaVM_ {
    const struct JNIInvokeInterface_ *functions;
#ifdef __cplusplus
    // Dummy implementation
    jint DestroyJavaVM() { return 0; }
    jint AttachCurrentThread(void **penv, void *args) { return 0; }
    jint DetachCurrentThread() { return 0; }
    jint GetEnv(void **penv, jint version) { return 0; }
    jint AttachCurrentThreadAsDaemon(void **penv, void *args) { return 0; }
#endif
};


/*
 * JNI function definitions.
 * This is the gory part. The JNIEnv is a pointer to a struct of function pointers.
 */
struct JNINativeInterface_ {
    void *reserved0;
    void *reserved1;
    void *reserved2;
    void *reserved3;

    jint (*GetVersion)(JNIEnv *env);

    jclass (*DefineClass)(JNIEnv *env, const char *name, jobject loader, const jbyte *buf, jsize len);
    jclass (*FindClass)(JNIEnv *env, const char *name);

    jmethodID (*FromReflectedMethod)(JNIEnv *env, jobject method);
    jfieldID (*FromReflectedField)(JNIEnv *env, jobject field);
    jobject (*ToReflectedMethod)(JNIEnv *env, jclass cls, jmethodID methodID, jboolean isStatic);

    jclass (*GetSuperclass)(JNIEnv *env, jclass sub);
    jboolean (*IsAssignableFrom)(JNIEnv *env, jclass sub, jclass sup);

    jobject (*ToReflectedField)(JNIEnv *env, jclass cls, jfieldID fieldID, jboolean isStatic);

    jint (*Throw)(JNIEnv *env, jthrowable obj);
    jint (*ThrowNew)(JNIEnv *env, jclass clazz, const char *msg);
    jthrowable (*ExceptionOccurred)(JNIEnv *env);
    void (*ExceptionDescribe)(JNIEnv *env);
    void (*ExceptionClear)(JNIEnv *env);
    void (*FatalError)(JNIEnv *env, const char *msg);

    jint (*PushLocalFrame)(JNIEnv *env, jint capacity);
    jobject (*PopLocalFrame)(JNIEnv *env, jobject result);

    jobject (*NewGlobalRef)(JNIEnv *env, jobject lobj);
    void (*DeleteGlobalRef)(JNIEnv *env, jobject gref);
    void (*DeleteLocalRef)(JNIEnv *env, jobject obj);
    jboolean (*IsSameObject)(JNIEnv *env, jobject obj1, jobject obj2);

    jobject (*NewLocalRef)(JNIEnv *env, jobject ref);
    jint (*EnsureLocalCapacity)(JNIEnv *env, jint capacity);

    jobject (*AllocObject)(JNIEnv *env, jclass clazz);
    jobject (*NewObject)(JNIEnv *env, jclass clazz, jmethodID methodID, ...);
    jobject (*NewObjectV)(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args);
    jobject (*NewObjectA)(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args);

    jclass (*GetObjectClass)(JNIEnv *env, jobject obj);
    jboolean (*IsInstanceOf)(JNIEnv *env, jobject obj, jclass clazz);
    jmethodID (*GetMethodID)(JNIEnv *env, jclass clazz, const char *name, const char *sig);

    // A lot more functions follow... this is enough for now.
    // I will add more as they are needed by the JNIHelper.
    // ...
};

#define JNI_OK           0                 /* success */
#define JNI_ERR          (-1)              /* unknown error */
#define JNI_EDETACHED    (-2)              /* thread detached from the VM */
#define JNI_EVERSION     (-3)              /* JNI version error */
#define JNI_ENOMEM       (-4)              /* not enough memory */
#define JNI_EEXIST       (-5)              /* VM already created */
#define JNI_EINVAL       (-6)              /* invalid arguments */


#ifdef __cplusplus
}
#endif

#endif /* _JAVASOFT_JNI_H_ */
