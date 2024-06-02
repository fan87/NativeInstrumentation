#include <cstring>
#include "javah/me_fan87_nativeinstrumentation_NativeInstrumentation.h"
#include "jvmti.h"
#include "type_convert.h"

#if (defined __linux__ && defined(MEMCHECK) && defined(__GNUC__))
#define DO_MEM_CHECK
#endif

#ifdef DO_MEM_CHECK
#include "mcheck.h"
#define MTRACE_START \
//    mtrace();
#define MTRACE_END \
//    muntrace();
#else
#define MTRACE_START
#define MTRACE_END
#endif


jboolean
checkForAndClearThrowable(  JNIEnv *    jnienv) {
    jboolean result = (*jnienv).ExceptionCheck();
    if ( result ) {
        (*jnienv).ExceptionClear();
    }
    return result;
}
void* allocate(jvmtiEnv *jvmtienv, size_t bytecount) {
    void *          resultBuffer    = nullptr;
    jvmtiError      error           = JVMTI_ERROR_NONE;

    error = (*jvmtienv).Allocate(bytecount,
                                 (unsigned char**) &resultBuffer);
    /* may be called from any phase */
    if ( error != JVMTI_ERROR_NONE ) {
        resultBuffer = nullptr;
    }
    return resultBuffer;
}
void deallocate(jvmtiEnv * jvmtienv, void * buffer) {
    jvmtiError  error = JVMTI_ERROR_NONE;

    error = (*jvmtienv).Deallocate((unsigned char*)buffer);
}
jboolean checkForThrowable(JNIEnv *jnienv) {
    return (*jnienv).ExceptionCheck();
}
void createAndThrowThrowableFromJVMTIErrorCode(JNIEnv * jnienv, jvmtiError errorCode) {
    if (errorCode == 0) {
        return;
    }
    const char * throwableClassName = nullptr;
    const char * message            = nullptr;
    jstring messageString           = nullptr;

    switch ( errorCode ) {
        case JVMTI_ERROR_NULL_POINTER:
            throwableClassName = "java/lang/NullPointerException";
            break;

        case JVMTI_ERROR_ILLEGAL_ARGUMENT:
            throwableClassName = "java/lang/IllegalArgumentException";
            break;

        case JVMTI_ERROR_OUT_OF_MEMORY:
            throwableClassName = "java/lang/OutOfMemoryError";
            break;

        case JVMTI_ERROR_CIRCULAR_CLASS_DEFINITION:
            throwableClassName = "java/lang/ClassCircularityError";
            break;

        case JVMTI_ERROR_FAILS_VERIFICATION:
            throwableClassName = "java/lang/VerifyError";
            break;

        case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_ADDED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "class redefinition failed: attempted to add a method";
            break;

        case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_SCHEMA_CHANGED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "class redefinition failed: attempted to change the schema (add/remove fields)";
            break;

        case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_HIERARCHY_CHANGED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "class redefinition failed: attempted to change superclass or interfaces";
            break;

        case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_DELETED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "class redefinition failed: attempted to delete a method";
            break;

        case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_CLASS_MODIFIERS_CHANGED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "class redefinition failed: attempted to change the class modifiers";
            break;

        case JVMTI_ERROR_UNSUPPORTED_REDEFINITION_METHOD_MODIFIERS_CHANGED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "class redefinition failed: attempted to change method modifiers";
            break;

        case JVMTI_ERROR_UNSUPPORTED_VERSION:
            throwableClassName = "java/lang/UnsupportedClassVersionError";
            break;

        case JVMTI_ERROR_NAMES_DONT_MATCH:
            throwableClassName = "java/lang/NoClassDefFoundError";
            message = "class names don't match";
            break;

        case JVMTI_ERROR_INVALID_CLASS_FORMAT:
            throwableClassName = "java/lang/ClassFormatError";
            break;

        case JVMTI_ERROR_UNMODIFIABLE_CLASS:
            throwableClassName = "java/lang/instrument/UnmodifiableClassException";
            break;

        case JVMTI_ERROR_INVALID_CLASS:
            throwableClassName = "java/lang/InternalError";
            message = "class redefinition failed: invalid class";
            break;

        case JVMTI_ERROR_CLASS_LOADER_UNSUPPORTED:
            throwableClassName = "java/lang/UnsupportedOperationException";
            message = "unsupported operation";
            break;

        case JVMTI_ERROR_INTERNAL:
        default:
            throwableClassName = "java/lang/InternalError";
            break;
    }

    jnienv->ThrowNew(jnienv->FindClass(throwableClassName), message);

}

