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
  (JNIEnv *pEnv, jobject) {
  jni::V8Env env(pEnv);

  return env.NewV8Object(v8::Object::New());
}

JNIEXPORT jobject JNICALL Java_lu_flier_script_V8Context_internalCreateArray
  (JNIEnv *pEnv, jobject, jobjectArray source) {
  jni::V8Env env(pEnv);

  int size = pEnv->GetArrayLength(source);
  v8::Handle<v8::Array> array = v8::Array::New(size);

  for (size_t i = 0; i < size; i++) {
      jobject item = pEnv->GetObjectArrayElement(source, i);
      array->Set(i, env.Wrap(item));
  }

  return env.NewV8Array(array);
}

JNIEXPORT void JNICALL Java_lu_flier_script_V8Array_internalSetElements
  (JNIEnv *pEnv, jobject, jlong pArray, jobjectArray elements) {
  jni::V8Env env(pEnv);

  int size = pEnv->GetArrayLength(elements);
  v8::Persistent<v8::Array> array((v8::Array *)pArray);

  for (size_t i = 0; i < size; i++) {
      jobject item = pEnv->GetObjectArrayElement(elements, i);
      array->Set(i, env.Wrap(item));
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
  (JNIEnv *pEnv, jobject pObj, jlong pArray)
{
  jni::V8Env env(pEnv);

  v8::Handle<v8::Array> array((v8::Array *)pArray);

  return env.HasCaught() ? NULL : env.WrapArrayToNative(array);
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
