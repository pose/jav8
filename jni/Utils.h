#pragma once

#include <string>

#include <jni.h>

namespace jni {

class Env
{
  JNIEnv *m_env;
public:
  Env(JNIEnv *env) : m_env(env) {}

  const std::string GetString(jstring str);

  void Throw(const char *type, const char *msg);
};

} // namespace jni