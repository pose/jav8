#include "Utils.h"

#include <cassert>
#include <sstream>

#include <time.h>
#include <stdlib.h>

namespace jni {

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

void Env::Throw(const char *type, const char *msg)
{
  m_env->ExceptionDescribe();
  m_env->ExceptionClear();

  jclass exc = FindClass(type);

  if (exc)
  {
    m_env->ThrowNew(exc, msg);
    m_env->DeleteLocalRef(exc);
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
  { "RangeError",     "java/lang/IndexOutOfBoundsException" },
  { "ReferenceError", "java/lang/NullPointerException" },
  { "SyntaxError",    "java/util/IllegalFormatException" },
  { "TypeError",      "java/lang/NoSuchMethodException" }
};

bool Env::ThrowIf(const v8::TryCatch& try_catch)
{
  if (try_catch.HasCaught())
  {
    v8::HandleScope handle_scope;

    const char *type = "java/lang/RuntimeException";
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
          if (strnicmp(SupportErrors[i].name, *s, s.length()) == 0)
          {
            type = SupportErrors[i].type;
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
  assert(v8::Context::InContext());

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

jobjectArray Env::NewObjectArray(const char *name, size_t size, jobject init)
{
  jclass clazz = FindClass(name);
  return m_env->NewObjectArray(size, clazz, init);
}

jobject Env::NewBoolean(jboolean value)
{
  jclass clazz = FindClass("java/lang/Boolean");
  jmethodID cid = GetMethodID(clazz, "<init>", "(Z)V");
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);
  
  return result;
}

jobject Env::NewInt(jint value)
{
  jclass clazz = FindClass("java/lang/Integer");
  jmethodID cid = GetMethodID(clazz, "<init>", "(I)V");
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

jobject Env::NewLong(jlong value)
{
  jclass clazz = FindClass("java/lang/Long");
  jmethodID cid = GetMethodID(clazz, "<init>", "(J)V");
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

jobject Env::NewDouble(jdouble value)
{
  jclass clazz = FindClass("java/lang/Double");
  jmethodID cid = GetMethodID(clazz, "<init>", "(D)V");
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

jstring Env::NewString(v8::Handle<v8::String> str)
{
  return m_env->NewStringUTF(*v8::String::Utf8Value(str));
}

jobject Env::NewDate(v8::Handle<v8::Date> date)
{
  jclass clazz = FindClass("java/util/Date");
  jmethodID cid = GetMethodID(clazz, "<init>", "(J)V");
  jlong value = floor(date->NumberValue() / 1000);
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

jobject Env::NewV8Object(v8::Handle<v8::Object> obj)
{
  jclass clazz = FindClass("lu/flier/script/V8Object");
  jmethodID cid = GetMethodID(clazz, "<init>", "(J)V");
  jlong value = (jlong) *v8::Persistent<v8::Object>::New(obj);
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

jobject Env::NewV8Array(v8::Handle<v8::Array> array)
{
  jclass clazz = FindClass("lu/flier/script/V8Array");
  jmethodID cid = GetMethodID(clazz, "<init>", "(J)V");
  jlong value = (jlong) *v8::Persistent<v8::Array>::New(array);
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

jobject Env::NewV8Function(v8::Handle<v8::Function> func)
{
  jclass clazz = FindClass("lu/flier/script/V8Function");
  jmethodID cid = GetMethodID(clazz, "<init>", "(J)V");
  jlong value = (jlong) *v8::Persistent<v8::Function>::New(func);
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

jobject Env::NewV8Context(v8::Handle<v8::Context> ctxt)
{
  jclass clazz = FindClass("lu/flier/script/V8Context");
  jmethodID cid = GetMethodID(clazz, "<init>", "(J)V");
  jlong value = (jlong) *v8::Persistent<v8::Context>::New(ctxt);
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

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
  if (value->IsNumber()) return NewDouble(value->NumberValue());
  if (value->IsDate()) return NewDate(v8::Handle<v8::Date>::Cast(value));
  if (value->IsArray()) return NewV8Array(v8::Handle<v8::Array>::Cast(value->ToObject()));
  if (value->IsFunction()) return NewV8Function(v8::Handle<v8::Function>::Cast(value->ToObject()));

  return NewV8Object(value->ToObject());
}

v8::Handle<v8::Value> V8Env::Wrap(jobject value)
{
  v8::HandleScope handle_scope;

  v8::Handle<v8::Value> result;

  jclass clazz = m_env->GetObjectClass(value);
    
  if (m_env->IsAssignableFrom(clazz, FindClass("java/lang/String")) == JNI_TRUE) 
  {
    jstring str = (jstring) value;
    const char *p = m_env->GetStringUTFChars(str, NULL);

    result = v8::String::New(p, m_env->GetStringUTFLength(str));

    m_env->ReleaseStringUTFChars(str, p);
  }
  else if (m_env->IsAssignableFrom(clazz, FindClass("java/lang/Integer")) == JNI_TRUE ||
           m_env->IsAssignableFrom(clazz, FindClass("java/lang/Short")) == JNI_TRUE ||
           m_env->IsAssignableFrom(clazz, FindClass("java/lang/Byte")) == JNI_TRUE) 
  {
    jmethodID mid = GetMethodID(clazz, "intValue", "()I");
    result = v8::Integer::New(m_env->CallIntMethod(value, mid));
  }
  else if (m_env->IsAssignableFrom(clazz, FindClass("java/lang/Double")) == JNI_TRUE ||
           m_env->IsAssignableFrom(clazz, FindClass("java/lang/Float")) == JNI_TRUE) 
  {
    jmethodID mid = GetMethodID(clazz, "doubleValue", "()D");
    result = v8::Number::New(m_env->CallDoubleMethod(value, mid));
  }
  else if (m_env->IsAssignableFrom(clazz, FindClass("java/lang/Boolean")) == JNI_TRUE) 
  {
    jmethodID mid = GetMethodID(clazz, "booleanValue", "()Z");
    result = v8::Boolean::New(m_env->CallBooleanMethod(value, mid));
  } else {

  }

  return handle_scope.Close(result);
}

std::vector< v8::Handle<v8::Value> > V8Env::GetArray(jobjectArray array)
{
  v8::HandleScope handle_scope;

  std::vector< v8::Handle<v8::Value> > items(m_env->GetArrayLength(array));

  for (size_t i=0; i<items.size(); i++)
  {
    items[i] = handle_scope.Close(Wrap(m_env->GetObjectArrayElement(array, i)));
  }

  return items;
}

} // namespace jni 