#define jplis_assert(cond) if (!(cond)) error(jnienv, "condition failed: %s", #cond);

#define EXCEPTION_ILLEGAL_STATE "java/lang/IllegalStateException"
#define EXCEPTION_NO_SUCH_METHOD "java/lang/NoSuchMethodException"
#define EXCEPTION_NO_SUCH_FIELD "java/lang/NoSuchFieldException"
#define ERROR_MAX_LEN 4096

void error(JNIEnv *jnienv, const char *__restrict format, ...) {
    char buf[ERROR_MAX_LEN];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
    jnienv->ThrowNew(jnienv->FindClass(EXCEPTION_ILLEGAL_STATE), buf);
}
void error_custom(JNIEnv *jnienv, const char *exception, const char *format, ...) {
    char buf[ERROR_MAX_LEN];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

    jnienv->ThrowNew(jnienv->FindClass(exception), buf);
}
jvmtiEnv* jvmti1 = nullptr;
jvmtiEnv* GetJvmTiEnv(JNIEnv *jnienv) {
    if (jvmti1 == nullptr) {
        JavaVM *vm;
        JNIEnv *jni;
        jvmtiEnv *jvmti0;
        jsize size;
        if (JNI_GetCreatedJavaVMs(&vm, 1, &size) != JNI_OK || size != 1) {
            error(jnienv, "Failed to GetCreatedJavaVMs");
        }
        jint result = vm->GetEnv((void**) &jni, JNI_VERSION_1_6);
        if (result == JNI_EDETACHED) {
            result = vm->AttachCurrentThread((void**) &jni, nullptr);
            if (result != JNI_OK) {
                error(jnienv, "Failed to attach thread");
            }
        }
        result = vm->GetEnv((void**) &jvmti0, JVMTI_VERSION_1_0);
        if (result != JNI_OK) {
            error(jnienv, "Failed to get JvmTi");
        }
        jvmtiCapabilities capabilities = {};
        capabilities.can_retransform_classes = 1;
        capabilities.can_retransform_any_class = 1;
        capabilities.can_redefine_classes = 1;
        capabilities.can_redefine_any_class = 1;
        capabilities.can_generate_all_class_hook_events = 1;
        jvmti0->AddCapabilities(&capabilities);
        jvmti1 = jvmti0;
    }

    return jvmti1;
}


jboolean Java_me_fan87_nativeinstrumentation_NativeInstrumentation_isModifiableClass0(JNIEnv *env, jclass clazz, jclass) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(env, clazz);
    jvmtiEnv *          jvmtienv = GetJvmTiEnv(env);
    jvmtiError          jvmtierror;
    jboolean            is_modifiable = JNI_FALSE;

    jvmtierror = (*jvmtienv).IsModifiableClass( clazz,
                                                 &is_modifiable);
    createAndThrowThrowableFromJVMTIErrorCode(env, jvmtierror);
    return is_modifiable;
}

void Java_me_fan87_nativeinstrumentation_NativeInstrumentation_setHasRetransformableTransformers(JNIEnv *env, jclass clazz, jboolean has) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(env, clazz);
    (*GetJvmTiEnv(env)).SetEventNotificationMode(
            has? JVMTI_ENABLE : JVMTI_DISABLE,
            JVMTI_EVENT_CLASS_FILE_LOAD_HOOK,
            nullptr /* all threads */);
}

