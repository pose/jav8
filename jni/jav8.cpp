#include "jav8.h"

#include <cstring>
#include <iostream>
#include <memory>

#include <v8.h>

#include "Config.h"

#ifdef USE_INTERNAL_V8_API
  #undef COMPILER
  #include <src/v8.h>
#endif

#include "Utils.h"

jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
  v8::V8::Initialize(); 

  return JNI_VERSION_1_2;
}

void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{

}

void JNICALL Java_lu_flier_script_ManagedV8Object_internalRelease
  (JNIEnv *, jobject, jlong ptr)
{
  if (ptr) 
  { 
    if (jni::V8Isolate::IsAlive()) {
      v8::Persistent<v8::Object>((v8::Object *) ptr).Dispose(); 
    }
  }
}

void JNICALL Java_lu_flier_script_V8ScriptEngine_gc
  (JNIEnv *, jclass)
{
  jni::V8Isolate::ensureInIsolate();

  #ifdef USE_INTERNAL_V8_API
    HEAP->CollectAllAvailableGarbage();
  #else
    // TODO throw a exception
  #endif
}

void JNICALL Java_lu_flier_script_V8ScriptEngine_lowMemory
  (JNIEnv *, jclass)
{
  jni::V8Isolate::ensureInIsolate();

  v8::V8::LowMemoryNotification();
}

jboolean JNICALL Java_lu_flier_script_V8ScriptEngine_idle
  (JNIEnv *, jclass)
{
  jni::V8Isolate::ensureInIsolate();

  return v8::V8::IdleNotification() ? JNI_TRUE : JNI_FALSE;
}

jobject JNICALL Java_lu_flier_script_V8ScriptEngineFactory_getParameter(JNIEnv *pEnv, jobject pObj, jstring key)
{
  jni::Env env(pEnv);

  const std::string name = env.GetString(key);

  if (name == "javax.script.name")  
    return pEnv->NewStringUTF("Jav8");
  
  if (name == "javax.script.engine")
    return pEnv->NewStringUTF("Google V8");
  if (name == "javax.script.engine_version")
    return pEnv->NewStringUTF(v8::V8::GetVersion());

  if (name == "javax.script.language")
    return pEnv->NewStringUTF("ECMAScript");
  if (name == "javax.script.language_version")
    return pEnv->NewStringUTF("1.8.5");
  if (name == "jav8.native_array")  
  #if USE_NATIVE_ARRAY
    return env.NewBoolean(true);
  #else
    return env.NewBoolean(false);
  #endif

  env.Throw("java/lang/IllegalArgumentException", "Invalid key");

  return NULL;
}

jlong JNICALL Java_lu_flier_script_V8CompiledScript_internalCompile
  (JNIEnv *pEnv, jobject pObj, jstring pScript)
{
  jni::V8Env env(pEnv);

  if (!v8::Context::InContext())
  {
    env.Throw("java/lang/IllegalStateException", "Not in context");
  }

  const std::string script = env.GetString(pScript);

  v8::Handle<v8::Script> compiledScript = v8::Script::Compile(v8::String::New(script.c_str(), script.size()));

  return env.HasCaught() ? 0 : (jlong) *v8::Persistent<v8::Script>::New(compiledScript);
}

void JNICALL Java_lu_flier_script_V8CompiledScript_internalRelease
  (JNIEnv *, jobject, jlong ptr)
{  
  if (ptr) { 
    if (jni::V8Isolate::IsAlive()) {
      v8::Persistent<v8::Script>((v8::Script *) ptr).Dispose(); 
    }
  }
}

jobject JNICALL Java_lu_flier_script_V8CompiledScript_internalExecute
  (JNIEnv *pEnv, jobject pObj, jlong pCompiledScript, jobject pContext)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Script> compiledScript((v8::Script *) pCompiledScript);

  v8::Handle<v8::Value> result = compiledScript->Run();

  return env.HasCaught() ? NULL : env.Wrap(result);
}

