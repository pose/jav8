#include "Utils.h"

#include <cassert>
#include <sstream>
#include <algorithm>
#include <functional>

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "Config.h"

#ifdef USE_INTERNAL_V8_API
  #undef COMPILER
  #include <src/v8.h>
#endif

#include "Wrapper.h"

namespace jni {

#ifdef _MSC_VER
  Cache::caches_t *Cache::s_caches = NULL;
#else
# ifdef __APPLE__
  pthread_key_t Cache::s_caches_key = NULL;
# else
  __thread Cache::caches_t *Cache::s_caches = NULL;
# endif
#endif

Cache& Cache::GetInstance(JNIEnv *env)
{  
#ifdef __APPLE__
  if (!s_caches_key)
  {
    pthread_key_create(&s_caches_key, NULL); 
  }

  caches_t *s_caches = (caches_t *) pthread_getspecific(s_caches_key);

  if (!s_caches) {
    s_caches = new caches_t();
    pthread_setspecific(s_caches_key, new caches_t());
  }
#else
  if (!s_caches) s_caches = new caches_t();
#endif

  caches_t::const_iterator it = s_caches->find(env);

  if (it != s_caches->end()) return *it->second;

  Cache *cache = new Cache(env);

  s_caches->insert(std::make_pair(env, cache));
  
  return *cache;
}

void Cache::Clear(void)
{
  for (classes_t::const_iterator it=m_classes.begin(); it!=m_classes.end(); it++)
  {
    m_env->DeleteGlobalRef(it->second);
  }

  for (members_t::const_iterator it=m_members.begin(); it!=m_members.end(); it++)
  {
    const fields_t& fields = it->second.first;
    const methods_t& methods = it->second.second;

    for (fields_t::const_iterator itField = fields.begin(); itField != fields.end(); itField++)
    {
      m_env->DeleteGlobalRef(itField->second);
    }
    
    for (methods_t::const_iterator itMethod = methods.begin(); itMethod != methods.end(); itMethod++)
    {
      CJavaFunction::ReleaseMethods(m_env, itMethod->second);
    }
  }

  m_classes.clear();  
  m_assignables.clear();
  m_fieldIDs.clear();
  m_methodIDs.clear();
  m_members.clear();
}

jclass Cache::FindClass(const char *name)
{
  classes_t::const_iterator it = m_classes.find(name);

  jclass clazz;

  if (it != m_classes.end()) 
  {
    clazz = it->second;
  }
  else
  {
    clazz = m_env->FindClass(name);

    if (clazz) 
    {
      clazz = (jclass) m_env->NewGlobalRef(clazz);

      m_classes[name] = clazz;
    }
  }

  return clazz;
}

bool Cache::IsAssignableFrom(jclass sub, jclass sup)
{
  assignables_t::key_type key = std::make_pair(sub, sup);
  assignables_t::const_iterator iter = m_assignables.find(key);

  if (iter != m_assignables.end()) return iter->second;
  
  bool assignable = m_env->IsAssignableFrom(sub, sup) == JNI_TRUE;

  m_assignables[key] = assignable;

  return assignable;
}

jfieldID Cache::InternalGetFieldID(jclass clazz, bool statiz, const char * name, const char *sig)
{
  fieldIDsByClass_t::key_type clazz_key = std::make_pair(clazz, statiz);
  fieldIDsByClass_t::iterator clazz_iter = m_fieldIDs.find(clazz_key);

  if (clazz_iter == m_fieldIDs.end())
  {
    clazz_iter = m_fieldIDs.insert(std::make_pair(clazz_key, fieldIDs_t())).first;
  }

  fieldIDs_t& fieldIDs = clazz_iter->second;
  fieldIDs_t::key_type field_key = std::make_pair(name, sig);
  fieldIDs_t::iterator field_iter = fieldIDs.find(field_key);

  jfieldID fid = NULL;

  if (field_iter == fieldIDs.end())
  {
    fid = statiz ? m_env->GetStaticFieldID(clazz, name, sig) :
                   m_env->GetFieldID(clazz, name, sig);

    fieldIDs[field_key] = fid;
  }
  else
  {
    fid = field_iter->second;
  }

  return fid;
}
jmethodID Cache::InternalGetMethodID(jclass clazz, bool statiz, const char * name, const char *sig)
{
  methodIDsByClass_t::key_type clazz_key = std::make_pair(clazz, statiz);
  methodIDsByClass_t::iterator clazz_iter = m_methodIDs.find(clazz_key);

  if (clazz_iter == m_methodIDs.end())
  {
    clazz_iter = m_methodIDs.insert(std::make_pair(clazz_key, methodIDs_t())).first;
  }

  methodIDs_t& methodIDs = clazz_iter->second;
  methodIDs_t::key_type method_key = std::make_pair(name, sig);
  methodIDs_t::iterator method_iter = methodIDs.find(method_key);

  jmethodID mid = NULL;

  if (method_iter == methodIDs.end())
  {
    mid = statiz ? m_env->GetStaticMethodID(clazz, name, sig) :
                   m_env->GetMethodID(clazz, name, sig);

    methodIDs[method_key] = mid;
  }
  else
  {
    mid = method_iter->second;
  }

  return mid;
}

void Cache::FillBuildIns(void)
{
  buildins.java.lang.Class                = FindClass("java/lang/Class");
  buildins.java.lang.Boolean              = FindClass("java/lang/Boolean");
  buildins.java.lang.Number               = FindClass("java/lang/Number");
  buildins.java.lang.Byte                 = FindClass("java/lang/Byte");
  buildins.java.lang.Short                = FindClass("java/lang/Short");
  buildins.java.lang.Integer              = FindClass("java/lang/Integer");
  buildins.java.lang.Long                 = FindClass("java/lang/Long");
  buildins.java.lang.Float                = FindClass("java/lang/Float");
  buildins.java.lang.Double               = FindClass("java/lang/Double");
  buildins.java.lang.String               = FindClass("java/lang/String");
  buildins.java.lang.RuntimeException     = FindClass("java/lang/RuntimeException");

  buildins.java.lang.reflect.Field        = FindClass("java/lang/reflect/Field");
  buildins.java.lang.reflect.Method       = FindClass("java/lang/reflect/Method");
  buildins.java.lang.reflect.Array        = FindClass("java/lang/reflect/Array");

  buildins.java.util.Date                 = FindClass("java/util/Date");

  buildins.lu.flier.script.V8Object       = FindClass("lu/flier/script/V8Object");
  buildins.lu.flier.script.V8Array        = FindClass("lu/flier/script/V8Array");
  buildins.lu.flier.script.V8Function     = FindClass("lu/flier/script/V8Function");
  buildins.lu.flier.script.V8Context      = FindClass("lu/flier/script/V8Context");
}

Cache::members_t::iterator Cache::CacheMembers(jclass clazz)
{
  members_t::iterator iter;
  
  for (iter = m_members.begin(); iter != m_members.end(); iter++)
  {
    if (m_env->IsSameObject(clazz, iter->first)) break;
  }

  if (iter == m_members.end())
  {
    iter = m_members.insert(std::make_pair((jclass) m_env->NewGlobalRef(clazz), std::make_pair(fields_t(), methods_t()))).first;

    V8Env env(m_env);

    static jmethodID midGetFields = env.GetMethodID(buildins.java.lang.Class, "getFields", "()[Ljava/lang/reflect/Field;");
    jobjectArray fields = (jobjectArray) env->CallObjectMethod(clazz, midGetFields);

    static jmethodID midGetFieldName = env.GetMethodID(buildins.java.lang.reflect.Field, "getName", "()Ljava/lang/String;");
    for (size_t i=0; i<env->GetArrayLength(fields); i++)
    {
      jobject field = env->GetObjectArrayElement(fields, i);

      std::string name = env.GetString((jstring) env->CallObjectMethod(field, midGetFieldName));

      iter->second.first[name] = env->NewGlobalRef(field);

      env->DeleteLocalRef(field);
    }

    static jmethodID midGetMethods = env.GetMethodID(buildins.java.lang.Class, "getMethods", "()[Ljava/lang/reflect/Method;");
    jobjectArray methods = (jobjectArray) env->CallObjectMethod(clazz, midGetMethods);

    static jmethodID midGetMethodName = env.GetMethodID(buildins.java.lang.reflect.Method, "getName", "()Ljava/lang/String;");

    for (size_t i=0; i<env->GetArrayLength(methods); i++)
    {
      jobject method = env->GetObjectArrayElement(methods, i);

      std::string name = env.GetString((jstring) env->CallObjectMethod(method, midGetMethodName));

      const types_t& types = CJavaFunction::GetParameterTypes(env, method);

      iter->second.second[name].push_back(std::make_pair(env->NewGlobalRef(method), types));

      env->DeleteLocalRef(method);
    }
  }

  return iter;
}

v8::Handle<v8::Value> Cache::GetMember(jobject obj, const std::string& name)
{
  jni::V8Env env(m_env);

  members_t::iterator iter = CacheMembers(env->GetObjectClass(obj));

  if (iter != m_members.end())
  {
    fields_t& fields = iter->second.first;
    {
      fields_t::const_iterator it = fields.find(name);

      if (it != fields.end()) {
        static jmethodID mid = env.GetMethodID(buildins.java.lang.reflect.Field, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");    

        return env.Close(env.Wrap(env->CallObjectMethod(it->second, mid, obj)));
      }
    }

    methods_t& methods = iter->second.second;
    {
      methods_t::const_iterator it = methods.find(name);

      if (it != methods.end()) {
        return env.Close(CJavaFunction::Wrap(m_env, it->second));
      }
    }
  }

  return v8::Handle<v8::Value>();
}
v8::Handle<v8::Value> Cache::SetMember(jobject obj, const std::string& name, v8::Handle<v8::Value> value)
{
  jni::V8Env env(m_env);

  members_t::iterator iter = CacheMembers(env->GetObjectClass(obj));

  if (iter != m_members.end())
  {
    fields_t& fields = iter->second.first;
    {
      fields_t::const_iterator it = fields.find(name);

      if (it != fields.end()) {
        static jmethodID mid = env.GetMethodID(buildins.java.lang.reflect.Field, "set", "(Ljava/lang/Object;Ljava/lang/Object;)V");     

        env->CallVoidMethod(it->second, mid, obj, env.Wrap(value));

        return value;
      }
    }
  }

  return v8::Handle<v8::Value>();
}
bool Cache::HasMember(jobject obj, const std::string& name)
{
  jni::V8Env env(m_env);

  members_t::iterator iter = CacheMembers(env->GetObjectClass(obj));

  if (iter != m_members.end())
  {
    fields_t& fields = iter->second.first;
    {
      fields_t::const_iterator it = fields.find(name);

      if (it != fields.end()) {
        return true;
      }
    }

    methods_t& methods = iter->second.second;
    {
      methods_t::const_iterator it = methods.find(name);

      if (it != methods.end()) {
        return true;
      }
    }
  }

  return false;
}
v8::Handle<v8::Array> Cache::GetMembers(jobject obj)
{
  jni::V8Env env(m_env);

  members_t::iterator iter = CacheMembers(env->GetObjectClass(obj));

  if (iter != m_members.end())
  {
    fields_t& fields = iter->second.first;
    methods_t& methods = iter->second.second;

    v8::Handle<v8::Array> result = v8::Array::New(fields.size() + methods.size());
    uint32_t idx = 0;

    for (fields_t::const_iterator it = fields.begin(); it != fields.end(); it++)
    {
      result->Set(idx++, v8::String::New(it->first.c_str(), it->first.size()));
    }

    for (methods_t::const_iterator it = methods.begin(); it != methods.end(); it++)
    {
      result->Set(idx++, v8::String::New(it->first.c_str(), it->first.size()));
    }

    return env.Close(result);
  }

  return v8::Handle<v8::Array>();
}

const std::string Env::GetString(jstring str)
{
  jboolean iscopy;

  const char *s = m_env->GetStringUTFChars(str, &iscopy);
  jsize len = m_env->GetStringUTFLength(str);
  
  const std::string value(s, len);

  m_env->ReleaseStringUTFChars(str, s);

  return value;
}

jobject Env::GetField(jobject obj, const char *name, const char *sig)
{
  jclass clazz = m_env->GetObjectClass(obj);
  jfieldID fid = GetFieldID(clazz, name, sig);

  return fid ? m_env->GetObjectField(obj, fid) : NULL;
}

jlong Env::GetLongField(jobject obj, const char *name)
{
  jclass clazz = m_env->GetObjectClass(obj);
  jfieldID fid = GetFieldID(clazz, name, "J");

  return m_env->GetLongField(obj, fid);
}

jobject Env::GetStaticField(jobject obj, const char *name, const char *sig)
{
  jclass clazz = m_env->GetObjectClass(obj);
  jfieldID fid = GetStaticFieldID(clazz, name, sig);

  return fid ? m_env->GetStaticObjectField(clazz, fid) : NULL;
}

void Env::Throw(jclass type, const char *msg)
{
  m_env->ExceptionDescribe();
  m_env->ExceptionClear();
  m_env->ThrowNew(type, msg);

}
void Env::Throw(const char *type, const char *msg)
{
  jclass exc = FindClass(type);

  if (exc)
  {
    Throw(exc, msg);
  }
  else
  {    
    Throw("java/lang/NoClassDefFoundError", type);
  }
}

static struct {
  const char *name;
  const char *type;
} SupportErrors[] = {
  { "RangeError",     "lu/flier/script/RangeError" },
  { "ReferenceError", "lu/flier/script/ReferenceError" },
  { "SyntaxError",    "lu/flier/script/SyntaxError" },
  { "TypeError",      "lu/flier/script/TypeError" }
};

bool Env::ThrowIf(const v8::TryCatch& try_catch)
{
  if (try_catch.HasCaught() && !try_catch.Exception().IsEmpty())
  {
    v8::HandleScope handle_scope;

    jclass type = buildins.java.lang.RuntimeException;
    v8::Handle<v8::Value> obj = try_catch.Exception();

    if (obj->IsObject())
    {
      v8::Handle<v8::Object> exc = obj->ToObject();
      v8::Handle<v8::String> name = v8::String::New("name");

      if (exc->Has(name))
      {
        v8::String::Utf8Value s(v8::Handle<v8::String>::Cast(exc->Get(name)));

        for (size_t i=0; i<_countof(SupportErrors); i++)
        {
          if (strncmp(SupportErrors[i].name, *s, s.length()) == 0)
          {
            type = FindClass(SupportErrors[i].type);
            break;
          }
        }        
      }      
    }

    const std::string msg = Extract(try_catch);

    Env::Throw(type, msg.c_str());
  }

  return try_catch.HasCaught();
}

const std::string Env::Extract(const v8::TryCatch& try_catch)
{
  v8::HandleScope handle_scope;

  std::ostringstream oss;

  v8::String::Utf8Value msg(try_catch.Exception());

  if (*msg)
    oss << std::string(*msg, msg.length());

  v8::Handle<v8::Message> message = try_catch.Message();

  if (!message.IsEmpty())
  {
    oss << " ( ";

    if (!message->GetScriptResourceName().IsEmpty() &&
        !message->GetScriptResourceName()->IsUndefined())
    {
      v8::String::Utf8Value name(message->GetScriptResourceName());

      oss << std::string(*name, name.length());
    }

    oss << " @ " << message->GetLineNumber() << " : " << message->GetStartColumn() << " ) ";
    
    if (!message->GetSourceLine().IsEmpty() &&
        !message->GetSourceLine()->IsUndefined())
    {
      v8::String::Utf8Value line(message->GetSourceLine());

      oss << " -> " << std::string(*line, line.length());
    }
  }

  return oss.str();
}

jobject Env::NewObject(const char *name, const char *sig, ...)
{
  jclass clazz = FindClass(name);
  jmethodID cid = GetMethodID(clazz, "<init>", sig);
  va_list args;

  return m_env->NewObjectV(clazz, cid, args);
}

jobjectArray Env::NewObjectArray(size_t size, const char *name, jobject init)
{
  jclass clazz = FindClass(name);
  return m_env->NewObjectArray(size, clazz, init);
}

jobject Env::NewBoolean(jboolean value)
{
  static jmethodID cid = GetMethodID(buildins.java.lang.Boolean, "<init>", "(Z)V");
  
  return m_env->NewObject(buildins.java.lang.Boolean, cid, value);  
}

jobject Env::NewInt(jint value)
{
  static jmethodID cid = GetMethodID(buildins.java.lang.Integer, "<init>", "(I)V");
  
  return m_env->NewObject(buildins.java.lang.Integer, cid, value);  
}

jobject Env::NewLong(jlong value)
{
  static jmethodID cid = GetMethodID(buildins.java.lang.Long, "<init>", "(J)V");
  
  return m_env->NewObject(buildins.java.lang.Long, cid, value);
}

jobject Env::NewDouble(jdouble value)
{
  static jmethodID cid = GetMethodID(buildins.java.lang.Double, "<init>", "(D)V");
  
  return m_env->NewObject(buildins.java.lang.Double, cid, value);  
}

jstring Env::NewString(v8::Handle<v8::String> str)
{
  return m_env->NewStringUTF(*v8::String::Utf8Value(str));
}

jobject Env::NewDate(v8::Handle<v8::Date> date)
{
  static jmethodID cid = GetMethodID(buildins.java.util.Date, "<init>", "(J)V");

  jlong value = floor(date->NumberValue());
  return m_env->NewObject(buildins.java.util.Date, cid, value);  
}

jobject Env::NewV8Object(v8::Handle<v8::Object> obj)
{
  static jmethodID cid = GetMethodID(buildins.lu.flier.script.V8Object, "<init>", "(J)V");

  jlong value = (jlong) *v8::Persistent<v8::Object>::New(obj);
  return m_env->NewObject(buildins.lu.flier.script.V8Object, cid, value);  
}

jobject Env::NewV8Array(v8::Handle<v8::Array> array)
{
  static jmethodID cid = GetMethodID(buildins.lu.flier.script.V8Array, "<init>", "(J)V");

  jlong value = (jlong) *v8::Persistent<v8::Array>::New(array);
  return m_env->NewObject(buildins.lu.flier.script.V8Array, cid, value);
}

jobject Env::NewV8Function(v8::Handle<v8::Function> func)
{
  static jmethodID cid = GetMethodID(buildins.lu.flier.script.V8Function, "<init>", "(J)V");

  jlong value = (jlong) *v8::Persistent<v8::Function>::New(func);
  return m_env->NewObject(buildins.lu.flier.script.V8Function, cid, value);  
}

jobject Env::NewV8Context(v8::Handle<v8::Context> ctxt)
{
  static jmethodID cid = GetMethodID(buildins.lu.flier.script.V8Context, "<init>", "(J)V");

  jlong value = (jlong) *v8::Persistent<v8::Context>::New(ctxt);
  return m_env->NewObject(buildins.lu.flier.script.V8Context, cid, value);  
}

#ifdef V8_SUPPORT_ISOLATE

void V8Isolate::ensureInIsolate() {
  if (v8::Isolate::GetCurrent() == NULL) 
  {
      v8::Isolate* isolate = v8::Isolate::New();
      isolate->Enter();
      assert(isolate == v8::Isolate::GetCurrent());

    #ifdef USE_INTERNAL_V8_API
      v8::internal::Isolate::Current()->InitializeLoggingAndCounters();
    #endif
  }
}

bool V8Isolate::IsAlive() {
  return v8::Isolate::GetCurrent() != NULL && !v8::V8::IsExecutionTerminating(v8::Isolate::GetCurrent()) && !v8::V8::IsDead();
}

#else

void V8Isolate::ensureInIsolate() { }

bool V8Isolate::IsAlive() { return !v8::V8::IsExecutionTerminating() && !v8::V8::IsDead(); }

#endif

jobject V8Env::Wrap(v8::Handle<v8::Value> value)
{
  assert(v8::Context::InContext());

  v8::HandleScope handle_scope;

  if (value.IsEmpty() || value->IsNull() || value->IsUndefined()) return NULL;
  if (value->IsTrue()) return NewBoolean(JNI_TRUE);
  if (value->IsFalse()) return NewBoolean(JNI_FALSE);

  if (value->IsInt32()) return NewInt(value->Int32Value());  
  if (value->IsUint32()) return NewLong(value->IntegerValue());
  if (value->IsString())
  {
    v8::String::Utf8Value str(v8::Handle<v8::String>::Cast(value));

    return m_env->NewStringUTF(*str);
  }
  if (value->IsDate()) return NewDate(v8::Handle<v8::Date>::Cast(value));
  if (value->IsNumber()) return NewDouble(value->NumberValue());

  return Wrap(value->ToObject());
}

jobject V8Env::Wrap(v8::Handle<v8::Object> obj)
{
  assert(v8::Context::InContext());

  v8::HandleScope handle_scope;

  if (obj->IsArray())
  {
  #ifdef USE_NATIVE_ARRAY
    return WrapArrayToNative(obj);
  #else
    return NewV8Array(v8::Handle<v8::Array>::Cast(obj));
  #endif
  }
  if (obj->IsFunction()) return NewV8Function(v8::Handle<v8::Function>::Cast(obj));
  if (CManagedObject::IsWrapped(obj)) return CManagedObject::Unwrap(obj).GetObject();

  return NewV8Object(obj);
}

jobjectArray V8Env::WrapArrayToNative(v8::Handle<v8::Value> obj)
{
  assert(v8::Context::InContext());
  assert(obj->IsArray());

  v8::HandleScope handle_scope;

  v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(obj);

  jobjectArray items = NewObjectArray(array->Length());

  for (size_t i=0; i<array->Length(); i++)
  {      
    jobject item;
    v8::Handle<v8::Value> obj = array->Get(i);

    if (obj->IsArray()) 
    {
      item = WrapArrayToNative(obj);
    } else 
    {
      item = Wrap(obj);
    }

    m_env->SetObjectArrayElement(items, i, item);
    m_env->DeleteLocalRef(item);
  }

  return items;
}

v8::Handle<v8::Value> V8Env::Wrap(jobject value)
{
  v8::HandleScope handle_scope;
  v8::TryCatch try_catch;

  if (value == NULL) return handle_scope.Close(v8::Null());

  v8::Handle<v8::Value> result;

  jclass clazz = m_env->GetObjectClass(value);

  if (IsAssignableFrom(clazz, buildins.java.lang.String)) 
  {
    jstring str = (jstring) value;
    const char *p = m_env->GetStringUTFChars(str, NULL);

    result = v8::String::New(p, m_env->GetStringUTFLength(str));

    m_env->ReleaseStringUTFChars(str, p);
  }
  else if (IsAssignableFrom(clazz, buildins.java.lang.Long) ||
           IsAssignableFrom(clazz, buildins.java.lang.Integer) ||
           IsAssignableFrom(clazz, buildins.java.lang.Short) ||
           IsAssignableFrom(clazz, buildins.java.lang.Byte)) 
  {
    static jmethodID mid = GetMethodID(buildins.java.lang.Number, "intValue", "()I");

    result = v8::Integer::New(m_env->CallIntMethod(value, mid));
  }
  else if (IsAssignableFrom(clazz, buildins.java.lang.Double) ||
           IsAssignableFrom(clazz, buildins.java.lang.Float)) 
  {
    static jmethodID mid = GetMethodID(buildins.java.lang.Number, "doubleValue", "()D");

    result = v8::Number::New(m_env->CallDoubleMethod(value, mid));
  }
  else if (IsAssignableFrom(clazz, buildins.java.lang.Boolean)) 
  {
    static jmethodID mid = GetMethodID(buildins.java.lang.Boolean, "booleanValue", "()Z");

    result = v8::Boolean::New(m_env->CallBooleanMethod(value, mid));
  }      
  else if (IsAssignableFrom(clazz, buildins.java.util.Date))
  {
    static jmethodID mid = GetMethodID(buildins.java.util.Date, "getTime", "()J");

    result = v8::Date::New(m_env->CallLongMethod(value, mid));
  }
  else if (IsAssignableFrom(clazz, buildins.java.lang.reflect.Method)) 
  {
    result = CJavaFunction::Wrap(m_env, value);  
  } 
  else if (IsAssignableFrom(clazz, buildins.lu.flier.script.V8Context)) 
  {
    result = CJavaContext::Wrap(m_env, value);  
  } 
  else if (IsAssignableFrom(clazz, buildins.lu.flier.script.V8Array)) 
  {
    jclass clazz = m_env->GetObjectClass(value);
    jfieldID fid = GetFieldID(clazz, "obj", "J");
    result = v8::Handle<v8::Array>((v8::Array *) m_env->GetLongField(value, fid));
  } 
  else if (IsAssignableFrom(clazz, buildins.lu.flier.script.V8Object)) 
  {
    jclass clazz = m_env->GetObjectClass(value);
    jfieldID fid = GetFieldID(clazz, "obj", "J");
    result = v8::Handle<v8::Object>((v8::Object *) m_env->GetLongField(value, fid));
  }
  else 
  { 
    static jmethodID mid = GetMethodID(buildins.java.lang.Class, "isArray", "()Z");

    if (m_env->CallBooleanMethod(m_env->GetObjectClass(value), mid)) {
    #ifdef USE_NATIVE_ARRAY
      size_t len = m_env->GetArrayLength((jarray) value);
      v8::Handle<v8::Array> items = v8::Array::New(len);

      for (size_t i=0; i<len; i++)
      {
        jobject item = m_env->GetObjectArrayElement((jobjectArray) value, i);

        items->Set(i, Wrap(item));
      }

      result = items;
    #else
      result = CJavaArray::Wrap(m_env, value);
    #endif
    } else {
      result = CJavaObject::Wrap(m_env, value);
    }
  }

  return ThrowIf(try_catch) ? v8::Handle<v8::Value>() : handle_scope.Close(result);
}

std::vector< v8::Handle<v8::Value> > V8Env::GetArray(jobjectArray array)
{
  std::vector< v8::Handle<v8::Value> > items(m_env->GetArrayLength(array));

  for (size_t i=0; i<items.size(); i++)
  {
    items[i] = Wrap(m_env->GetObjectArrayElement(array, i));
  }

  return items;
}

} // namespace jni 
