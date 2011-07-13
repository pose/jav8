#pragma once

#include <string>

#include <jni.h>

#include <v8.h>

namespace jni {

class Env
{
  JNIEnv *m_env;

  const std::string Extract(v8::TryCatch& try_catch);
public:
  Env(JNIEnv *env) : m_env(env) {}

  const std::string GetString(jstring str);

  jobject NewBoolean(jboolean value);
  jobject NewInt(jint value);
  jobject NewLong(jlong value);
  jobject NewDouble(jdouble value);
  jobject NewDate(v8::Handle<v8::Date> date);
  jobject NewV8Object(v8::Handle<v8::Object> obj);
  jobject NewV8Context(v8::Handle<v8::Context> ctxt);

  void Throw(const char *type, const char *msg);
  void Throw(v8::TryCatch try_catch);

  jobject Wrap(v8::Handle<v8::Value> value);
};

} // namespace jni