jobject JNICALL Java_lu_flier_script_V8Context_getEntered(JNIEnv *pEnv, jclass)
{
  if (!v8::Context::InContext()) return NULL;

  jni::V8Env env(pEnv);
  
  return env.NewV8Context(v8::Context::GetEntered());
}

jobject JNICALL Java_lu_flier_script_V8Context_getCurrent(JNIEnv *pEnv, jclass)
{
  if (!v8::Context::InContext()) return NULL;

  jni::V8Env env(pEnv);

  return env.NewV8Context(v8::Context::GetCurrent());
}

jobject JNICALL Java_lu_flier_script_V8Context_getCalling(JNIEnv *pEnv, jclass)
{
  if (!v8::Context::InContext()) return NULL;

  jni::V8Env env(pEnv);

  return env.NewV8Context(v8::Context::GetCalling());
}

jboolean JNICALL Java_lu_flier_script_V8Context_inContext(JNIEnv *pEnv, jclass)
{
  jni::V8Env env(pEnv);

  return v8::Context::InContext() ? JNI_TRUE : JNI_FALSE;
}

jlong JNICALL Java_lu_flier_script_V8Context_internalCreate(JNIEnv *pEnv, jobject pObj)
{
  jni::V8Isolate::ensureInIsolate();
  v8::Handle<v8::Context> ctxt = v8::Context::New();

  jni::V8Env env(pEnv);

  v8::Context::Scope context_scope(ctxt);

  ctxt->Global()->Set(v8::String::NewSymbol("__proto__"), env.Wrap(pObj));

  return (jlong) *v8::Persistent<v8::Context>::New(ctxt);
}

void JNICALL Java_lu_flier_script_V8Context_internalRelease
  (JNIEnv *pEnv, jobject, jlong ptr)
{
  if (ptr) { 
    if (jni::V8Isolate::IsAlive()) {
      v8::Persistent<v8::Context> ctxt((v8::Context *) ptr);

      ctxt->Exit();
      ctxt.Dispose(); 
    }
  }
}

void JNICALL Java_lu_flier_script_V8Context_internalEnter
  (JNIEnv *pEnv, jobject, jlong ptr)
{
  if (ptr) { 
    jni::V8Env env(pEnv);

    v8::Persistent<v8::Context>((v8::Context *) ptr)->Enter(); 
  }
}

void JNICALL Java_lu_flier_script_V8Context_internalLeave
  (JNIEnv *pEnv, jobject, jlong ptr)
{
  if (ptr) { 
    jni::V8Env env(pEnv);

    v8::Persistent<v8::Context>((v8::Context *) ptr)->Exit(); 
  }
}

