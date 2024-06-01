#include "type_convert.h"
#include "jni.h"
#include <cstring>
#include <cstdlib>

#define _define_types_req_dat(type, _a, _b, _c) \
    jclass type ## _class; \
    jmethodID type ## _constructor_id; \
    jmethodID type ## _convert_id;


#define boolean
#define byte
#define character
#define short
#define integer
#define long
#define float
#define double

#define _each_type(op) \
    op(boolean, "Boolean", 'Z', "Z") \
    op(byte, "Byte", 'B', "B") \
    op(character, "Character", 'C', "C") \
    op(short, "Short", 'S', "S") \
    op(integer, "Integer", 'I', "I") \
    op(long, "Long", 'J', "J") \
    op(float, "Float", 'F', "F") \
    op(double, "Double", 'D', "D")
#define _each_type_det(op) \
    op(boolean, Boolean, Z, 'Z', z, Boolean, boolean) \
    op(byte, Byte, B, 'B', b, Byte, byte) \
    op(character, Character, C, 'C', c, Char, char) \
    op(short, Short, S, 'S', s, Short, short) \
    op(integer, Integer, I, 'I', i, Int, int) \
    op(long, Long, J, 'J', j, Long, long) \
    op(float, Float, F, 'F', f, Float, float) \
    op(double, Double, D, 'D', d, Double, double)   \

_each_type(_define_types_req_dat)


