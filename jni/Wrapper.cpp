#include "Wrapper.h"

#include "Utils.h"

namespace jni {

CJavaObject::~CJavaObject(void)
{
  for (fields_t::const_iterator it = m_fields.begin(); it != m_fields.end(); it++)
  {
    m_pEnv->DeleteGlobalRef(it->second);
  }

  m_fields.clear();

  for (methods_t::const_iterator it = m_methods.begin(); it != m_methods.end(); it++)
  {
    for (size_t i=0; i<it->second.size(); i++)
    {
      m_pEnv->DeleteGlobalRef(it->second[i]);
    }
  }
}

void CJavaObject::CacheNames(void) 
{
  jni::Env env(m_pEnv);

  jmethodID mid = env.GetMethodID("java/lang/Class", "getFields", "()[Ljava/lang/reflect/Field;");
  jobjectArray fields = (jobjectArray) m_pEnv->CallObjectMethod(env->GetObjectClass(m_obj), mid);
  
  mid = NULL;
  for (size_t i=0; i<env->GetArrayLength(fields); i++)
  {
    jobject field = env->GetObjectArrayElement(fields, i);

    if (!mid) {
      mid = env.GetMethodID("java/lang/reflect/Field", "getName", "()Ljava/lang/String;");
    }
    
    std::string name = env.GetString((jstring) env->CallObjectMethod(field, mid));
    
    m_fields[name] = env->NewGlobalRef(field);

    env->DeleteLocalRef(field);
  }

  mid = env.GetMethodID("java/lang/Class", "getMethods", "()[Ljava/lang/reflect/Method;");
  jobjectArray methods = (jobjectArray) m_pEnv->CallObjectMethod(env->GetObjectClass(m_obj), mid);

  mid = NULL;
  for (size_t i=0; i<env->GetArrayLength(methods); i++)
  {
    jobject method = env->GetObjectArrayElement(methods, i);

    if (!mid) {
      mid = env.GetMethodID("java/lang/reflect/Method", "getName", "()Ljava/lang/String;");
    }

    std::string name = env.GetString((jstring) env->CallObjectMethod(method, mid));

    m_methods[name].push_back(env->NewGlobalRef(method));

    env->DeleteLocalRef(method);
  }
}

v8::Handle<v8::Value> CJavaObject::NamedGetter(
  v8::Local<v8::String> prop, const v8::AccessorInfo& info)
{
  CJavaObject& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  v8::String::Utf8Value name(prop);

  {
    fields_t::const_iterator it = obj.m_fields.find(*name);

    if (it != obj.m_fields.end()) {
      jmethodID mid = env.GetMethodID(env->GetObjectClass(it->second), "get", "(Ljava/lang/Object;)Ljava/lang/Object;");      
      return env.Close(env.Wrap(env->CallObjectMethod(it->second, mid, obj.m_obj)));
    }
  }

  {
    methods_t::const_iterator it = obj.m_methods.find(*name);

    if (it != obj.m_methods.end()) {
      return env.Close(CJavaFunction::Wrap(obj.m_pEnv, it->second[0]));
    }
  }

  return __base__::NamedGetter(prop, info);
}
v8::Handle<v8::Value> CJavaObject::NamedSetter(
  v8::Local<v8::String> prop, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{
  CJavaObject& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  v8::String::Utf8Value name(prop);


  return __base__::NamedSetter(prop, value, info);
}
v8::Handle<v8::Integer> CJavaObject::NamedQuery(
  v8::Local<v8::String> prop, const v8::AccessorInfo& info)
{
  CJavaObject& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  v8::String::Utf8Value name(prop);

  fields_t::const_iterator it = obj.m_fields.find(*name);

  return __base__::NamedQuery(prop, info);
}
v8::Handle<v8::Boolean> CJavaObject::NamedDeleter(
  v8::Local<v8::String> prop, const v8::AccessorInfo& info)
{
  CJavaObject& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  v8::String::Utf8Value name(prop);


  return __base__::NamedDeleter(prop, info);
}
v8::Handle<v8::Array> CJavaObject::NamedEnumerator(const v8::AccessorInfo& info)
{
  CJavaObject& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  
  return __base__::NamedEnumerator(info);
}
/*
v8::Handle<v8::Value> CJavaContext::NamedGetter(
  v8::Local<v8::String> prop, const v8::AccessorInfo& info)
{
  CJavaContext& ctxt = Unwrap(info.Holder());

  jclass cls = ctxt.m_pEnv->GetObjectClass(ctxt.m_obj);

  
}
v8::Handle<v8::Value> CJavaContext::NamedSetter(
  v8::Local<v8::String> prop, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{

}
v8::Handle<v8::Integer> CJavaContext::NamedQuery(
  v8::Local<v8::String> prop, const v8::AccessorInfo& info)
{

}
v8::Handle<v8::Boolean> CJavaContext::NamedDeleter(
  v8::Local<v8::String> prop, const v8::AccessorInfo& info)
{

}
v8::Handle<v8::Array> CJavaContext::NamedEnumerator(const v8::AccessorInfo& info)
{

}
*/
} // namespace jni