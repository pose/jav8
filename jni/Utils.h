#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>

#include <jni.h>

#include <v8.h>

#include "Config.h"

#ifdef _WIN32

#ifndef _USE_32BIT_TIME_T
# define _USE_32BIT_TIME_T
#endif

#else

#include <cmath>
using std::isnan;

#ifndef isfinite
# include <limits>
# define isfinite(val) (val <= std::numeric_limits<double>::max())
#endif

#include <strings.h>
#define strnicmp strncasecmp

#define _countof(array) (sizeof(array)/sizeof(array[0]))

#endif

#ifdef __APPLE__
# include <pthread.h>
#endif

namespace jni {

class Cache
{
  typedef std::map<JNIEnv *, Cache *> caches_t;

  typedef std::map<std::string, jclass> classes_t;

  typedef std::map<std::pair<jclass, jclass>, bool> assignables_t;

  typedef std::map<std::pair<std::string, std::string>, jfieldID> fieldIDs_t;
  typedef std::map<std::pair<jclass, bool>, fieldIDs_t> fieldIDsByClass_t;

  typedef std::map<std::pair<std::string, std::string>, jmethodID> methodIDs_t;
  typedef std::map<std::pair<jclass, bool>, methodIDs_t> methodIDsByClass_t;

  typedef std::vector<jclass> types_t;
  typedef std::pair<jobject, types_t> method_t;

  typedef std::map<std::string, jobject> fields_t;
  typedef std::map<std::string, std::vector<method_t> > methods_t;
  typedef std::map<jclass, std::pair<fields_t, methods_t> > members_t;

  JNIEnv *m_env;

  classes_t m_classes;
  assignables_t m_assignables;
  fieldIDsByClass_t m_fieldIDs;
  methodIDsByClass_t m_methodIDs;
  members_t m_members;

#ifdef _MSC_VER
  __declspec( thread ) static caches_t *s_caches;
#else 
# ifdef __APPLE__
  static pthread_key_t s_caches_key;
# else
  static __thread caches_t *s_caches;
# endif
#endif

  void FillBuildIns(void);
  members_t::iterator CacheMembers(jclass clazz);

  jfieldID InternalGetFieldID(jclass clazz, bool statiz, const char * name, const char *sig);
  jmethodID InternalGetMethodID(jclass clazz, bool statiz, const char * name, const char *sig);
public:
  Cache(JNIEnv *env) : m_env(env) { FillBuildIns(); }
  virtual ~Cache(void) { Clear(); }

  static Cache& GetInstance(JNIEnv *env);

  void Clear(void);

  jclass FindClass(const char *name);

  bool IsAssignableFrom(jclass sub, jclass sup);

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

  v8::Handle<v8::Value> GetMember(jobject obj, const std::string& name);
  v8::Handle<v8::Value> SetMember(jobject obj, const std::string& name, v8::Handle<v8::Value> value);
  bool HasMember(jobject obj, const std::string& name);
  v8::Handle<v8::Array> GetMembers(jobject obj);

  struct buildins_t {
    struct {
      struct {
        jclass Class;
        jclass Boolean;
        jclass Number;
        jclass Byte;
        jclass Short;
        jclass Integer;
        jclass Long;
        jclass Float;
        jclass Double;
        jclass String;
        jclass RuntimeException;

        struct {
          jclass Field;
          jclass Method;
          jclass Array;
        } reflect;
      } lang;

      struct {
        jclass Date;
      } util;
    } java;

    struct {
      struct {
        struct {
          jclass V8Object;
          jclass V8Array;
          jclass V8Function;
          jclass V8Context;
        } script;
      } flier;
    } lu;
  };
  
  buildins_t buildins;
};

class Env
{
protected:
  JNIEnv *m_env;
  
  const std::string Extract(const v8::TryCatch& try_catch);
public:
  Env(JNIEnv *env) : m_env(env), buildins(Cache::GetInstance(env).buildins) {}
  virtual ~Env() {}

  Cache::buildins_t& buildins;

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
    return IsAssignableFrom(sub, FindClass(sup));
  }
  bool IsAssignableFrom(const char * sub, jclass sup) {
    return IsAssignableFrom(FindClass(sub), sup);
  }
  bool IsAssignableFrom(jclass sub, jclass sup) {
    return m_env->IsAssignableFrom(sub, sup) == JNI_TRUE;
    //return Cache::GetInstance(m_env).IsAssignableFrom(sub, sup) == JNI_TRUE;
  }

  v8::Handle<v8::Value> GetMember(jobject obj, const std::string& name)
  {
    return Cache::GetInstance(m_env).GetMember(obj, name);
  }
  v8::Handle<v8::Value> SetMember(jobject obj, const std::string& name, v8::Handle<v8::Value> value)
  {
    return Cache::GetInstance(m_env).SetMember(obj, name, value);
  }
  bool HasMember(jobject obj, const std::string& name)
  {
    return Cache::GetInstance(m_env).HasMember(obj, name);
  }
  v8::Handle<v8::Array> GetMembers(jobject obj)
  {
    return Cache::GetInstance(m_env).GetMembers(obj);
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

  void Throw(jclass type, const char *msg);
  void Throw(const char *type, const char *msg);
  bool ThrowIf(const v8::TryCatch& try_catch);
};

class V8Isolate {
  public: 
    static bool IsAlive();
    static void ensureInIsolate();
};

class V8Env : public Env, public V8Isolate {
  v8::HandleScope handle_scope;  
  v8::TryCatch try_catch;
public:
  V8Env(JNIEnv *env) : Env(env) 
  {
  }
  virtual ~V8Env() { ThrowIf(try_catch); }

  bool HasCaught() const { return try_catch.HasCaught(); }

  std::vector< v8::Handle<v8::Value> > GetArray(jobjectArray array);
  
  jobject Wrap(v8::Handle<v8::Value> value);
  jobject Wrap(v8::Handle<v8::Object> value);
  jobjectArray WrapArrayToNative(v8::Handle<v8::Value> obj);
  v8::Handle<v8::Value> Wrap(jobject value);

  template <class T> v8::Local<T> Close(v8::Handle<T> value) { return handle_scope.Close(value); }
};

} // namespace jni
