#pragma once

#include <string>
#include <vector>
#include <map>

#include <jni.h>

#include <v8.h>

namespace jni {

class Cache
{
  typedef std::map<JNIEnv *, std::auto_ptr<Cache> > caches_t;

  typedef std::map<std::string, jclass> classes_t;

  typedef std::map<std::pair<std::string, std::string>, jfieldID> fieldIDs_t;
  typedef std::map<std::pair<jclass, bool>, fieldIDs_t> fieldIDsByClass_t;

  typedef std::map<std::pair<std::string, std::string>, jmethodID> methodIDs_t;
  typedef std::map<std::pair<jclass, bool>, methodIDs_t> methodIDsByClass_t;

  JNIEnv *m_env;

  classes_t m_classes;
  fieldIDsByClass_t m_fieldIDs;
  methodIDsByClass_t m_methodIDs;

  jfieldID InternalGetFieldID(jclass clazz, bool statiz, const char * name, const char *sig);
  jmethodID InternalGetMethodID(jclass clazz, bool statiz, const char * name, const char *sig);
public:
  Cache(JNIEnv *env) : m_env(env) {}
  virtual ~Cache(void) { Clear(); }

  static Cache& GetInstance(JNIEnv *env);

  void Clear(void);

  jclass FindClass(const char *name);

  jfieldID GetFieldID(jclass clazz, const char * name, const char *sig)
  {
    return InternalGetFieldID(clazz, false, name, sig);
  }
  jfieldID GetStaticFieldID(jclass clazz, const char * name, const char *sig)
  {
    return InternalGetFieldID(clazz, true, name, sig);
  }
  jmethodID GetMethodID(jclass clazz, const char * name, const char *sig)
  {
    return InternalGetMethodID(clazz, false, name, sig);
  }
  jmethodID GetStaticMethodID(jclass clazz, const char * name, const char *sig)
  {
    return InternalGetMethodID(clazz, true, name, sig);
  }
};

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
    return Cache::GetInstance(m_env).FindClass(name); 
  }

  jfieldID GetFieldID(jclass clazz, const char * name, const char *sig) {
    return Cache::GetInstance(m_env).GetFieldID(clazz, name, sig);
  }
  jfieldID GetFieldID(const char * clazz, const char * name, const char *sig) {
    return Cache::GetInstance(m_env).GetFieldID(FindClass(clazz), name, sig);
  }

  jfieldID GetStaticFieldID(jclass clazz, const char * name, const char *sig) {
    return Cache::GetInstance(m_env).GetStaticFieldID(clazz, name, sig);
  }
  jfieldID GetStaticFieldID(const char * clazz, const char * name, const char *sig) {
    return Cache::GetInstance(m_env).GetStaticFieldID(FindClass(clazz), name, sig);
  }

  jmethodID GetMethodID(jclass clazz, const char * name, const char *sig) {    
    return Cache::GetInstance(m_env).GetMethodID(clazz, name, sig);
  }
  jmethodID GetMethodID(const char * clazz, const char * name, const char *sig) {
    return GetMethodID(FindClass(clazz), name, sig);
  }

  jmethodID GetStaticMethodID(jclass clazz, const char * name, const char *sig) {    
    return Cache::GetInstance(m_env).GetStaticMethodID(clazz, name, sig);
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
