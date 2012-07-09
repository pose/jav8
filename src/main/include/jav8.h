/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class lu_flier_script_ManagedV8Object */

#ifndef _Included_lu_flier_script_ManagedV8Object
#define _Included_lu_flier_script_ManagedV8Object
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     lu_flier_script_ManagedV8Object
 * Method:    internalRelease
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_ManagedV8Object_internalRelease
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
/* Header for class lu_flier_script_V8ScriptEngineFactory */

#ifndef _Included_lu_flier_script_V8ScriptEngineFactory
#define _Included_lu_flier_script_V8ScriptEngineFactory
#ifdef __cplusplus
extern "C" {
#endif
/* Inaccessible static: names */
/* Inaccessible static: mimeTypes */
/* Inaccessible static: extensions */
/*
 * Class:     lu_flier_script_V8ScriptEngineFactory
 * Method:    getParameter
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8ScriptEngineFactory_getParameter
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
/* Header for class lu_flier_script_V8ScriptEngine */

#ifndef _Included_lu_flier_script_V8ScriptEngine
#define _Included_lu_flier_script_V8ScriptEngine
#ifdef __cplusplus
extern "C" {
#endif
/* Inaccessible static: _00024assertionsDisabled */
/*
 * Class:     lu_flier_script_V8ScriptEngine
 * Method:    gc
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8ScriptEngine_gc
  (JNIEnv *, jclass);

/*
 * Class:     lu_flier_script_V8ScriptEngine
 * Method:    lowMemory
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8ScriptEngine_lowMemory
  (JNIEnv *, jclass);

/*
 * Class:     lu_flier_script_V8ScriptEngine
 * Method:    idle
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_lu_flier_script_V8ScriptEngine_idle
  (JNIEnv *, jclass);

#ifdef __cplusplus
}
#endif
#endif
/* Header for class lu_flier_script_V8CompiledScript */

#ifndef _Included_lu_flier_script_V8CompiledScript
#define _Included_lu_flier_script_V8CompiledScript
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     lu_flier_script_V8CompiledScript
 * Method:    internalCompile
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_lu_flier_script_V8CompiledScript_internalCompile
  (JNIEnv *, jobject, jstring);

/*
 * Class:     lu_flier_script_V8CompiledScript
 * Method:    internalRelease
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8CompiledScript_internalRelease
  (JNIEnv *, jobject, jlong);

/*
 * Class:     lu_flier_script_V8CompiledScript
 * Method:    internalExecute
 * Signature: (JLjavax/script/ScriptContext;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8CompiledScript_internalExecute
  (JNIEnv *, jobject, jlong, jobject);

#ifdef __cplusplus
}
#endif
#endif
/* Header for class lu_flier_script_V8Context */

#ifndef _Included_lu_flier_script_V8Context
#define _Included_lu_flier_script_V8Context
#ifdef __cplusplus
extern "C" {
#endif
/* Inaccessible static: scopes */
/*
 * Class:     lu_flier_script_V8Context
 * Method:    getEntered
 * Signature: ()Llu/flier/script/V8Context;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_getEntered
  (JNIEnv *, jclass);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    getCurrent
 * Signature: ()Llu/flier/script/V8Context;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_getCurrent
  (JNIEnv *, jclass);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    getCalling
 * Signature: ()Llu/flier/script/V8Context;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_getCalling
  (JNIEnv *, jclass);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    inContext
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_lu_flier_script_V8Context_inContext
  (JNIEnv *, jclass);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreateObject
 * Signature: ()Llu/flier/script/V8Object;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateObject
  (JNIEnv *, jobject);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreateArray
 * Signature: ([Ljava/lang/Object;I)Llu/flier/script/V8Array;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateArray
  (JNIEnv *, jobject, jobjectArray, jint);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreateIntArray
 * Signature: ([II)Llu/flier/script/V8Array;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateIntArray
  (JNIEnv *, jobject, jintArray, jint);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreateStringArray
 * Signature: ([Ljava/lang/String;I)Llu/flier/script/V8Array;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateStringArray
  (JNIEnv *, jobject, jobjectArray, jint);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreateLongArray
 * Signature: ([JI)Llu/flier/script/V8Array;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateLongArray
  (JNIEnv *, jobject, jlongArray, jint);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreateDoubleArray
 * Signature: ([DI)Llu/flier/script/V8Array;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateDoubleArray
  (JNIEnv *, jobject, jdoubleArray, jint);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreateFloatArray
 * Signature: ([FI)Llu/flier/script/V8Array;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateFloatArray
  (JNIEnv *, jobject, jfloatArray, jint);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreateShortArray
 * Signature: ([SI)Llu/flier/script/V8Array;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateShortArray
  (JNIEnv *, jobject, jshortArray, jint);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreateBoolArray
 * Signature: ([ZI)Llu/flier/script/V8Array;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateBoolArray
  (JNIEnv *, jobject, jbooleanArray, jint);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreateDateArray
 * Signature: ([Ljava/util/Date;I)Llu/flier/script/V8Array;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateDateArray
  (JNIEnv *, jobject, jobjectArray, jint);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreateV8ArrayArray
 * Signature: ([Llu/flier/script/V8Array;I)Llu/flier/script/V8Array;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateV8ArrayArray
  (JNIEnv *, jobject, jobjectArray, jint);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreateV8ObjectArray
 * Signature: ([Llu/flier/script/V8Object;I)Llu/flier/script/V8Array;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateV8ObjectArray
  (JNIEnv *, jobject, jobjectArray, jint);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreateV8Function
 * Signature: (Ljava/lang/Object;Ljava/lang/Class;Ljava/lang/String;Ljava/lang/String;ZZ)Llu/flier/script/V8Function;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateV8Function
  (JNIEnv *, jobject, jobject, jclass, jstring, jstring, jboolean, jboolean);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalCreate
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_lu_flier_script_V8Context_internalCreate
  (JNIEnv *, jobject);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalRelease
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Context_internalRelease
  (JNIEnv *, jobject, jlong);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalEnter
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Context_internalEnter
  (JNIEnv *, jobject, jlong);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalLeave
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Context_internalLeave
  (JNIEnv *, jobject, jlong);

/*
 * Class:     lu_flier_script_V8Context
 * Method:    internalGetGlobal
 * Signature: (J)Llu/flier/script/V8Object;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalGetGlobal
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
/* Header for class lu_flier_script_V8Object */

#ifndef _Included_lu_flier_script_V8Object
#define _Included_lu_flier_script_V8Object
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     lu_flier_script_V8Object
 * Method:    internalGetKeys
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_lu_flier_script_V8Object_internalGetKeys
  (JNIEnv *, jobject);

/*
 * Class:     lu_flier_script_V8Object
 * Method:    size
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_lu_flier_script_V8Object_size
  (JNIEnv *, jobject);

/*
 * Class:     lu_flier_script_V8Object
 * Method:    isEmpty
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_lu_flier_script_V8Object_isEmpty
  (JNIEnv *, jobject);

/*
 * Class:     lu_flier_script_V8Object
 * Method:    clear
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Object_clear
  (JNIEnv *, jobject);

/*
 * Class:     lu_flier_script_V8Object
 * Method:    containsKey
 * Signature: (Ljava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL Java_lu_flier_script_V8Object_containsKey
  (JNIEnv *, jobject, jobject);

/*
 * Class:     lu_flier_script_V8Object
 * Method:    internalGet
 * Signature: (Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Object_internalGet
  (JNIEnv *, jobject, jobject);

/*
 * Class:     lu_flier_script_V8Object
 * Method:    internalPut
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Object_internalPut
  (JNIEnv *, jobject, jstring, jobject);

/*
 * Class:     lu_flier_script_V8Object
 * Method:    internalRemove
 * Signature: (Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Object_internalRemove
  (JNIEnv *, jobject, jobject);

#ifdef __cplusplus
}
#endif
#endif
/* Header for class lu_flier_script_V8Array */

#ifndef _Included_lu_flier_script_V8Array
#define _Included_lu_flier_script_V8Array
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalGet
 * Signature: (JI)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Array_internalGet
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalSet
 * Signature: (JILjava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Array_internalSet
  (JNIEnv *, jobject, jlong, jint, jobject);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalSetElements
 * Signature: (J[Ljava/lang/Object;I)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetElements
  (JNIEnv *, jobject, jlong, jobjectArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalSetStringElements
 * Signature: (J[Ljava/lang/Object;I)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetStringElements
  (JNIEnv *, jobject, jlong, jobjectArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalSetIntElements
 * Signature: (J[II)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetIntElements
  (JNIEnv *, jobject, jlong, jintArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalSetLongElements
 * Signature: (J[JI)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetLongElements
  (JNIEnv *, jobject, jlong, jlongArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalSetShortElements
 * Signature: (J[SI)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetShortElements
  (JNIEnv *, jobject, jlong, jshortArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalSetDoubleElements
 * Signature: (J[DI)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetDoubleElements
  (JNIEnv *, jobject, jlong, jdoubleArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalSetFloatElements
 * Signature: (J[FI)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetFloatElements
  (JNIEnv *, jobject, jlong, jfloatArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalSetBooleanElements
 * Signature: (J[ZI)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetBooleanElements
  (JNIEnv *, jobject, jlong, jbooleanArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalSetDateElements
 * Signature: (J[Ljava/util/Date;I)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetDateElements
  (JNIEnv *, jobject, jlong, jobjectArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalSetV8ArrayElements
 * Signature: (J[Llu/flier/script/V8Array;I)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetV8ArrayElements
  (JNIEnv *, jobject, jlong, jobjectArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalSetV8ObjectElements
 * Signature: (J[Llu/flier/script/V8Object;I)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetV8ObjectElements
  (JNIEnv *, jobject, jlong, jobjectArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalGetSize
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_lu_flier_script_V8Array_internalGetSize
  (JNIEnv *, jobject, jlong);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalToObjectArray
 * Signature: (JI)[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL Java_lu_flier_script_V8Array_internalToObjectArray
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalToIntArray
 * Signature: (J[II)[I
 */
JNIEXPORT jintArray JNICALL Java_lu_flier_script_V8Array_internalToIntArray
  (JNIEnv *, jobject, jlong, jintArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalToLongArray
 * Signature: (J[JI)[J
 */
JNIEXPORT jlongArray JNICALL Java_lu_flier_script_V8Array_internalToLongArray
  (JNIEnv *, jobject, jlong, jlongArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalToShortArray
 * Signature: (J[SI)[S
 */
JNIEXPORT jshortArray JNICALL Java_lu_flier_script_V8Array_internalToShortArray
  (JNIEnv *, jobject, jlong, jshortArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalToDoubleArray
 * Signature: (J[DI)[D
 */
JNIEXPORT jdoubleArray JNICALL Java_lu_flier_script_V8Array_internalToDoubleArray
  (JNIEnv *, jobject, jlong, jdoubleArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalToFloatArray
 * Signature: (J[FI)[F
 */
JNIEXPORT jfloatArray JNICALL Java_lu_flier_script_V8Array_internalToFloatArray
  (JNIEnv *, jobject, jlong, jfloatArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalToBooleanArray
 * Signature: (J[ZI)[Z
 */
JNIEXPORT jbooleanArray JNICALL Java_lu_flier_script_V8Array_internalToBooleanArray
  (JNIEnv *, jobject, jlong, jbooleanArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalToStringArray
 * Signature: (J[Ljava/lang/String;I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_lu_flier_script_V8Array_internalToStringArray
  (JNIEnv *, jobject, jlong, jobjectArray, jint);

/*
 * Class:     lu_flier_script_V8Array
 * Method:    internalToDateArray
 * Signature: (J[Ljava/util/Date;I)[Ljava/util/Date;
 */
JNIEXPORT jobjectArray JNICALL Java_lu_flier_script_V8Array_internalToDateArray
  (JNIEnv *, jobject, jlong, jobjectArray, jint);

#ifdef __cplusplus
}
#endif
#endif
/* Header for class lu_flier_script_V8Function */

#ifndef _Included_lu_flier_script_V8Function
#define _Included_lu_flier_script_V8Function
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     lu_flier_script_V8Function
 * Method:    internalInvoke
 * Signature: (JJ[Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Function_internalInvoke
  (JNIEnv *, jobject, jlong, jlong, jobjectArray);

/*
 * Class:     lu_flier_script_V8Function
 * Method:    internalInvokeVoid
 * Signature: (J[Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_lu_flier_script_V8Function_internalInvokeVoid
  (JNIEnv *, jobject, jlong, jobjectArray);

#ifdef __cplusplus
}
#endif
#endif