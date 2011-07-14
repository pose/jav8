#pragma once

#include <string>

#include <jni.h>

#include <v8.h>

namespace jni {

class Env
{
protected:
  JNIEnv *m_env;

  jclass FindClass(const char *name) { 
    // TODO cache it with TLS
    return m_env->FindClass(name); 
  }

  jfieldID GetFieldID(jclass clazz, const char * name, const char *sig) {
    // TODO cache it with TLS
    return m_env->GetFieldID(clazz, name, sig);
  }

  jfieldID GetStaticFieldID(jclass clazz, const char * name, const char *sig) {
    // TODO cache it with TLS
    return m_env->GetStaticFieldID(clazz, name, sig);
  }

  jmethodID GetMethodID(jclass clazz, const char * name, const char *sig) {
    // TODO cache it with TLS
    return m_env->GetMethodID(clazz, name, sig);
  }

  jmethodID GetStaticMethodID(jclass clazz, const char * name, const char *sig) {
    // TODO cache it with TLS
    return m_env->GetStaticMethodID(clazz, name, sig);
  }
  
  const std::string Extract(v8::TryCatch& try_catch);
public:
  Env(JNIEnv *env) : m_env(env) {}
  virtual ~Env() {}

  operator JNIEnv *() const { return m_env; }

  const std::string GetString(jstring str);

  jobject GetField(jobject obj, const char *name, const char *sig);
  jlong GetLongField(jobject obj, const char *name);
  jobject GetStaticField(jobject obj, const char *name, const char *sig);

  jobject NewObject(const char *name, const char *sig = "()V", ...);
  jobjectArray NewObjectArray(const char *name, size_t size, jobject init = NULL);
  
  jobject NewBoolean(jboolean value);
  jobject NewInt(jint value);
  jobject NewLong(jlong value);
  jobject NewDouble(jdouble value);
  jstring NewString(v8::Handle<v8::String> str);
  jobject NewDate(v8::Handle<v8::Date> date);
  jobject NewV8Object(v8::Handle<v8::Object> obj);
  jobject NewV8Context(v8::Handle<v8::Context> ctxt);

  void Throw(const char *type, const char *msg);
  bool ThrowIf(v8::TryCatch try_catch);
};

class V8Env : public Env {
  v8::HandleScope handle_scope;  
  v8::TryCatch m_tc;
public:
  V8Env(JNIEnv *env) : Env(env) {}
  virtual ~V8Env() { ThrowIf(m_tc); }

  bool HasCaught() const { return m_tc.HasCaught(); }

  jobject Wrap(v8::Handle<v8::Value> value);
  v8::Handle<v8::Value> Wrap(jobject value);
};

} // namespace jni