void Java_me_fan87_nativeinstrumentation_NativeInstrumentation_retransformClasses0(JNIEnv *jnienv, jclass clazz, jobjectArray classes) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(jnienv, clazz);
    jvmtiEnv *  retransformerEnv     = GetJvmTiEnv(jnienv);
    jboolean    errorOccurred        = JNI_FALSE;
    jvmtiError  errorCode            = JVMTI_ERROR_NONE;
    jsize       numClasses           = 0;
    jclass *    classArray           = nullptr;


    /* This was supposed to be checked by caller too */
    if (!errorOccurred && classes == nullptr) {
        jplis_assert(classes != nullptr);
        errorOccurred = JNI_TRUE;
        errorCode = JVMTI_ERROR_NULL_POINTER;
    }

    if (!errorOccurred) {
        numClasses = (*jnienv).GetArrayLength(classes);
        errorOccurred = checkForThrowable(jnienv);
        jplis_assert(!errorOccurred);

        if (!errorOccurred && numClasses == 0) {
            jplis_assert(numClasses != 0);
            errorOccurred = JNI_TRUE;
            errorCode = JVMTI_ERROR_NULL_POINTER;
        }
    }

    if (!errorOccurred) {
        classArray = (jclass *) allocate(retransformerEnv,
                                         numClasses * sizeof(jclass));
        errorOccurred = (classArray == nullptr);
        jplis_assert(!errorOccurred);
        if (errorOccurred) {
            errorCode = JVMTI_ERROR_OUT_OF_MEMORY;
        }
    }

    if (!errorOccurred) {
        jint index;
        for (index = 0; index < numClasses; index++) {
            classArray[index] = static_cast<jclass>((*jnienv).GetObjectArrayElement(classes, index));
            errorOccurred = checkForThrowable(jnienv);
            jplis_assert(!errorOccurred);
            if (errorOccurred) {
                break;
            }

            if (classArray[index] == nullptr) {
                jplis_assert(classArray[index] != nullptr);
                errorOccurred = JNI_TRUE;
                errorCode = JVMTI_ERROR_NULL_POINTER;
                break;
            }
        }
    }

    if (!errorOccurred) {
        errorCode = (*retransformerEnv).RetransformClasses(
                                                            numClasses, classArray);
        errorOccurred = (errorCode != JVMTI_ERROR_NONE);
    }

    /* Give back the buffer if we allocated it.  Throw any exceptions after.
     */
    if (classArray != nullptr) {
        deallocate(retransformerEnv, (void*)classArray);
    }

    if (errorCode != JVMTI_ERROR_NONE) {
        createAndThrowThrowableFromJVMTIErrorCode(jnienv, errorCode);
    }

//    mapThrownThrowableIfNecessary(jnienv, redefineClassMapper);
}