int parseSignature(const char *signature, char *output, size_t *len) {
    bool readingObject = false;
    int size = 0;

    char c;
    int i = 1;
    while ((c = signature[i++])) {
        if (c == ')') {
            break;
        }

        if (readingObject) {
            if (c == ';') {
                readingObject = false;
            }
            continue;
        }

        if (c == 'L') {
            readingObject = true;
        }

        if (c == '[') {
            if (signature[i] == 'L') {
                readingObject = true;
            } else {
                i++;
            }
        }

        output[size++] = c;
    }
    output[size++] = '\0';
    *len = size;

    return 0;
}
#define _objectsToJvalues_inn(norm, name, sign, sign_char, lower, getter, ...) \
        if (targetType == sign_char && env->IsSameObject(objClass, norm ## _class)) { \
            values[i].lower = env->Call ## getter ## Method(obj, norm ## _convert_id); \
            continue; \
        }
int objectsToJvalues(JNIEnv* env, const char* signature, const jobject* args, jsize length, jvalue *values) {
    char *targetParameterTypes = static_cast<char *>(malloc((strlen(signature) + 1) * sizeof(char)));
    size_t len = 0;
    parseSignature(signature, targetParameterTypes, &len);

    for (int i = 0; i < length; i++) {
        jobject obj = args[i];
        jclass objClass = env->GetObjectClass(obj);
        char targetType = targetParameterTypes[i];
        _each_type_det(_objectsToJvalues_inn)

        values[i].l = obj;
    }
    free(targetParameterTypes);
    return 0;
}

char getReturnType(char* signature) {
    char* index = strchr(signature, ')');
    char type = index == nullptr ? *signature : *(index + 1);
    return type == '[' ? 'L' : type;
}


#define _init_type(name, class_name, signature, sign_char, _0, _1, lower_case_short) \
    name ## _class = (jclass) env->NewGlobalRef(env->FindClass("java/lang/" #class_name)); \
    name ## _constructor_id = env->GetMethodID(name ## _class, "<init>", "(" #signature ")V"); \
    name ## _convert_id = env->GetMethodID(name ## _class, #lower_case_short "Value", "()" #signature);


int type_convert_init(JNIEnv *env) {
    
    // primitive types
    _each_type_det(_init_type)
    return 0;
}

#define _getField_inn(name, class_name, upper, sign_char, lower, getter, ...) \
    case sign_char: \
        constructorArgs->lower = env->Get ## getter ## Field(classInstance, id); \
        return env->NewObjectA(name ## _class, name ## _constructor_id, constructorArgs); \

jobject getField(JNIEnv *env, jfieldID id, jobject classInstance, char *signature) {
    char returnType = getReturnType(signature);
    jvalue constructorArgs[1];
    switch (returnType) {
        case 'L':
            return env->GetObjectField(classInstance, id);
        _each_type_det(_getField_inn)
        default:
            break;
    }

    return nullptr;
}

#define _getStaticField_inn(name, class_name, upper, sign_char, lower, getter, ...) \
    case sign_char: \
        constructorArgs->lower = env->GetStatic ## getter ## Field(clazz, id); \
        return env->NewObjectA(name ## _class, name ## _constructor_id, constructorArgs); \

jobject getStaticField(JNIEnv *env, jfieldID id, jclass clazz, char *signature) {
    char returnType = getReturnType(signature);

    jvalue constructorArgs[1];
    switch (returnType) {
        case 'L':
            return env->GetStaticObjectField(clazz, id);
        _each_type_det(_getStaticField_inn)
        default:
            break;
    }

    return nullptr;
}

#define _setField_inn(name, class_name, upper, sign_char, lower, getter, ...) \
    case sign_char: \
        env->Set ## getter ## Field(classInstance, id, env->Call ## getter ## Method(value, name ## _convert_id));\
    break;
void setField(JNIEnv *env, jfieldID id, jobject classInstance, jobject value, char* signature) {
    char returnType = getReturnType(signature);
    switch (returnType) {
        case 'L':
            env->SetObjectField(classInstance, id, value);
            break;
        _each_type_det(_setField_inn)
        default:
            break;
    }
}

#define _setStaticField_inn(name, class_name, upper, sign_char, lower, getter, ...) \
    case sign_char: \
        env->SetStatic ## getter ## Field(clazz, id, env->Call ## getter ## Method(value, name ## _convert_id)); \
    break;

void setStaticField(JNIEnv *env, jfieldID id, jclass clazz, jobject value, char* signature) {
    char returnType = getReturnType(signature);
    switch (returnType) {
        case 'L':
            env->SetStaticObjectField(clazz, id, value);
            break;
        _each_type_det(_setStaticField_inn)
        default:
            break;
    }
}

#define _callMethod_inn(name, class_name, upper, sign_char, lower, getter, ...) \
    case sign_char: \
        constructorArgs->lower = env->Call ## getter ## MethodA(classInstance, id, values); \
        returnValue = env->NewObjectA(name ## _class, name ## _constructor_id, constructorArgs); \
        break;

jobject callMethod(JNIEnv *env, jmethodID id, jobject classInstance, jobject* objects, jsize length, char* signature) {
    char returnType = getReturnType(signature);
    auto *values = static_cast<jvalue *>(malloc(length * sizeof(jvalue)));
    objectsToJvalues(env, signature, objects, length, values);
    jobject returnValue = nullptr;
    jvalue constructorArgs[1];
    switch (returnType) {
        case 'V':
            env->CallVoidMethodA(classInstance, id, values);
            break;
        case 'L':
            returnValue = env->CallObjectMethodA(classInstance, id, values);
            break;
        _each_type_det(_callMethod_inn)
        default:
            break;
    }

    free(values);
    return returnValue;
}
#define _callStaticMethod_inn(name, class_name, upper, sign_char, lower, getter, ...) \
    case sign_char: \
        constructorArgs->lower = env->CallStatic ## getter ## MethodA(clazz, id, values); \
        returnValue = env->NewObjectA(name ## _class, name ## _constructor_id, constructorArgs); \
        break;

jobject callStaticMethod(JNIEnv *env, jmethodID id, jclass clazz, jobject* objects, jsize length, char* signature) {
    char returnType = getReturnType(signature);

    auto *values = static_cast<jvalue *>(malloc(length * sizeof(jvalue)));
    objectsToJvalues(env, signature, objects, length, values);

    jvalue constructorArgs[1];
    jobject returnValue = nullptr;
    switch (returnType) {
        case 'V':
            env->CallStaticVoidMethodA(clazz, id, values);
            break;

        case 'L':
            returnValue = env->CallStaticObjectMethodA(clazz, id, values);
            break;
            _each_type_det(_callStaticMethod_inn);
        default:
            break;
    }
    free(values);

    return returnValue;
}