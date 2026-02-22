#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(DOCRAFT_BUILD_SHARED_LIB)
#define DOCRAFT_LIB __declspec(dllexport)
#elif defined(DOCRAFT_USE_SHARED_LIB)
#define DOCRAFT_LIB __declspec(dllimport)
#else
#define DOCRAFT_LIB
#endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#if defined(DOCRAFT_BUILD_SHARED_LIB)
#define DOCRAFT_LIB __attribute__((visibility("default")))
#else
#define DOCRAFT_LIB
#endif
#else
#define DOCRAFT_LIB
#endif