void Java_me_fan87_nativeinstrumentation_NativeInstrumentation_redefineClasses0(JNIEnv *jnienv, jclass clazz, jobjectArray classDefinitions) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(jnienv, clazz);
    jvmtiEnv*   jvmtienv                        = GetJvmTiEnv(jnienv);
    jboolean    errorOccurred                   = JNI_FALSE;
    jclass      classDefClass                   = nullptr;
    jmethodID   getDefinitionClassMethodID      = nullptr;
    jmethodID   getDefinitionClassFileMethodID  = nullptr;
    jvmtiClassDefinition* classDefs             = nullptr;
    jbyteArray* targetFiles                     = nullptr;
    jsize       numDefs                         = 0;

    jplis_assert(classDefinitions != nullptr);

    numDefs = jnienv->GetArrayLength(classDefinitions);
    errorOccurred = checkForThrowable(jnienv);
    jplis_assert(!errorOccurred);

    if (!errorOccurred) {
        jplis_assert(numDefs > 0);
        /* get method IDs for methods to call on class definitions */
        classDefClass = jnienv->FindClass("java/lang/instrument/ClassDefinition");
        errorOccurred = checkForThrowable(jnienv);
        jplis_assert(!errorOccurred);
    }

    if (!errorOccurred) {
        getDefinitionClassMethodID = jnienv->GetMethodID(
                                                                classDefClass,
                                                                "getDefinitionClass",
                                                                "()Ljava/lang/Class;");
        errorOccurred = checkForThrowable(jnienv);
        jplis_assert(!errorOccurred);
    }

    if (!errorOccurred) {
        getDefinitionClassFileMethodID = jnienv->GetMethodID(
                                                                    classDefClass,
                                                                    "getDefinitionClassFile",
                                                                    "()[B");
        errorOccurred = checkForThrowable(jnienv);
        jplis_assert(!errorOccurred);
    }

    if (!errorOccurred) {
        classDefs = (jvmtiClassDefinition *) allocate(
                jvmtienv,
                numDefs * sizeof(jvmtiClassDefinition));
        errorOccurred = (classDefs == nullptr);
        jplis_assert(!errorOccurred);
        if ( errorOccurred ) {
            createAndThrowThrowableFromJVMTIErrorCode(jnienv, JVMTI_ERROR_OUT_OF_MEMORY);
        }

        else {
            /*
             * We have to save the targetFile values that we compute so
             * that we can release the class_bytes arrays that are
             * returned by GetByteArrayElements(). In case of a JNI
             * error, we can't (easily) recompute the targetFile values
             * and we still want to free any memory we allocated.
             */
            targetFiles = (jbyteArray *) allocate(jvmtienv,
                                                  numDefs * sizeof(jbyteArray));
            errorOccurred = (targetFiles == nullptr);
            jplis_assert(!errorOccurred);
            if ( errorOccurred ) {
                deallocate(jvmtienv, (void*)classDefs);
                createAndThrowThrowableFromJVMTIErrorCode(jnienv,
                                                          JVMTI_ERROR_OUT_OF_MEMORY);
            }
            else {
                jint i, j;

                // clear classDefs so we can correctly free memory during errors
                memset(classDefs, 0, numDefs * sizeof(jvmtiClassDefinition));

                for (i = 0; i < numDefs; i++) {
                    jclass      classDef    = nullptr;

                    classDef = static_cast<jclass>(jnienv->GetObjectArrayElement(classDefinitions, i));
                    errorOccurred = checkForThrowable(jnienv);
                    jplis_assert(!errorOccurred);
                    if (errorOccurred) {
                        break;
                    }

                    classDefs[i].klass = static_cast<jclass>(jnienv->CallObjectMethod(classDef,
                                                                                      getDefinitionClassMethodID));
                    errorOccurred = checkForThrowable(jnienv);
                    jplis_assert(!errorOccurred);
                    if (errorOccurred) {
                        break;
                    }

                    targetFiles[i] = static_cast<jbyteArray>(jnienv->CallObjectMethod(classDef,
                                                                                      getDefinitionClassFileMethodID));
                    errorOccurred = checkForThrowable(jnienv);
                    jplis_assert(!errorOccurred);
                    if (errorOccurred) {
                        break;
                    }

                    classDefs[i].class_byte_count = jnienv->GetArrayLength(targetFiles[i]);
                    errorOccurred = checkForThrowable(jnienv);
                    jplis_assert(!errorOccurred);
                    if (errorOccurred) {
                        break;
                    }

                    /*
                     * Allocate class_bytes last so we don't have to free
                     * memory on a partial row error.
                     */
                    classDefs[i].class_bytes = (unsigned char*)jnienv->GetByteArrayElements(targetFiles[i], nullptr);
                    errorOccurred = checkForThrowable(jnienv);
                    jplis_assert(!errorOccurred);
                    if (errorOccurred) {
                        break;
                    }
                }

                if (!errorOccurred) {
                    jvmtiError  errorCode = JVMTI_ERROR_NONE;
                    errorCode = jvmtienv->RedefineClasses(numDefs, classDefs);
                    if (errorCode == JVMTI_ERROR_WRONG_PHASE) {
                        /* insulate caller from the wrong phase error */
                        errorCode = JVMTI_ERROR_NONE;
                    } else {
                        errorOccurred = (errorCode != JVMTI_ERROR_NONE);
                        if ( errorOccurred ) {
                            createAndThrowThrowableFromJVMTIErrorCode(jnienv, errorCode);
                        }
                    }
                }

                /*
                 * Cleanup memory that we allocated above. If we had a
                 * JNI error, a JVM/TI error or no errors, index 'i'
                 * tracks how far we got in processing the classDefs
                 * array. Note:  ReleaseByteArrayElements() is safe to
                 * call with a JNI exception pending.
                 */
                for (j = 0; j < i; j++) {
                    if ((jbyte *)classDefs[j].class_bytes != nullptr) {
                        jnienv->ReleaseByteArrayElements(
                                                            targetFiles[j], (jbyte *)classDefs[j].class_bytes,
                                                            0 /* copy back and free */);
                        /*
                         * Only check for error if we didn't already have one
                         * so we don't overwrite errorOccurred.
                         */
                        if (!errorOccurred) {
                            errorOccurred = checkForThrowable(jnienv);
                            jplis_assert(!errorOccurred);
                        }
                    }
                }
                deallocate(jvmtienv, (void*)targetFiles);
                deallocate(jvmtienv, (void*)classDefs);
            }
        }
    }

