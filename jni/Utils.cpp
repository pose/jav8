#include "Utils.h"

namespace jni {

const std::string Env::GetString(jstring str)
{
  jboolean iscopy;

  const char *s = m_env->GetStringUTFChars(str, &iscopy);

  const std::string value = value;

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

} // namespace jni 