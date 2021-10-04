/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */
#pragma once

#if defined(__PRETTY_FUNCTION__)
#define _MF_PRETTY_FUNCTION __PRETTY_FUNCTION__
#else
#define _MF_PRETTY_FUNCTION __func__
#endif

#if defined(NDEBUG) || defined(_NDEBUG)

#define MF_ASSERT(cond) (void)0

#else

// C++ Standard Library
#include <cstdlib>

#define MF_ASSERT(cond)                                                                                                \
  if (!(cond))                                                                                                         \
  {                                                                                                                    \
    std::printf(                                                                                                       \
      "\n\n*** ASSERTION FAILURE ***\n\n"                                                                              \
      "cond: %s"                                                                                                       \
      "file: %s"                                                                                                       \
      "line: %d"                                                                                                       \
      "func: %s",                                                                                                      \
      #cond,                                                                                                           \
      __FILE__,                                                                                                        \
      __LINE__,                                                                                                        \
      _MF_PRETTY_FUNCTION);                                                                                            \
    std::abort();                                                                                                      \
  }

#endif