//    mapThrownThrowableIfNecessary(jnienv, redefineClassMapper);

}

jclass nativeInstrumentation;
jmethodID transform;
jfieldID transformOutput;

bool initialized = false;

void classFileLoad(jvmtiEnv *jvmti_env,
                   JNIEnv* jni_env,
                   jclass class_being_redefined,
                   jobject loader,
                   const char* name,
                   jobject protection_domain,
                   jint class_data_len,
                   const unsigned char* class_data,
                   jint* new_class_data_len,
                   unsigned char** new_class_data) {
    if (nativeInstrumentation == nullptr) {
        printf("Class not found! Loading %s\n", name);
        *new_class_data_len = class_data_len;
        void * bytes = allocate(jvmti_env, class_data_len + 1);
        memcpy(bytes, class_data, class_data_len);
        *new_class_data = (unsigned char*) bytes;
        return;
    }
    if (transform == nullptr) {
        error(jni_env, "method not found\n");
        return;
    }
    jbyteArray jbyteArray1 = jni_env->NewByteArray(class_data_len);
    jni_env->SetByteArrayRegion(jbyteArray1, 0, class_data_len, (signed char*)class_data);
    jni_env->CallStaticVoidMethod(nativeInstrumentation, transform, loader, jni_env->NewStringUTF(name), class_being_redefined, protection_domain, jbyteArray1);
    auto out = (jbyteArray)jni_env->GetStaticObjectField(nativeInstrumentation, transformOutput);
    if (out == nullptr) {
        return;
    }
    *new_class_data_len = jni_env->GetArrayLength(out);
    *new_class_data = (unsigned char*) jni_env->GetByteArrayElements(out, nullptr);
}
jvmtiEventCallbacks callbacks;

JNIEXPORT void JNICALL Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit
        (JNIEnv *env, jclass) {
    if (initialized) {
        return;
    }
    #ifdef DO_MEM_CHECK
    mtrace();
    #endif
    MTRACE_START
    #ifdef NDEBUG
        printf("[NativeInstrumentation] libInit: A debug version of NativeInstrumnetation is currently being used, do not use it in production!\n");
    #endif
    #ifdef DO_MEM_CHECK
        printf("[NativeInstrumentation] libInit: The version of NativeInstrumentation has memory leak checks enabled, do not use it in production!\n");
    #endif
    jvmtiEnv *jvmtienv = GetJvmTiEnv(env);
    int err;
    callbacks.ClassFileLoadHook = classFileLoad;
    jvmtienv->SetEventCallbacks(&callbacks, sizeof(callbacks));
    jvmtienv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, nullptr);

    nativeInstrumentation = env->FindClass("me/fan87/nativeinstrumentation/NativeInstrumentation");
    if (nativeInstrumentation == nullptr) {
        error(env, "NativeInstrumentation is nullptr");
        MTRACE_END
        return;
    }
    transformOutput = env->GetStaticFieldID(nativeInstrumentation, "tmpTransformOutput", "[B");
    transform = env->GetStaticMethodID(nativeInstrumentation, "transform", "(Ljava/lang/ClassLoader;Ljava/lang/String;Ljava/lang/Class;Ljava/security/ProtectionDomain;[B)V");
    if (transform == nullptr) {
        error(env, "Transform method is nullptr");
        MTRACE_END
        return;
    }
    err = type_convert_init(env);
    if (err != 0) {
        error(env, "type convert init failure: %d", err);
        MTRACE_END
        return;
    }
    initialized = true;
    MTRACE_END
}

