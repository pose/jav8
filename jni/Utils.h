#pragma once

#include <string>
#include <vector>

#include <jni.h>

#include <v8.h>

namespace jni {

class Env
{
protected:
  JNIEnv *m_env;
  
  const std::string Extract(const v8::TryCatch& try_catch);
public:
  Env(JNIEnv *env) : m_env(env) {}
  virtual ~Env() {}

  operator JNIEnv *() const { return m_env; }
  JNIEnv *operator ->() { return m_env; }

  const std::string GetString(jstring str);

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

  jmethodID GetMethodID(const char * clazz, const char * name, const char *sig) {
    return GetMethodID(FindClass(clazz), name, sig);
  }

  jmethodID GetStaticMethodID(jclass clazz, const char * name, const char *sig) {
    // TODO cache it with TLS
    return m_env->GetStaticMethodID(clazz, name, sig);
  }

  jmethodID GetStaticMethodID(const char * clazz, const char * name, const char *sig) {
    return GetStaticMethodID(FindClass(clazz), name, sig);
  }

  jobject GetField(jobject obj, const char *name, const char *sig);
  jlong GetLongField(jobject obj, const char *name);
  jobject GetStaticField(jobject obj, const char *name, const char *sig);

  bool IsAssignableFrom(jclass sub, const char *sup) {
    return m_env->IsAssignableFrom(sub, FindClass(sup)) == JNI_TRUE;
  }
  bool IsAssignableFrom(const char * sub, jclass sup) {
    return m_env->IsAssignableFrom(FindClass(sub), sup) == JNI_TRUE;
  }

  jobject NewObject(const char *name, const char *sig = "()V", ...);
  jobjectArray NewObjectArray(size_t size, const char *name = "java/lang/Object", jobject init = NULL);
  
  jobject NewBoolean(jboolean value);
  jobject NewInt(jint value);
  jobject NewLong(jlong value);
  jobject NewDouble(jdouble value);
  jstring NewString(v8::Handle<v8::String> str);
  jobject NewDate(v8::Handle<v8::Date> date);
  jobject NewV8Object(v8::Handle<v8::Object> obj);
  jobject NewV8Array(v8::Handle<v8::Array> array);
  jobject NewV8Function(v8::Handle<v8::Function> func);
  jobject NewV8Context(v8::Handle<v8::Context> ctxt);

  void Throw(const char *type, const char *msg);
  bool ThrowIf(const v8::TryCatch& try_catch);
};

class V8Env : public Env {
  v8::HandleScope handle_scope;  
  v8::TryCatch try_catch;
public:
  V8Env(JNIEnv *env) : Env(env) {}
  virtual ~V8Env() { ThrowIf(try_catch); }

  bool HasCaught() const { return try_catch.HasCaught(); }

  std::vector< v8::Handle<v8::Value> > GetArray(jobjectArray array);
  
  jobject Wrap(v8::Handle<v8::Value> value);
  jobject Wrap(v8::Handle<v8::Object> value);
  v8::Handle<v8::Value> Wrap(jobject value);

  template <class T> v8::Local<T> Close(v8::Handle<T> value) { return handle_scope.Close(value); }
};

} // namespace jni
