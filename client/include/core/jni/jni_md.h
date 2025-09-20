#ifndef _JAVASOFT_JNI_MD_H_
#define _JAVASOFT_JNI_MD_H_

#include <stdint.h>

#define JNIEXPORT __attribute__((visibility("default")))
#define JNICALL

typedef int32_t jint;
typedef int64_t jlong;
typedef int8_t jbyte;

#endif /* !_JAVASOFT_JNI_MD_H_ */