jobjectArray Java_me_fan87_nativeinstrumentation_NativeInstrumentation_getAllLoadedClasses0(JNIEnv *env, jclass clazz) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(env, clazz);
    MTRACE_START
    jvmtiEnv *jvmtienv = GetJvmTiEnv(env);
    jint size;
    jclass *classes;
    jvmtienv->GetLoadedClasses(&size, &classes);
    jobjectArray array = env->NewObjectArray(size, (jclass) env->NewGlobalRef(env->FindClass("java/lang/Object")), nullptr);
    for (int i = 0; i < size; ++i) {
        env->SetObjectArrayElement(array, i, classes[i]);
    }

    MTRACE_END
    return array;
}

jobjectArray Java_me_fan87_nativeinstrumentation_NativeInstrumentation_getInitiatedClasses0(JNIEnv *env, jclass clazz, jobject classLoader) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(env, clazz);
    MTRACE_START
    jvmtiEnv *jvmtienv = GetJvmTiEnv(env);
    jint size;
    jclass *classes;
    jvmtienv->GetClassLoaderClasses(classLoader, &size, &classes);
    jobjectArray array = env->NewObjectArray(size, env->FindClass("java/lang/Class"), nullptr);
    for (int i = 0; i < size; ++i) {
        env->SetObjectArrayElement(array, size, classes[i]);
    }
    MTRACE_END
    return array;
}

jlong Java_me_fan87_nativeinstrumentation_NativeInstrumentation_getObjectSize0(JNIEnv *env, jclass clazz, jobject obj) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(env, clazz);
    MTRACE_START
    jvmtiEnv *jvmtienv = GetJvmTiEnv(env);
    jlong objectSize = -1;
    jvmtienv->GetObjectSize(obj, &objectSize);
    MTRACE_END
    return objectSize;
}

void Java_me_fan87_nativeinstrumentation_NativeInstrumentation_appendToClassLoaderSearch0(JNIEnv *jnienv, jclass clazz, jstring jarFile, jboolean isBootLoader) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(jnienv, clazz);
    if (isBootLoader) {
        createAndThrowThrowableFromJVMTIErrorCode(jnienv, GetJvmTiEnv(jnienv)->AddToBootstrapClassLoaderSearch(jnienv->GetStringUTFChars(jarFile, nullptr)));
    } else {
        createAndThrowThrowableFromJVMTIErrorCode(jnienv, GetJvmTiEnv(jnienv)->AddToSystemClassLoaderSearch(jnienv->GetStringUTFChars(jarFile, nullptr)));
    }
}

