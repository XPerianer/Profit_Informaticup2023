#pragma once

#include <stdexcept>

// NOLINTBEGIN(cppcoreguidelines-macro-usage): Need to use macros for source code location

#define FAIL_MESSAGE(msg)                                              \
  ("ERROR at " __FILE__ ":" + std::to_string(__LINE__) + " " + (msg) + \
   "\nExecution failed. Check logs for more details.")

#define FAIL(msg) throw std::runtime_error(FAIL_MESSAGE(msg))

#define ASSERT(expr, msg)                      \
  if (!static_cast<bool>(expr)) [[unlikely]] { \
    FAIL((msg));                               \
  }                                            \
  static_assert(true, "End call of macro with a semicolon")

#ifdef NDEBUG
#define DEBUG_ASSERT(expr, msg) static_assert(true, "End call of macro with a semicolon")
#else
#define DEBUG_ASSERT(expr, msg) ASSERT((expr), (msg))
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)
