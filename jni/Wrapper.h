#pragma once

#include <map>
#include <vector>

#include <jni.h>

#include <v8.h>

namespace jni {

class CManagedObject {
protected:
  JNIEnv *m_pEnv;
  jobject m_obj;

  CManagedObject(JNIEnv *pEnv, jobject obj) 
    : m_pEnv(pEnv), m_obj(m_pEnv->NewGlobalRef(obj)) 
  {
  }
public:
  virtual ~CManagedObject(void) 
  {
    m_pEnv->DeleteGlobalRef(m_obj);
  }

  JNIEnv *GetEnv(void) const { return m_pEnv; }
  jobject GetObject(void) const { return m_obj; }

  static bool IsWrapped(v8::Handle<v8::Object> obj)
  {
    return !obj.IsEmpty() && obj->InternalFieldCount() == 1;
  }

  static CManagedObject& Unwrap(v8::Handle<v8::Object> obj) 
  {
    v8::HandleScope handle_scope;

    return *static_cast<CManagedObject *>(v8::Handle<v8::External>::Cast(obj->GetInternalField(0))->Value());
  }
};

template <class T>
class CBaseJavaObject : public CManagedObject {
protected:
  typedef CManagedObject __base__;
  typedef std::vector<jobject> objects_t;

  static v8::Handle<v8::Value> NamedGetter(
    v8::Local<v8::String> prop, const v8::AccessorInfo& info)
  { return v8::Handle<v8::Value>(); }
  static v8::Handle<v8::Value> NamedSetter(
    v8::Local<v8::String> prop, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
  { return v8::Handle<v8::Value>(); }
  static v8::Handle<v8::Integer> NamedQuery(
    v8::Local<v8::String> prop, const v8::AccessorInfo& info)
  { return v8::Handle<v8::Integer>(); }
  static v8::Handle<v8::Boolean> NamedDeleter(
    v8::Local<v8::String> prop, const v8::AccessorInfo& info)
  { return v8::False(); }
  static v8::Handle<v8::Array> NamedEnumerator(const v8::AccessorInfo& info)
  { return v8::Handle<v8::Array>(); }

  static v8::Handle<v8::Value> IndexedGetter(
    uint32_t index, const v8::AccessorInfo& info)
  { return v8::Handle<v8::Value>(); }
  static v8::Handle<v8::Value> IndexedSetter(
    uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
  { return v8::Handle<v8::Value>(); }
  static v8::Handle<v8::Integer> IndexedQuery(
    uint32_t index, const v8::AccessorInfo& info)
  { return v8::Handle<v8::Integer>(); }
  static v8::Handle<v8::Boolean> IndexedDeleter(
    uint32_t index, const v8::AccessorInfo& info)
  { return v8::False(); }
  static v8::Handle<v8::Array> IndexedEnumerator(const v8::AccessorInfo& info)
  { return v8::Handle<v8::Array>(); }

  static v8::Handle<v8::Value> Caller(const v8::Arguments& args) 
  { return v8::Handle<v8::Value>(); }
private:
  static void SetupObjectTemplate(v8::Handle<v8::ObjectTemplate> clazz)
  {
    clazz->SetInternalFieldCount(1);
    clazz->SetNamedPropertyHandler(T::NamedGetter, T::NamedSetter, T::NamedQuery, T::NamedDeleter, T::NamedEnumerator);
    clazz->SetIndexedPropertyHandler(T::IndexedGetter, T::IndexedSetter, T::IndexedQuery, T::IndexedDeleter, T::IndexedEnumerator);
    clazz->SetCallAsFunctionHandler(T::Caller);
  }

  static v8::Persistent<v8::ObjectTemplate> CreateObjectTemplate(void)
  {
    v8::HandleScope handle_scope;

    v8::Handle<v8::ObjectTemplate> clazz = v8::ObjectTemplate::New();

    SetupObjectTemplate(clazz);

    return v8::Persistent<v8::ObjectTemplate>::New(clazz);
  }
protected:
  static v8::Handle<v8::Object> InternalWrap(T *obj)
  {
    v8::HandleScope handle_scope;
    v8::TryCatch try_catch;

    static v8::Persistent<v8::ObjectTemplate> s_template(CreateObjectTemplate());

    v8::Handle<v8::Object> instance = s_template->NewInstance();    

    // TODO trace the life cycle with a weak reference

    instance->SetInternalField(0, v8::External::New(obj));

    return handle_scope.Close(instance);
  }
public:
  CBaseJavaObject(JNIEnv *pEnv, jobject obj) : __base__(pEnv, obj) 
  {

  }

  static v8::Handle<v8::Object> Wrap(JNIEnv *pEnv, jobject obj)
  {
    v8::HandleScope handle_scope;

    return handle_scope.Close(InternalWrap(new T(pEnv, obj)));
  }

  static T& Unwrap(v8::Handle<v8::Object> obj) 
  {
    return static_cast<T&>(__base__::Unwrap(obj));
  }
};

class CJavaObject : public CBaseJavaObject<CJavaObject> {
  typedef CBaseJavaObject<CJavaObject> __base__;

  typedef std::vector<jclass> types_t;
  typedef std::pair<jobject, types_t> method_t;

  typedef std::map<std::string, jobject> fields_t;
  typedef std::map<std::string, std::vector<method_t> > methods_t;

  fields_t m_fields;
  methods_t m_methods;

  void CacheNames(void);
public:
  CJavaObject(JNIEnv *pEnv, jobject obj) : __base__(pEnv, obj) {
    CacheNames();
  }

  virtual ~CJavaObject(void);
  
  static v8::Handle<v8::Value> NamedGetter(
    v8::Local<v8::String> prop, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> NamedSetter(
    v8::Local<v8::String> prop, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
  static v8::Handle<v8::Integer> NamedQuery(
    v8::Local<v8::String> prop, const v8::AccessorInfo& info);
  static v8::Handle<v8::Array> NamedEnumerator(const v8::AccessorInfo& info);    
};

class CJavaArray : public CBaseJavaObject<CJavaArray> {
  typedef CBaseJavaObject<CJavaArray> __base__;
public:
  CJavaArray(JNIEnv *pEnv, jobject obj) : __base__(pEnv, obj) {
  }

  size_t GetLength(void) { return m_pEnv->GetArrayLength((jarray) m_obj); }

  static v8::Handle<v8::Value> NamedGetter(
    v8::Local<v8::String> prop, const v8::AccessorInfo& info);
  static v8::Handle<v8::Integer> NamedQuery(
    v8::Local<v8::String> prop, const v8::AccessorInfo& info);
  
  static v8::Handle<v8::Value> IndexedGetter(
    uint32_t index, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> IndexedSetter(
    uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
  static v8::Handle<v8::Integer> IndexedQuery(
    uint32_t index, const v8::AccessorInfo& info);
  static v8::Handle<v8::Array> IndexedEnumerator(const v8::AccessorInfo& info);
};

class CJavaFunction {
  typedef CManagedObject __base__;

  typedef std::vector<jclass> types_t;
  typedef std::pair<jobject, types_t> method_t;
  typedef std::vector<method_t> methods_t;

  JNIEnv *m_pEnv;
  methods_t m_methods;

  bool CanConvert(jclass clazz, v8::Handle<v8::Value> value);
public:
  CJavaFunction(JNIEnv *pEnv, jobject obj);
  CJavaFunction(JNIEnv *pEnv, const methods_t& methods) 
    : m_pEnv(pEnv), m_methods(methods)
  {
  }
  virtual ~CJavaFunction(void)
  {
    ReleaseMethods(m_pEnv, m_methods);
  }

  JNIEnv *GetEnv(void) { return m_pEnv; }
  jobject GetMethod(const v8::Arguments& args);

  static void ReleaseMethods(JNIEnv *pEnv, const methods_t& methods);
  static const types_t GetParameterTypes(JNIEnv *pEnv, jobject method);

  template <typename T>
  static v8::Handle<v8::Value> Wrap(JNIEnv *pEnv, T methods)
  {
    jni::V8Env env(pEnv);

    v8::Handle<v8::FunctionTemplate> func_tmpl = v8::FunctionTemplate::New();    

    func_tmpl->SetCallHandler(Caller, v8::External::New(new CJavaFunction(pEnv, methods)));

    return env.Close(func_tmpl->GetFunction());
  }
  static v8::Handle<v8::Value> Caller(const v8::Arguments& args);
};

class CJavaContext : public CBaseJavaObject<CJavaContext> {
  typedef CBaseJavaObject<CJavaContext> __base__;
public:
  CJavaContext(JNIEnv *pEnv, jobject obj) : __base__(pEnv, obj) {
  }
public:
  /*
  static v8::Handle<v8::Value> NamedGetter(
    v8::Local<v8::String> prop, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> NamedSetter(
    v8::Local<v8::String> prop, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
  static v8::Handle<v8::Integer> NamedQuery(
    v8::Local<v8::String> prop, const v8::AccessorInfo& info);
  static v8::Handle<v8::Boolean> NamedDeleter(
    v8::Local<v8::String> prop, const v8::AccessorInfo& info);
  static v8::Handle<v8::Array> NamedEnumerator(const v8::AccessorInfo& info);  
  */
};

} // namespace jni