void
Java_me_fan87_nativeinstrumentation_NativeInstrumentation_setNativeMethodPrefixes(JNIEnv *jnienv, jclass clazz, jobjectArray prefixArray, jboolean isRetransformable) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(jnienv, clazz);
    jvmtiEnv*   jvmtienv                        = GetJvmTiEnv(jnienv);
    jvmtiError  err                             = JVMTI_ERROR_NONE;
    jsize       arraySize;
    jboolean    errorOccurred                   = JNI_FALSE;

    arraySize = (*jnienv).GetArrayLength(prefixArray);
    errorOccurred = checkForThrowable(jnienv);

    if (!errorOccurred) {
        /* allocate the native to hold the native prefixes */
        const char** prefixes = (const char**) allocate(jvmtienv,
                                                        arraySize * sizeof(char*));
        /* since JNI ReleaseStringUTFChars needs the jstring from which the native
         * string was allocated, we store them in a parallel array */
        auto* originForRelease = (jstring*) allocate(jvmtienv,
                                                        arraySize * sizeof(jstring));
        errorOccurred = (prefixes == nullptr || originForRelease == nullptr);
        jplis_assert(!errorOccurred);
        if ( errorOccurred ) {
            createAndThrowThrowableFromJVMTIErrorCode(jnienv, JVMTI_ERROR_OUT_OF_MEMORY);
        }
        else {
            jint inx = 0;
            jint i;
            for (i = 0; i < arraySize; i++) {
                jstring      prefixStr  = nullptr;
                const char*  prefix;
                jsize        prefixLen;
                jboolean     isCopy;

                prefixStr = (jstring) (jnienv->GetObjectArrayElement(prefixArray, i));
                errorOccurred = checkForThrowable(jnienv);
                jplis_assert(!errorOccurred);
                if (errorOccurred) {
                    break;
                }
                if (prefixStr == nullptr) {
                    continue;
                }

                prefixLen = jnienv->GetStringUTFLength(prefixStr);
                errorOccurred = checkForThrowable(jnienv);
                jplis_assert(!errorOccurred);
                if (errorOccurred) {
                    break;
                }

                if (prefixLen > 0) {
                    prefix = jnienv->GetStringUTFChars(prefixStr, &isCopy);
                    errorOccurred = checkForThrowable(jnienv);
                    jplis_assert(!errorOccurred);
                    if (!errorOccurred && prefix != nullptr) {
                        prefixes[inx] = prefix;
                        originForRelease[inx] = prefixStr;
                        ++inx;
                    }
                }
            }

            err = (*jvmtienv).SetNativeMethodPrefixes(inx, (char**)prefixes);
            /* can be called from any phase */
            jplis_assert(err == JVMTI_ERROR_NONE);

            for (i = 0; i < inx; i++) {
                (*jnienv).ReleaseStringUTFChars(originForRelease[i], prefixes[i]);
            }
        }
        deallocate(jvmtienv, (void*)prefixes);
        deallocate(jvmtienv, (void*)originForRelease);
        
        
    }
}

jfieldID findField(JNIEnv *env, jclass clazz, jstring fieldName, char** signature) {
    const char *fName = env->GetStringUTFChars(fieldName, nullptr);
    jint count;
    jfieldID *fields;

    createAndThrowThrowableFromJVMTIErrorCode(env, jvmti1->GetClassFields(clazz, &count, &fields));

    for (int i = 0; i < count; i++) {
        jfieldID id = fields[i];
        char *name;
        jvmti1->GetFieldName(clazz, id, &name, signature, nullptr);

        if (strcmp(name, fName) == 0) {
            env->ReleaseStringUTFChars(fieldName, fName);
            return id;
        }
    }

    env->ReleaseStringUTFChars(fieldName, fName);
    MTRACE_END
    return nullptr;
}

JNIEXPORT jobject JNICALL Java_me_fan87_nativeinstrumentation_NativeInstrumentation_getField
        (JNIEnv * env, jclass thisClass, jclass clazz, jobject classInstance, jstring fieldName) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(env, thisClass);
    MTRACE_START
    char *signature;
    jfieldID id = findField(env, clazz, fieldName, &signature);
    if (id == nullptr) {
        error_custom(env, EXCEPTION_NO_SUCH_FIELD, "failed to find field %s", fieldName);
        MTRACE_END
        return nullptr;
    }
    MTRACE_END
    return getField(env, id, classInstance, signature);
}

JNIEXPORT jobject JNICALL Java_me_fan87_nativeinstrumentation_NativeInstrumentation_getStaticField
        (JNIEnv * env, jclass thisClass, jclass clazz, jstring fieldName) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(env, thisClass);
    MTRACE_START
    char *signature;
    jfieldID id = findField(env, clazz, fieldName, &signature);
    if (id == nullptr) {
        error_custom(env, EXCEPTION_NO_SUCH_FIELD, "failed to find field %s", fieldName);
        MTRACE_END
        return nullptr;
    }
    MTRACE_END
    return getStaticField(env, id, clazz, signature);
}

