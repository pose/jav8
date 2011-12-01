#pragma once

// Convert the array content when wrap it
//#define USE_NATIVE_ARRAY 1

#pragma warning( disable : 4100 ) // The formal parameter is not referenced in the body of the function. The unreferenced parameter is ignored.
#pragma warning( disable : 4503 ) // The decorated name was longer than the compiler limit (4096), and was truncated. 

#ifndef V8_TARGET_ARCH_ARM
  #define USE_INTERNAL_V8_API 1
  #define V8_SUPPORT_ISOLATE  1
#endif