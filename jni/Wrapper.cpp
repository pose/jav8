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

  {
    fields_t::const_iterator it = obj.m_fields.find(*name);

    if (it != obj.m_fields.end()) {
      jmethodID mid = env.GetMethodID(env->GetObjectClass(it->second), "set", "(Ljava/lang/Object;Ljava/lang/Object;)V");     

      env->CallVoidMethod(it->second, mid, obj.m_obj, env.Wrap(value));

      return value;
    }
  }

  return __base__::NamedSetter(prop, value, info);
}
v8::Handle<v8::Integer> CJavaObject::NamedQuery(
  v8::Local<v8::String> prop, const v8::AccessorInfo& info)
{
  CJavaObject& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  v8::String::Utf8Value name(prop);

  {
    fields_t::const_iterator it = obj.m_fields.find(*name);

    if (it != obj.m_fields.end()) {
      return env.Close(v8::Integer::New(v8::None));
    }
  }

  {
    methods_t::const_iterator it = obj.m_methods.find(*name);

    if (it != obj.m_methods.end()) {
      return env.Close(v8::Integer::New(v8::None));
    }
  }

  return __base__::NamedQuery(prop, info);
}
v8::Handle<v8::Array> CJavaObject::NamedEnumerator(const v8::AccessorInfo& info)
{
  CJavaObject& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  v8::Handle<v8::Array> result = v8::Array::New(obj.m_fields.size() + obj.m_methods.size());
  uint32_t idx = 0;
  
  for (fields_t::const_iterator it = obj.m_fields.begin(); it != obj.m_fields.end(); it++)
  {
    result->Set(idx++, v8::String::New(it->first.c_str(), it->first.size()));
  }
  
  for (methods_t::const_iterator it = obj.m_methods.begin(); it != obj.m_methods.end(); it++)
  {
    result->Set(idx++, v8::String::New(it->first.c_str(), it->first.size()));
  }

  return env.Close(result);
}


v8::Handle<v8::Value> CJavaArray::NamedGetter(
  v8::Local<v8::String> prop, const v8::AccessorInfo& info)
{
  CJavaArray& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  v8::String::Utf8Value name(prop);

  if (strcmp("length", *name) == 0) {
    return env.Close(v8::Uint32::New(obj.GetLength()));
  }

  return __base__::NamedGetter(prop, info);
}
v8::Handle<v8::Integer> CJavaArray::NamedQuery(
  v8::Local<v8::String> prop, const v8::AccessorInfo& info)
{
  CJavaArray& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  v8::String::Utf8Value name(prop);

  if (strcmp("length", *name) == 0) {
    return env.Close(v8::Integer::New(v8::None));
  }

  return __base__::NamedQuery(prop, info);
}
v8::Handle<v8::Array> CJavaArray::NamedEnumerator(const v8::AccessorInfo& info)
{
  CJavaArray& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  v8::Handle<v8::Array> result = v8::Array::New(1);

  result->Set(v8::Uint32::New(0), v8::String::NewSymbol("length"));

  return env.Close(result);
}

v8::Handle<v8::Value> CJavaArray::IndexedGetter(
  uint32_t index, const v8::AccessorInfo& info)
{
  CJavaArray& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  jclass clazz = env.FindClass("java/lang/reflect/Array");
  jmethodID mid = env.GetStaticMethodID(clazz, "get", "(Ljava/lang/Object;I)Ljava/lang/Object;");

  return env.Close(env.Wrap(env->CallStaticObjectMethod(clazz, mid, obj.m_obj, index)));
}
v8::Handle<v8::Value> CJavaArray::IndexedSetter(
  uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{
  CJavaArray& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  jclass clazz = env.FindClass("java/lang/reflect/Array");
  jmethodID mid = env.GetStaticMethodID(clazz, "set", "(Ljava/lang/Object;ILjava/lang/Object;)V");

  env->CallStaticVoidMethod(clazz, mid, obj.m_obj, index, env.Wrap(value));

  return env.Close(value);
}
v8::Handle<v8::Integer> CJavaArray::IndexedQuery(
  uint32_t index, const v8::AccessorInfo& info)
{
  CJavaArray& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  if (index < obj.GetLength()) {
    return env.Close(v8::Integer::New(v8::None));
  }

  return __base__::IndexedQuery(index, info);
}
v8::Handle<v8::Array> CJavaArray::IndexedEnumerator(const v8::AccessorInfo& info)
{
  CJavaArray& obj = Unwrap(info.Holder());

  jni::V8Env env(obj.m_pEnv);

  size_t length = obj.GetLength();
  v8::Handle<v8::Array> result = v8::Array::New(length);

  for (size_t i=0; i<length; i++)
  {
    result->Set(v8::Uint32::New(i), v8::Int32::New(i)->ToString());
  }

  return env.Close(result);
}
v8::Handle<v8::Value> CJavaFunction::Wrap(JNIEnv *pEnv, jobject obj)
{
  jni::V8Env env(pEnv);

  v8::Handle<v8::FunctionTemplate> func_tmpl = v8::FunctionTemplate::New();    

  func_tmpl->SetCallHandler(Caller, v8::External::New(new CJavaFunction(pEnv, obj)));

  return env.Close(func_tmpl->GetFunction());
}
v8::Handle<v8::Value> CJavaFunction::Caller(const v8::Arguments& args) 
{
  CJavaFunction& func = *static_cast<CJavaFunction *>(v8::Handle<v8::External>::Cast(args.Data())->Value());

  jni::V8Env env(func.GetEnv());

  bool hasThiz = CManagedObject::IsWrapped(args.This()->ToObject());
  jobject thiz = hasThiz ? CManagedObject::Unwrap(args.This()->ToObject()).GetObject() : NULL;
  
  jobjectArray params = (jobjectArray) env->NewObjectArray(args.Length(), env.FindClass("java/lang/Object"), NULL);

  for (size_t i=0; i<args.Length(); i++)
  {
    env->SetObjectArrayElement(params, i, env.Wrap(args[i]));
  }
  
  jmethodID mid = env.GetMethodID(env->GetObjectClass(func.GetObject()), "invoke", "(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");
  
  jobject result = env->CallObjectMethod(func.GetObject(), mid, thiz, params);

  return env.Close(env.Wrap(result));  
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