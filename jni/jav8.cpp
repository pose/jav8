#include "jav8.h"

#include <cstring>
#include <iostream>
#include <memory>

#include <v8.h>

#include "Utils.h"

void JNICALL Java_lu_flier_script_V8ScriptEngineFactory_initialize(JNIEnv *, jclass)
{
  v8::V8::Initialize(); 
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

  env.Throw("java/lang/IllegalArgumentException", "Invalid key");
}

jobject JNICALL Java_lu_flier_script_V8CompiledScript_internalExecute
	(JNIEnv *pEnv, jobject pObj, jlong pCompiledScript, jobject pContext)
{
  jni::Env env(pEnv);

  v8::HandleScope handle_scope;
  v8::TryCatch try_catch;

  v8::Persistent<v8::Script> compiledScript((v8::Script *) pCompiledScript);

  v8::Handle<v8::Value> result = compiledScript->Run();

  if (result.IsEmpty())
  {
    if (try_catch.HasCaught()) 
    {
      env.Throw(try_catch);
    }
    else
    {
      result = v8::Null();
    }
  }

  return env.Wrap(result);
}

jlong JNICALL Java_lu_flier_script_V8CompiledScript_internalCompile
  (JNIEnv *pEnv, jobject pObj, jstring pScript)
{
  jni::Env env(pEnv);

  if (!v8::Context::InContext())
  {
    env.Throw("java/lang/IllegalStateException", "Not in context");
  }

  const std::string script = env.GetString(pScript);

  v8::HandleScope handle_scope;
  v8::TryCatch try_catch;

  v8::Handle<v8::Script> compiledScript = v8::Script::Compile(v8::String::New(script.c_str(), script.size()));

  if (compiledScript.IsEmpty())
  {
    env.Throw(try_catch);
  }
  else
  {
    return (jlong) *v8::Persistent<v8::Script>::New(compiledScript);
  }
}

void JNICALL Java_lu_flier_script_V8CompiledScript_internalRelease
  (JNIEnv *pEnv, jobject pObj, jlong ptr)
{
  if (ptr) { v8::Persistent<v8::Script>((v8::Script *) ptr).Dispose(); }
}

void JNICALL Java_lu_flier_script_V8Object_internalRelease
  (JNIEnv *pEnv, jobject pObj, jlong ptr)
{
  if (ptr) { v8::Persistent<v8::Object>((v8::Object *) ptr).Dispose(); }
}

jobject JNICALL Java_lu_flier_script_V8Context_getEntered(JNIEnv *pEnv, jclass)
{
  if (!v8::Context::InContext()) return NULL;

  return jni::Env(pEnv).NewV8Context(v8::Context::GetEntered());
}

jobject JNICALL Java_lu_flier_script_V8Context_getCurrent(JNIEnv *pEnv, jclass)
{
  if (!v8::Context::InContext()) return NULL;

  return jni::Env(pEnv).NewV8Context(v8::Context::GetCurrent());
}

jobject JNICALL Java_lu_flier_script_V8Context_getCalling(JNIEnv *pEnv, jclass)
{
  if (!v8::Context::InContext()) return NULL;

  return jni::Env(pEnv).NewV8Context(v8::Context::GetCalling());
}

jboolean JNICALL Java_lu_flier_script_V8Context_inContext(JNIEnv *, jclass)
{
  return v8::Context::InContext() ? JNI_TRUE : JNI_FALSE;
}

jlong JNICALL Java_lu_flier_script_V8Context_internalCreate
  (JNIEnv *pEnv, jclass)
{
  v8::HandleScope handle_scope;  

  return (jlong) *v8::Persistent<v8::Context>::New(v8::Context::New());
}

void JNICALL Java_lu_flier_script_V8Context_internalRelease
  (JNIEnv *, jobject, jlong ptr)
{
  if (ptr) { v8::Persistent<v8::Context>((v8::Context *) ptr).Dispose(); }
}

void JNICALL Java_lu_flier_script_V8Context_internalEnter
  (JNIEnv *, jobject, jlong ptr)
{
  if (ptr) { v8::Persistent<v8::Context>((v8::Context *) ptr)->Enter(); }
}

void JNICALL Java_lu_flier_script_V8Context_internalLeave
  (JNIEnv *, jobject, jlong ptr)
{
  if (ptr) { v8::Persistent<v8::Context>((v8::Context *) ptr)->Exit(); }
}