JNIEXPORT void JNICALL Java_me_fan87_nativeinstrumentation_NativeInstrumentation_setField
        (JNIEnv * env, jclass thisClass, jclass clazz, jobject classInstance, jstring fieldName, jobject value) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(env, thisClass);
    MTRACE_START
    char *signature;
    jfieldID id = findField(env, clazz, fieldName, &signature);
    if (id == nullptr) {
        error_custom(env, EXCEPTION_NO_SUCH_FIELD, "failed to find field %s", fieldName);
        MTRACE_END
        return;
    }
    setField(env, id, classInstance, value, signature);
    MTRACE_END
}

JNIEXPORT void JNICALL Java_me_fan87_nativeinstrumentation_NativeInstrumentation_setStaticField
        (JNIEnv * env, jclass thisClass, jclass clazz, jstring fieldName, jobject value) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(env, thisClass);
    MTRACE_START
    char *signature;
    jfieldID id = findField(env, clazz, fieldName, &signature);
    if (id == nullptr) {
        error_custom(env, EXCEPTION_NO_SUCH_FIELD, "failed to find field %s", fieldName);
        MTRACE_END
        return;
    }
    setStaticField(env, id, clazz, value, signature);
    MTRACE_END
}

jobject
Java_me_fan87_nativeinstrumentation_NativeInstrumentation_invokeMethodS(JNIEnv *env, jclass thisClass, jclass clazz, jobject classInstance, jstring methodName, jstring signature, jobjectArray args) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(env, thisClass);
    MTRACE_START
    const char* mName = env->GetStringUTFChars(methodName, nullptr);
    const char* mSig = env->GetStringUTFChars(signature, nullptr);
    jmethodID id = env->GetMethodID(clazz, mName, mSig);
    if (id == nullptr) {
        error_custom(env, EXCEPTION_NO_SUCH_METHOD, "failed to find method %s with signature %s", mName, mSig);
        env->ReleaseStringUTFChars(methodName, mName);
        env->ReleaseStringUTFChars(signature, mSig);
        MTRACE_END
        return nullptr;
    }

    jsize length = env->GetArrayLength(args);
    auto *objects = static_cast<jobject *>(malloc(length * sizeof(jobject)));
    for (int i = 0; i < length; i++) {
        objects[i] = env->GetObjectArrayElement(args, i);
    }

    jobject returnValue = callMethod(env, id, classInstance, objects, length, (char*) mSig);
    free(objects);
    env->ReleaseStringUTFChars(methodName, mName);
    env->ReleaseStringUTFChars(signature, mSig);
    MTRACE_END
    return returnValue;
}

jobject
Java_me_fan87_nativeinstrumentation_NativeInstrumentation_invokeStaticMethodS(JNIEnv *env, jclass thisClass, jclass clazz, jstring methodName, jstring signature, jobjectArray args) {
    Java_me_fan87_nativeinstrumentation_NativeInstrumentation_libInit(env, thisClass);
    MTRACE_START
    const char* mName = env->GetStringUTFChars(methodName, nullptr);
    const char* mSig = env->GetStringUTFChars(signature, nullptr);
    jmethodID id = env->GetStaticMethodID(clazz, mName, mSig);
    if (id == nullptr) {
        error_custom(env, EXCEPTION_NO_SUCH_METHOD, "failed to find method %s with signature %s", mName, mSig);
        env->ReleaseStringUTFChars(methodName, mName);
        env->ReleaseStringUTFChars(signature, mSig);
        MTRACE_END
        return nullptr;
    }

    jsize length = env->GetArrayLength(args);
    auto *objects = static_cast<jobject *>(malloc(length * sizeof(jobject)));
    for (int i = 0; i < length; i++) {
        objects[i] = env->GetObjectArrayElement(args, i);
    }

    jobject returnValue = callStaticMethod(env, id, clazz, objects, length, (char*) mSig);
    free(objects);
    env->ReleaseStringUTFChars(methodName, mName);
    env->ReleaseStringUTFChars(signature, mSig);
    MTRACE_END
    return returnValue;
}