jobject JNICALL Java_lu_flier_script_V8Context_internalGetGlobal
  (JNIEnv *pEnv, jobject, jlong ptr)
{
  jni::V8Env env(pEnv);
  
  v8::Handle<v8::Object> global = v8::Persistent<v8::Context>((v8::Context *) ptr)->Global();

  return env.HasCaught() ? NULL : env.NewV8Object(global);
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateObject
  (JNIEnv *pEnv, jobject) 
{
  jni::V8Env env(pEnv);

  return env.NewV8Object(v8::Object::New());
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateArray
  (JNIEnv *pEnv, jobject, jobjectArray source, jint length) 
{
  jni::V8Env env(pEnv);
  v8::HandleScope handleScope;

  v8::Handle<v8::Array> array = v8::Array::New(length);

  for (size_t i = 0; i < length; i++) {
      jobject item = pEnv->GetObjectArrayElement(source, i);
      array->Set(i, env.Wrap(item));
  }

  handleScope.Close(array);

  return env.NewV8Array(array);
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateIntArray
  (JNIEnv *pEnv, jobject, jintArray source, jint length) 
{
  jni::V8Env env(pEnv);

  int size = pEnv->GetArrayLength(source);

  if (length > size) {
    length = size;
  }

  v8::Handle<v8::Array> array = v8::Array::New(length);
  v8::Handle<v8::Integer> zero = v8::Integer::New(0); 

  jint *data = (jint *)(pEnv->GetPrimitiveArrayCritical(source, 0));

  for (size_t i = 0; i < length; i++) {
    jint d = data[i];

    if (d == 0) {
      array->Set(i, zero);
    } else {
      array->Set(i, v8::Integer::New(d));
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(source, data, 0);

  return env.NewV8Array(array);
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateLongArray
  (JNIEnv *pEnv, jobject, jlongArray source, jint length) 
{
  jni::V8Env env(pEnv);

  int size = pEnv->GetArrayLength(source);

  if (length > size) {
    length = size;
  }

  v8::Handle<v8::Array> array = v8::Array::New(length);
  v8::Handle<v8::Number> zero = v8::Number::New(0.0);

  jlong *data = (jlong *)(pEnv->GetPrimitiveArrayCritical(source, 0));

  for (size_t i = 0; i < length; i++) {
    jlong d = data[i];

    if (d == 0) {
      array->Set(i, zero);
    } else {
      array->Set(i, v8::Number::New(d));
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(source, data, 0);

  return env.NewV8Array(array);
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateDoubleArray
  (JNIEnv *pEnv, jobject, jdoubleArray source, jint length) 
{
  jni::V8Env env(pEnv);

  int size = pEnv->GetArrayLength(source);

  if (length > size) {
    length = size;
  }

  v8::Handle<v8::Array> array = v8::Array::New(length);
  v8::Handle<v8::Number> zero = v8::Number::New(0.0);

  jdouble *data = (jdouble *)(pEnv->GetPrimitiveArrayCritical(source, 0));

  for (size_t i = 0; i < length; i++) {
    jdouble d = data[i];

    if (d == 0.0) {
      array->Set(i, zero);
    } else {
      array->Set(i, v8::Number::New(d));
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(source, data, 0);

  return env.NewV8Array(array);
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateFloatArray
  (JNIEnv *pEnv, jobject, jfloatArray source, jint length) 
{
  jni::V8Env env(pEnv);

  int size = pEnv->GetArrayLength(source);

  if (length > size) {
    length = size;
  }

  v8::Handle<v8::Array> array = v8::Array::New(length);
  v8::Handle<v8::Number> zero = v8::Number::New(0.0); 

  jfloat *data = (jfloat *)(pEnv->GetPrimitiveArrayCritical(source, 0));

  for (size_t i = 0; i < length; i++) {
    jfloat d = data[i];

    if (d == 0.0f) {
      array->Set(i, zero);
    } else {
      array->Set(i, v8::Number::New(d));
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(source, data, 0);

  return env.NewV8Array(array);
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateShortArray
  (JNIEnv *pEnv, jobject, jshortArray source, jint length) 
{
  jni::V8Env env(pEnv);

  int size = pEnv->GetArrayLength(source);

  if (length > size) {
    length = size;
  }

  v8::Handle<v8::Array> array = v8::Array::New(length);
  v8::Handle<v8::Integer> zero = v8::Integer::New(0); 

  jshort *data = (jshort *)(pEnv->GetPrimitiveArrayCritical(source, 0));

  for (size_t i = 0; i < length; i++) {
    jshort d = data[i];

    if (d == 0) {
      array->Set(i, zero);
    } else {
      array->Set(i, v8::Number::New(d));
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(source, data, 0);

  return env.NewV8Array(array);
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateBoolArray
  (JNIEnv *pEnv, jobject, jbooleanArray source, jint length) 
{
  jni::V8Env env(pEnv);

  int size = pEnv->GetArrayLength(source);

  if (length > size) {
    length = size;
  }

  v8::Handle<v8::Array> array = v8::Array::New(length);
  v8::Handle<v8::Value> trueValue = v8::True();
  v8::Handle<v8::Value> falseValue = v8::False();

  jboolean *data = (jboolean *)(pEnv->GetPrimitiveArrayCritical(source, 0));

  for (size_t i = 0; i < length; i++) {
    array->Set(i, data[i] == JNI_TRUE ? trueValue : falseValue);
  }

  pEnv->ReleasePrimitiveArrayCritical(source, data, 0);

  return env.NewV8Array(array);
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateStringArray
  (JNIEnv *pEnv, jobject, jobjectArray source, jint length) {
  jni::V8Env env(pEnv);

  v8::Handle<v8::Array> array = v8::Array::New(length);
  v8::Handle<v8::Value> nullValue = v8::Null();

  for (size_t i = 0; i < length; i++) {
    jstring str = (jstring)pEnv->GetObjectArrayElement(source, i);

    if (str == NULL) {
      array->Set(i, nullValue);
    } else {
      int length = pEnv->GetStringUTFLength(str);
      const jchar *p = pEnv->GetStringCritical(str, NULL);
      v8::Handle<v8::String> next = v8::String::New((const uint16_t *)p, length);
      pEnv->ReleaseStringCritical(str, p);
      array->Set(i, next);
    }
  }

  return env.NewV8Array(array);
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateDateArray
  (JNIEnv *pEnv, jobject, jobjectArray source, jint length) {
  jni::V8Env env(pEnv);

  v8::Handle<v8::Array> array = v8::Array::New(length);
  v8::Handle<v8::Value> nullValue = v8::Null();

  for (size_t i = 0; i < length; i++) {
    jobject jdate = pEnv->GetObjectArrayElement(source, i);

    if (jdate == NULL) {
      array->Set(i, nullValue);
    } else {
      array->Set(i, env.WrapDate(jdate));
    }
  }

  return env.NewV8Array(array);
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateV8ArrayArray
  (JNIEnv *pEnv, jobject, jobjectArray source, jint length) {
  jni::V8Env env(pEnv);

  v8::Handle<v8::Array> array = v8::Array::New(length);
  v8::Handle<v8::Value> nullValue = v8::Null();

  for (size_t i = 0; i < length; i++) {
    jobject jarr = pEnv->GetObjectArrayElement(source, i);

    if (jarr == NULL) {
      array->Set(i, nullValue);
    } else {
      array->Set(i, env.WrapV8Array(jarr));
    }
  }

  return env.NewV8Array(array);
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateV8ObjectArray
  (JNIEnv *pEnv, jobject, jobjectArray source, jint length) {
  jni::V8Env env(pEnv);

  v8::Handle<v8::Array> array = v8::Array::New(length);
  v8::Handle<v8::Value> nullValue = v8::Null();

  for (size_t i = 0; i < length; i++) {
    jobject jobj = pEnv->GetObjectArrayElement(source, i);

    if (jobj == NULL) {
      array->Set(i, nullValue);
    } else {
      array->Set(i, env.WrapV8Object(jobj));
    }
  }

  return env.NewV8Array(array);
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Array_internalSet
  (JNIEnv *pEnv, jobject, jlong pArray, jint index, jobject element) 
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Array> array((v8::Array *)pArray);

  array->Set(index, env.Wrap(element));
}

JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetElements
  (JNIEnv *pEnv, jobject, jlong pArray, jobjectArray elements, jint length) 
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Array> array((v8::Array *)pArray);

  for (size_t i = 0; i < length; i++) {
      jobject item = pEnv->GetObjectArrayElement(elements, i);
      array->Set(i, env.Wrap(item));
  }
}

JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetIntElements
  (JNIEnv *pEnv, jobject, jlong pArray, jintArray source, jint length) 
{
  v8::HandleScope handleScope;
  int size = pEnv->GetArrayLength(source);

  if (length > size) {
    length = size;
  }

  v8::Persistent<v8::Array> array = ((v8::Array *)pArray);
  v8::Handle<v8::Integer> zero = v8::Integer::New(0);

  jint *data = (jint *)(pEnv->GetPrimitiveArrayCritical(source, 0));

  for (size_t i = 0; i < length; i++) {
    jint d = data[i];

    if (d == 0) {
        array->Set(i, zero);
    } else {
        array->Set(i, v8::Integer::New(d));
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(source, data, 0);
}

JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetLongElements
  (JNIEnv *pEnv, jobject, jlong pArray, jlongArray source, jint length) 
{
  v8::HandleScope handleScope;
  int size = pEnv->GetArrayLength(source);

  if (length > size) {
    length = size;
  }

  v8::Persistent<v8::Array> array = ((v8::Array *)pArray);
  v8::Handle<v8::Number> zero = v8::Number::New(0.0);

  jlong *data = (jlong *)(pEnv->GetPrimitiveArrayCritical(source, 0));

  for (size_t i = 0; i < length; i++) {
    jlong d = data[i];

    if (d == 0) {
        array->Set(i, zero);
    } else {
        array->Set(i, v8::Number::New(d));
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(source, data, 0);
}

JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetShortElements
  (JNIEnv *pEnv, jobject, jlong pArray, jshortArray source, jint length)
{
  v8::HandleScope handleScope;
  int size = pEnv->GetArrayLength(source);

  if (length > size) {
    length = size;
  }

  v8::Persistent<v8::Array> array = ((v8::Array *)pArray);
  v8::Handle<v8::Integer> zero = v8::Integer::New(0);

  jshort *data = (jshort *)(pEnv->GetPrimitiveArrayCritical(source, 0));

  for (size_t i = 0; i < length; i++) {
    jshort d = data[i];

    if (d == 0) {
        array->Set(i, zero);
    } else {
        array->Set(i, v8::Integer::New(d));
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(source, data, 0);
}

JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetDoubleElements
  (JNIEnv *pEnv, jobject, jlong pArray, jdoubleArray source, jint length)
{
  v8::HandleScope handleScope;
  int size = pEnv->GetArrayLength(source);

  if (length > size) {
    length = size;
  }

  v8::Persistent<v8::Array> array = ((v8::Array *)pArray);
  v8::Handle<v8::Number> zero = v8::Number::New(0.0);

  jdouble *data = (jdouble *)(pEnv->GetPrimitiveArrayCritical(source, 0));

  for (size_t i = 0; i < length; i++) {
    jdouble d = data[i];

    if (d == 0.0) {
        array->Set(i, zero);
    } else {
        array->Set(i, v8::Number::New(d));
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(source, data, 0);
}

JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetFloatElements
  (JNIEnv *pEnv, jobject, jlong pArray, jfloatArray source, jint length)
{
  v8::HandleScope handleScope;
  int size = pEnv->GetArrayLength(source);

  if (length > size) {
    length = size;
  }

  v8::Persistent<v8::Array> array = ((v8::Array *)pArray);
  v8::Handle<v8::Number> zero = v8::Number::New(0.0);

  jfloat *data = (jfloat *)(pEnv->GetPrimitiveArrayCritical(source, 0));

  for (size_t i = 0; i < length; i++) {
    jfloat d = data[i];

    if (d == 0.0f) {
        array->Set(i, zero);
    } else {
        array->Set(i, v8::Number::New(d));
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(source, data, 0);
}

JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetBooleanElements
  (JNIEnv *pEnv, jobject, jlong pArray, jbooleanArray source, jint length)
{
  v8::HandleScope handleScope;
  int size = pEnv->GetArrayLength(source);

  if (length > size) {
    length = size;
  }

  v8::Persistent<v8::Array> array = ((v8::Array *)pArray);
  v8::Handle<v8::Value> trueValue = v8::True();
  v8::Handle<v8::Value> falseValue = v8::False();

  jboolean *data = (jboolean *)(pEnv->GetPrimitiveArrayCritical(source, 0));

  for (size_t i = 0; i < length; i++) {
    array->Set(i, data[i] == JNI_TRUE ? trueValue : falseValue);
  }

  pEnv->ReleasePrimitiveArrayCritical(source, data, 0);
}

JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetDateElements
  (JNIEnv *pEnv, jobject, jlong pArray, jobjectArray source, jint length)
{
  jni::V8Env env(pEnv);
  v8::Persistent<v8::Array> array = ((v8::Array *)pArray);
  v8::Handle<v8::Value> nullValue = v8::Null();

  for (size_t i = 0; i < length; i++) {
    jobject jdate = pEnv->GetObjectArrayElement(source, i);

    if (jdate == NULL) {
      array->Set(i, nullValue);
    } else {
      array->Set(i, env.WrapDate(jdate));
    }
  }
}

JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetV8ArrayElements
  (JNIEnv *pEnv, jobject, jlong pArray, jobjectArray source, jint length)
{
  jni::V8Env env(pEnv);
  v8::Persistent<v8::Array> array = ((v8::Array *)pArray);
  v8::Handle<v8::Value> nullValue = v8::Null();

  for (size_t i = 0; i < length; i++) {
    jobject jarr = pEnv->GetObjectArrayElement(source, i);

    if (jarr == NULL) {
      array->Set(i, nullValue);
    } else {
      array->Set(i, env.WrapV8Array(jarr));
    }
  }
}

JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetV8ObjectElements
  (JNIEnv *pEnv, jobject, jlong pArray, jobjectArray source, jint length)
{
  jni::V8Env env(pEnv);
  v8::Persistent<v8::Array> array = ((v8::Array *)pArray);
  v8::Handle<v8::Value> nullValue = v8::Null();

  for (size_t i = 0; i < length; i++) {
    jobject jobj = pEnv->GetObjectArrayElement(source, i);

    if (jobj == NULL) {
      array->Set(i, nullValue);
    } else {
      array->Set(i, env.WrapV8Object(jobj));
    }
  }
}

JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetStringElements
  (JNIEnv *pEnv, jobject, jlong pArray, jobjectArray source, jint length) {
  v8::HandleScope handleScope;
  v8::Persistent<v8::Array> array = ((v8::Array *)pArray);
  v8::Handle<v8::Value> nullValue = v8::Null();

  for (size_t i = 0; i < length; i++) {
    jstring str = (jstring)pEnv->GetObjectArrayElement(source, i);

    if (str == NULL) {
      array->Set(i, nullValue);
    } else {
      int length = pEnv->GetStringUTFLength(str);
      const jchar *p = pEnv->GetStringCritical(str, NULL);
      v8::Handle<v8::String> next = v8::String::New((const uint16_t *)p, length);
      pEnv->ReleaseStringCritical(str, p);
      array->Set(i, next);
    }
  }
}

jobjectArray JNICALL Java_lu_flier_script_V8Object_internalGetKeys(JNIEnv *pEnv, jobject pObj)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));

  v8::Handle<v8::Array> names = obj->GetPropertyNames();
  jobjectArray keys = env.NewObjectArray(names->Length(), "java/lang/String");

  for (size_t i=0; i<names->Length(); i++)
  {
    v8::Handle<v8::String> name = v8::Handle<v8::String>::Cast(names->Get(i));

    pEnv->SetObjectArrayElement(keys, i, env.NewString(name));
  }

  return keys;
}

jint JNICALL Java_lu_flier_script_V8Object_size(JNIEnv *pEnv, jobject pObj)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));

  return obj->GetPropertyNames()->Length();
}

jboolean JNICALL Java_lu_flier_script_V8Object_isEmpty(JNIEnv *pEnv, jobject pObj)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));

  return obj->GetPropertyNames()->Length() == 0;
}

void JNICALL Java_lu_flier_script_V8Object_clear(JNIEnv *pEnv, jobject pObj)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));

  v8::Handle<v8::Array> names = obj->GetPropertyNames();

  for (size_t i=0; i<names->Length(); i++)
  {
    obj->ForceDelete(names->Get(i));   
  }
}

jboolean JNICALL Java_lu_flier_script_V8Object_containsKey
  (JNIEnv *pEnv, jobject pObj, jobject pKey)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));
  const std::string key = env.GetString((jstring) pKey); 

  return obj->Has(v8::String::New(key.c_str(), key.size())) ? JNI_TRUE : JNI_FALSE;
}

jobject JNICALL Java_lu_flier_script_V8Object_internalGet
  (JNIEnv *pEnv, jobject pObj, jobject pKey)
{
  jni::V8Env env(pEnv);
  
  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));
  const std::string key = env.GetString((jstring) pKey);  

  v8::Handle<v8::Value> value = obj->Get(v8::String::New(key.c_str(), key.size()));

  return env.HasCaught() ? NULL : env.Wrap(value);
}

jobject JNICALL Java_lu_flier_script_V8Object_internalPut
  (JNIEnv *pEnv, jobject pObj, jstring pKey, jobject pValue)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));
  const std::string key = env.GetString((jstring) pKey);  
  v8::Handle<v8::String> name = v8::String::New(key.c_str(), key.size());

  v8::Handle<v8::Value> value = obj->Get(name);
  obj->Set(name, env.Wrap(pValue));

  return env.HasCaught() ? NULL : env.Wrap(value);
}

jobject JNICALL Java_lu_flier_script_V8Object_internalRemove
  (JNIEnv *pEnv, jobject pObj, jobject pKey)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Object> obj((v8::Object *) env.GetLongField(pObj, "obj"));
  const std::string key = env.GetString((jstring) pKey);  
  v8::Handle<v8::String> name = v8::String::New(key.c_str(), key.size());

  v8::Handle<v8::Value> value = obj->Get(name);
  obj->Delete(name);

  return env.HasCaught() ? NULL : env.Wrap(value);
}

jobject JNICALL Java_lu_flier_script_V8Array_internalGet
  (JNIEnv *pEnv, jobject pObj, jlong pArray, jint index)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Array> array((v8::Array *)pArray);
  v8::Handle<v8::Value> value = array->Get(index);

  return env.HasCaught() ? NULL : env.Wrap(value);
}

jobjectArray JNICALL Java_lu_flier_script_V8Array_internalToObjectArray
  (JNIEnv *pEnv, jobject pObj, jlong pArray, jint length)
{
  jni::V8Env env(pEnv);

  v8::Handle<v8::Array> array((v8::Array *)pArray);

  return env.HasCaught() ? NULL : env.WrapArrayToNative(array);
}

JNIEXPORT jintArray JNICALL Java_lu_flier_script_V8Array_internalToIntArray
  (JNIEnv *pEnv, jobject pObj, jlong pArray, jintArray buf, jint length) 
{
  v8::HandleScope handleScope;
  v8::Handle<v8::Array> array((v8::Array *)pArray);

  jint *data = (jint *)(pEnv->GetPrimitiveArrayCritical(buf, 0));

  for (size_t i=0; i<length; i++)
  {      
    int value = array->Get(i)->Int32Value();

    if (value != 0) {
      data[i] = value;
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(buf, data, 0);
}

JNIEXPORT jlongArray JNICALL Java_lu_flier_script_V8Array_internalToLongArray
  (JNIEnv *pEnv, jobject pObj, jlong pArray, jlongArray buf, jint length) 
{
  v8::HandleScope handleScope;
  v8::Handle<v8::Array> array((v8::Array *)pArray);

  jlong *data = (jlong *)(pEnv->GetPrimitiveArrayCritical(buf, 0));

  for (size_t i=0; i<length; i++)
  {      
    double value = array->Get(i)->NumberValue();

    if (value != 0.0) {
      data[i] = value;
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(buf, data, 0);
}

JNIEXPORT jshortArray JNICALL Java_lu_flier_script_V8Array_internalToShortArray
  (JNIEnv *pEnv, jobject pObj, jlong pArray, jshortArray buf, jint length) 
{
  v8::HandleScope handleScope;
  v8::Handle<v8::Array> array((v8::Array *)pArray);

  jshort *data = (jshort *)(pEnv->GetPrimitiveArrayCritical(buf, 0));

  for (size_t i=0; i<length; i++)
  {      
    int value = array->Get(i)->Int32Value();

    if (value != 0) {
      data[i] = value;
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(buf, data, 0);
}

JNIEXPORT jdoubleArray JNICALL Java_lu_flier_script_V8Array_internalToDoubleArray
  (JNIEnv *pEnv, jobject pObj, jlong pArray, jdoubleArray buf, jint length) 
{
  v8::HandleScope handleScope;
  v8::Handle<v8::Array> array((v8::Array *)pArray);

  jdouble *data = (jdouble *)(pEnv->GetPrimitiveArrayCritical(buf, 0));

  for (size_t i=0; i<length; i++)
  {      
    double value = array->Get(i)->NumberValue();

    if (value != 0.0) {
      data[i] = value;
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(buf, data, 0);
}

JNIEXPORT jfloatArray JNICALL Java_lu_flier_script_V8Array_internalToFloatArray
  (JNIEnv *pEnv, jobject pObj, jlong pArray, jfloatArray buf, jint length) 
{
  v8::HandleScope handleScope;
  v8::Handle<v8::Array> array((v8::Array *)pArray);

  jfloat *data = (jfloat *)(pEnv->GetPrimitiveArrayCritical(buf, 0));

  for (size_t i=0; i<length; i++)
  {      
    double value = array->Get(i)->NumberValue();

    if (value != 0.0) {
      data[i] = value;
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(buf, data, 0);
}

JNIEXPORT jbooleanArray JNICALL Java_lu_flier_script_V8Array_internalToBooleanArray
  (JNIEnv *pEnv, jobject pObj, jlong pArray, jbooleanArray buf, jint length) 
{
  v8::HandleScope handleScope;
  v8::Handle<v8::Array> array((v8::Array *)pArray);

  jboolean *data = (jboolean *)(pEnv->GetPrimitiveArrayCritical(buf, 0));

  for (size_t i=0; i<length; i++)
  {      
    if (array->Get(i)->IsTrue()) {
      data[i] = JNI_TRUE;
    }
  }

  pEnv->ReleasePrimitiveArrayCritical(buf, data, 0);
}

JNIEXPORT jobjectArray JNICALL Java_lu_flier_script_V8Array_internalToStringArray
  (JNIEnv *pEnv, jobject, jlong pArray, jobjectArray dest, jint length)
{
  v8::HandleScope handleScope;
  v8::Handle<v8::Array> array((v8::Array *)pArray);
  v8::Handle<v8::Value> value;

  for (size_t i=0; i<length; i++)
  {      
    value = array->Get(i);

    if (!value->IsNull()) {
      v8::String::Utf8Value str(v8::Handle<v8::String>::Cast(value));
      pEnv->SetObjectArrayElement(dest, i, pEnv->NewStringUTF(*str));
    }
  }
}

JNIEXPORT jobjectArray JNICALL Java_lu_flier_script_V8Array_internalToDateArray
  (JNIEnv *pEnv, jobject, jlong pArray, jobjectArray dest, jint length) 
{
  jni::V8Env env(pEnv);
  v8::HandleScope handleScope;
  v8::Handle<v8::Array> array((v8::Array *)pArray);

  for (size_t i=0; i<length; i++)
  {      
    v8::Handle<v8::Value> value = array->Get(i);

    if (!value->IsNull()) {
      pEnv->SetObjectArrayElement(dest, i, env.NewDate(v8::Handle<v8::Date>::Cast(value)));
    }
  }
} 

jint JNICALL Java_lu_flier_script_V8Array_internalGetSize
  (JNIEnv *pEnv, jobject pObj, jlong pArray)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Array> array((v8::Array *)pArray);

  return array->Length();
}

jobject JNICALL Java_lu_flier_script_V8Function_internalInvoke
  (JNIEnv *pEnv, jobject pObj, jlong pFunc, jlong pThiz, jobjectArray pArgs)
{
  jni::V8Env env(pEnv);

  v8::Persistent<v8::Function> func((v8::Function *)pFunc);
  v8::Handle<v8::Object> thiz = v8::Persistent<v8::Object>((v8::Object *) pThiz);

  if (thiz.IsEmpty()) thiz = v8::Context::GetCurrent()->Global();
  std::vector< v8::Handle<v8::Value> > args;
  
  if (pArgs) args = env.GetArray(pArgs);

  v8::Handle<v8::Value> result = func->Call(thiz, args.size(), args.empty() ? NULL : &args[0]);

  return env.Wrap(result);
}
