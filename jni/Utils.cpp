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

void Env::Throw(const char *type, const char *msg)
{
  m_env->ExceptionDescribe();
  m_env->ExceptionClear();

  jclass exc = m_env->FindClass(type);

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

void Env::Throw(v8::TryCatch try_catch)
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
}

const std::string Env::Extract(v8::TryCatch& try_catch)
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

jobject Env::Wrap(v8::Handle<v8::Value> value)
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

  return NewV8Object(value->ToObject());
}

jobject Env::NewBoolean(jboolean value)
{
  jclass clazz = m_env->FindClass("java/lang/Boolean");
  jmethodID cid = m_env->GetMethodID(clazz, "<init>", "(Z)V");
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);
  
  return result;
}

jobject Env::NewInt(jint value)
{
  jclass clazz = m_env->FindClass("java/lang/Integer");
  jmethodID cid = m_env->GetMethodID(clazz, "<init>", "(I)V");
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

jobject Env::NewLong(jlong value)
{
  jclass clazz = m_env->FindClass("java/lang/Long");
  jmethodID cid = m_env->GetMethodID(clazz, "<init>", "(J)V");
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

jobject Env::NewDouble(jdouble value)
{
  jclass clazz = m_env->FindClass("java/lang/Double");
  jmethodID cid = m_env->GetMethodID(clazz, "<init>", "(D)V");
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

jobject Env::NewDate(v8::Handle<v8::Date> date)
{
  jclass clazz = m_env->FindClass("java/util/Date");
  jmethodID cid = m_env->GetMethodID(clazz, "<init>", "(J)V");
  jlong value = floor(date->NumberValue() / 1000);
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

jobject Env::NewV8Object(v8::Handle<v8::Object> obj)
{
  jclass clazz = m_env->FindClass("lu/flier/script/V8Object");
  jmethodID cid = m_env->GetMethodID(clazz, "<init>", "(J)V");
  jlong value = (jlong) *v8::Persistent<v8::Object>::New(obj);
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

jobject Env::NewV8Context(v8::Handle<v8::Context> ctxt)
{
  jclass clazz = m_env->FindClass("lu/flier/script/V8Context");
  jmethodID cid = m_env->GetMethodID(clazz, "<init>", "(J)V");
  jlong value = (jlong) *v8::Persistent<v8::Context>::New(ctxt);
  jobject result = m_env->NewObject(clazz, cid, value);
  m_env->DeleteLocalRef(clazz);

  return result;
}

} // namespace jni 
