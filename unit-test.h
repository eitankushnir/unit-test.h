#ifndef TERMINAL_COLORS_H_
#define TERMINAL_COLORS_H_

// --- Utility ---

#define RESET "\x1B[0m"

// --- Regular ---
#define BLACK "\x1B[30m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"

// --- Bold ---
#define BOLD_BLACK "\x1B[1;30m"
#define BOLD_RED "\x1B[1;31m"
#define BOLD_GREEN "\x1B[1;32m"
#define BOLD_YELLOW "\x1B[1;33m"
#define BOLD_BLUE "\x1B[1;34m"
#define BOLD_MAGENTA "\x1B[1;35m"
#define BOLD_CYAN "\x1B[1;36m"
#define BOLD_WHITE "\x1B[1;37m"

// --- Background ---
#define BG_BLACK "\x1B[40m"
#define BG_RED "\x1B[41m"
#define BG_GREEN "\x1B[42m"
#define BG_YELLOW "\x1B[43m"
#define BG_BLUE "\x1B[44m"
#define BG_MAGENTA "\x1B[45m"
#define BG_CYAN "\x1B[46m"
#define BG_WHITE "\x1B[47m"

#endif // TERMINAL_COLORS_H_

#ifndef UNIT_TEST_H_
#define UNIT_TEST_H_

#include <stddef.h>
#include <stdint.h>

// GLOBAL STATE

typedef void (*_UT_INTERNAL_RUNNABLE)(void);

typedef struct TEST_INFO {
  int assertions_passed;
  int assertions_failed;
  const char *name;
  _UT_INTERNAL_RUNNABLE test_fn;

  _UT_INTERNAL_RUNNABLE setup;
  _UT_INTERNAL_RUNNABLE teardown;

  int exit_code;
  int signal;
  int timeout;

  struct TEST_INFO *next;
} TEST_INFO;

typedef struct SUITE {
  TEST_INFO *tests;
  TEST_INFO *tests_tail;

  int tests_passed;
  int tests_failed;
  const char *name;

  struct SUITE *next;
  _UT_INTERNAL_RUNNABLE setup;
  _UT_INTERNAL_RUNNABLE teardown;
} SUITE_INFO;

typedef struct {
  SUITE_INFO *suites;
  SUITE_INFO *tail;

  SUITE_INFO *current_suite;
  TEST_INFO *current_test;
  int *current_assertions_failed;

  int tests_passed;
  int tests_failed;
  int suites_total;
} RUNNER_INFO;

RUNNER_INFO _ut_global_runner = {0};

// ASSERTIONS
#define ASSERT_TRUE(cond)                                                    \
  do {                                                                       \
    if (!_ut_internal_check_condition((cond), #cond, __FILE__, __LINE__, 1)) \
      return;                                                                \
  } while (0)

#define ASSERT_FALSE(cond)                                                             \
  do {                                                                                 \
    if (!_ut_internal_check_condition(!(cond), "!(" #cond ")", __FILE__, __LINE__, 1)) \
      return;                                                                          \
  } while (0)

#define _UT_RUN_OP(type, fmt, cmp, actual, expected, op, rel, fatal, ...)                                           \
  do {                                                                                                              \
    type _ut_a_ = (actual);                                                                                         \
    type _ut_b_ = (expected);                                                                                       \
    int res = cmp(_ut_a_, _ut_b_, ##__VA_ARGS__);                                                                   \
    if (!(res op 0)) {                                                                                              \
      _ut_internal_report_fail_cmp(__FILE__, __LINE__, fatal,                                                       \
                                   "Expected " BOLD_MAGENTA #actual " " RESET "(" BOLD_MAGENTA fmt RESET ") " RESET \
                                   "to be " #rel " " BOLD_CYAN #expected " " RESET "(" BOLD_CYAN fmt RESET ")",     \
                                   _ut_a_, _ut_b_);                                                                 \
      if (fatal)                                                                                                    \
        return;                                                                                                     \
    } else {                                                                                                        \
      _ut_global_runner.current_test->assertions_passed++;                                                          \
    }                                                                                                               \
  } while (0)
#define _UT_ASSERT_OP(type, fmt, cmp, actual, expected, op, rel, ...) _UT_RUN_OP(type, fmt, cmp, actual, expected, op, rel, 1, ##__VA_ARGS__)
#define _UT_EXPECT_OP(type, fmt, cmp, actual, expected, op, rel, ...) _UT_RUN_OP(type, fmt, cmp, actual, expected, op, rel, 0, ##__VA_ARGS__)

#define ASSERT_INT_EQ(actual, expected) _UT_ASSERT_OP(intmax_t, "%jd", _ut_internal_cmp_int, actual, expected, ==, equal to)
#define ASSERT_INT_NE(actual, expected) _UT_ASSERT_OP(intmax_t, "%jd", _ut_internal_cmp_int, actual, expected, !=, not equal to)
#define ASSERT_INT_GT(actual, expected) _UT_ASSERT_OP(intmax_t, "%jd", _ut_internal_cmp_int, actual, expected, >, greater than)
#define ASSERT_INT_LT(actual, expected) _UT_ASSERT_OP(intmax_t, "%jd", _ut_internal_cmp_int, actual, expected, <, lower than)
#define ASSERT_INT_GE(actual, expected) _UT_ASSERT_OP(intmax_t, "%jd", _ut_internal_cmp_int, actual, expected, >=, greater or equal to)
#define ASSERT_INT_LE(actual, expected) _UT_ASSERT_OP(intmax_t, "%jd", _ut_internal_cmp_int, actual, expected, <=, lower or equal to)

#define ASSERT_UINT_EQ(actual, expected) _UT_ASSERT_OP(uintmax_t, "%ju", _ut_internal_cmp_uint, actual, expected, ==, equal to)
#define ASSERT_UINT_NE(actual, expected) _UT_ASSERT_OP(uintmax_t, "%ju", _ut_internal_cmp_uint, actual, expected, !=, not equal to)
#define ASSERT_UINT_GT(actual, expected) _UT_ASSERT_OP(uintmax_t, "%ju", _ut_internal_cmp_uint, actual, expected, >, greater than)
#define ASSERT_UINT_LT(actual, expected) _UT_ASSERT_OP(uintmax_t, "%ju", _ut_internal_cmp_uint, actual, expected, <, lower than)
#define ASSERT_UINT_GE(actual, expected) _UT_ASSERT_OP(uintmax_t, "%ju", _ut_internal_cmp_uint, actual, expected, >=, greater or equal to)
#define ASSERT_UINT_LE(actual, expected) _UT_ASSERT_OP(uintmax_t, "%ju", _ut_internal_cmp_uint, actual, expected, <=, lower or equal to)

#define ASSERT_STR_EQ(actual, expected) _UT_ASSERT_OP(const char *, "%s", _ut_internal_cmp_str, actual, expected, ==, equal to)
#define ASSERT_STR_NE(actual, expected) _UT_ASSERT_OP(const char *, "%s", _ut_internal_cmp_str, actual, expected, !=, not equal to)

#define ASSERT_FLT_EQ_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(float, "%.9g", _ut_internal_cmp_flt, actual, expected, ==, equal to, epsilon)
#define ASSERT_FLT_NE_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(float, "%.9g", _ut_internal_cmp_flt, actual, expected, !=, not equal to, epsilon)
#define ASSERT_FLT_GT_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(float, "%.9g", _ut_internal_cmp_flt, actual, expected, >, greater than, epsilon)
#define ASSERT_FLT_LT_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(float, "%.9g", _ut_internal_cmp_flt, actual, expected, <, lower than, epsilon)
#define ASSERT_FLT_GE_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(float, "%.9g", _ut_internal_cmp_flt, actual, expected, >=, greater or equal to, epsilon)
#define ASSERT_FLT_LE_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(float, "%.9g", _ut_internal_cmp_flt, actual, expected, <=, lower or equal to, epsilon)

#define ASSERT_DBL_EQ_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(double, "%.17g", _ut_internal_cmp_dbl, actual, expected, ==, equal to, epsilon)
#define ASSERT_DBL_NE_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(double, "%.17g", _ut_internal_cmp_dbl, actual, expected, !=, not equal to, epsilon)
#define ASSERT_DBL_GT_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(double, "%.17g", _ut_internal_cmp_dbl, actual, expected, >, greater than, epsilon)
#define ASSERT_DBL_LT_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(double, "%.17g", _ut_internal_cmp_dbl, actual, expected, <, lower than, epsilon)
#define ASSERT_DBL_GE_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(double, "%.17g", _ut_internal_cmp_dbl, actual, expected, >=, greater or equal to, epsilon)
#define ASSERT_DBL_LE_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(double, "%.17g", _ut_internal_cmp_dbl, actual, expected, <=, lower or equal to, epsilon)

#define ASSERT_LDBL_EQ_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(long double, "%.21Lg", _ut_internal_cmp_ldbl, actual, expected, ==, equal to, epsilon)
#define ASSERT_LDBL_NE_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(long double, "%.21Lg", _ut_internal_cmp_ldbl, actual, expected, !=, not equal to, epsilon)
#define ASSERT_LDBL_GT_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(long double, "%.21Lg", _ut_internal_cmp_ldbl, actual, expected, >, greater than, epsilon)
#define ASSERT_LDBL_LT_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(long double, "%.21Lg", _ut_internal_cmp_ldbl, actual, expected, <, lower than, epsilon)
#define ASSERT_LDBL_GE_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(long double, "%.21Lg", _ut_internal_cmp_ldbl, actual, expected, >=, greater or equal to, epsilon)
#define ASSERT_LDBL_LE_EPSILON(actual, expected, epsilon) _UT_ASSERT_OP(long double, "%.21Lg", _ut_internal_cmp_ldbl, actual, expected, <=, lower or equal to, epsilon)

#define ASSERT_FLT_EQ(actual, expected) ASSERT_FLT_EQ_EPSILON(actual, expected, _UT_FLT_EPSILON)
#define ASSERT_FLT_NE(actual, expected) ASSERT_FLT_NE_EPSILON(actual, expected, _UT_FLT_EPSILON)
#define ASSERT_FLT_GT(actual, expected) ASSERT_FLT_GT_EPSILON(actual, expected, _UT_FLT_EPSILON)
#define ASSERT_FLT_LT(actual, expected) ASSERT_FLT_LT_EPSILON(actual, expected, _UT_FLT_EPSILON)
#define ASSERT_FLT_GE(actual, expected) ASSERT_FLT_GE_EPSILON(actual, expected, _UT_FLT_EPSILON)
#define ASSERT_FLT_LE(actual, expected) ASSERT_FLT_LE_EPSILON(actual, expected, _UT_FLT_EPSILON)

#define ASSERT_DBL_EQ(actual, expected) ASSERT_DBL_EQ_EPSILON(actual, expected, _UT_DBL_EPSILON)
#define ASSERT_DBL_NE(actual, expected) ASSERT_DBL_NE_EPSILON(actual, expected, _UT_DBL_EPSILON)
#define ASSERT_DBL_GT(actual, expected) ASSERT_DBL_GT_EPSILON(actual, expected, _UT_DBL_EPSILON)
#define ASSERT_DBL_LT(actual, expected) ASSERT_DBL_LT_EPSILON(actual, expected, _UT_DBL_EPSILON)
#define ASSERT_DBL_GE(actual, expected) ASSERT_DBL_GE_EPSILON(actual, expected, _UT_DBL_EPSILON)
#define ASSERT_DBL_LE(actual, expected) ASSERT_DBL_LE_EPSILON(actual, expected, _UT_DBL_EPSILON)

#define ASSERT_LDBL_EQ(actual, expected) ASSERT_LDBL_EQ_EPSILON(actual, expected, _UT_LDBL_EPSILON)
#define ASSERT_LDBL_NE(actual, expected) ASSERT_LDBL_NE_EPSILON(actual, expected, _UT_LDBL_EPSILON)
#define ASSERT_LDBL_GT(actual, expected) ASSERT_LDBL_GT_EPSILON(actual, expected, _UT_LDBL_EPSILON)
#define ASSERT_LDBL_LT(actual, expected) ASSERT_LDBL_LT_EPSILON(actual, expected, _UT_LDBL_EPSILON)
#define ASSERT_LDBL_GE(actual, expected) ASSERT_LDBL_GE_EPSILON(actual, expected, _UT_LDBL_EPSILON)
#define ASSERT_LDBL_LE(actual, expected) ASSERT_LDBL_LE_EPSILON(actual, expected, _UT_LDBL_EPSILON)

#define _UT_RUN_MEM_OP(actual, expected, size, op, rel, fatal)                                                       \
  do {                                                                                                               \
    const void *_ut_a_ = &(actual);                                                                                  \
    const void *_ut_b_ = &(expected);                                                                                \
    int res = _ut_internal_cmp_mem(_ut_a_, _ut_b_, size);                                                            \
    if (!(res op 0)) {                                                                                               \
      char buf_a[2 * (size) + 1];                                                                                    \
      char buf_b[2 * (size) + 1];                                                                                    \
      _ut_internal_hex_dump(_ut_a_, (size), buf_a, sizeof(buf_a));                                                   \
      _ut_internal_hex_dump(_ut_b_, (size), buf_b, sizeof(buf_b));                                                   \
      _ut_internal_report_fail_cmp(__FILE__, __LINE__, fatal,                                                        \
                                   "Expected " BOLD_MAGENTA #actual " " RESET "(" BOLD_MAGENTA "%s" RESET ") " RESET \
                                   "to be " #rel " " BOLD_CYAN #expected " " RESET "(" BOLD_CYAN "%s" RESET ")",     \
                                   buf_a, buf_b);                                                                    \
      if (fatal)                                                                                                     \
        return;                                                                                                      \
    } else {                                                                                                         \
      _ut_global_runner.current_test->assertions_passed++;                                                           \
    }                                                                                                                \
  } while (0)

#define _UT_ASSERT_MEM_OP(actual, expected, size, op, rel) _UT_RUN_MEM_OP(actual, expected, size, op, rel, 1)
#define _UT_EXPECT_MEM_OP(actual, expected, size, op, rel) _UT_RUN_MEM_OP(actual, expected, size, op, rel, 0)

#define ASSERT_MEM_EQ(actual, expected, size) _UT_ASSERT_MEM_OP(actual, expected, size, ==, equal to)
#define ASSERT_MEM_NE(actual, expected, size) _UT_ASSERT_MEM_OP(actual, expected, size, !=, not equal to)
#define ASSERT_MEM_GT(actual, expected, size) _UT_ASSERT_MEM_OP(actual, expected, size, >, greater than)
#define ASSERT_MEM_LT(actual, expected, size) _UT_ASSERT_MEM_OP(actual, expected, size, <, lower than)
#define ASSERT_MEM_GE(actual, expected, size) _UT_ASSERT_MEM_OP(actual, expected, size, >=, greater or equal to)
#define ASSERT_MEM_LE(actual, expected, size) _UT_ASSERT_MEM_OP(actual, expected, size, <=, lower or equal to)

#ifndef UT_TOSTR_BUFF_SIZE
#define UT_TOSTR_BUFF_SIZE 256
#endif

#define _UT_RUN_CUSTOM_OP(type, cmp, to_str, actual, expected, op, rel, fatal, ...)                                  \
  do {                                                                                                               \
    type _ut_a_ = (actual);                                                                                          \
    type _ut_b_ = (expected);                                                                                        \
    int res = cmp(&_ut_a_, &_ut_b_, ##__VA_ARGS__);                                                                  \
    if (!(res op 0)) {                                                                                               \
      char buf_a[UT_TOSTR_BUFF_SIZE];                                                                                \
      char buf_b[UT_TOSTR_BUFF_SIZE];                                                                                \
      to_str(&a, buf_a, UT_TOSTR_BUFF_SIZE);                                                                         \
      to_str(&b, buf_b, UT_TOSTR_BUFF_SIZE);                                                                         \
                                                                                                                     \
      _ut_internal_report_fail_cmp(__FILE__, __LINE__, fatal,                                                        \
                                   "Expected " BOLD_MAGENTA #actual " " RESET "(" BOLD_MAGENTA "%s" RESET ") " RESET \
                                   "to be " #rel " " BOLD_CYAN #expected " " RESET "(" BOLD_CYAN "%s" RESET ")",     \
                                   buf_a, buf_b);                                                                    \
      if (fatal)                                                                                                     \
        return;                                                                                                      \
    } else {                                                                                                         \
      _ut_global_runner.current_test->assertions_passed++;                                                           \
    }                                                                                                                \
  } while (0)

#define _UT_ASSERT_CUSTOM_OP(type, cmp, to_str, actual, expected, op, rel, ...) _UT_RUN_CUSTOM_OP(type, cmp, to_str, actual, expected, op, rel, 1, ##__VA_ARGS__)
#define _UT_EXPECT_CUSTOM_OP(type, cmp, to_str, actual, expected, op, rel, ...) _UT_RUN_CUSTOM_OP(type, cmp, to_str, actual, expected, op, rel, 0, ##__VA_ARGS__)

#define ASSERT_CUSTOM_EQ(type, cmp, to_str, actual, expected, ...) _UT_ASSERT_CUSTOM_OP(type, cmp, to_str, actual, expected, ==, equal to, ##__VA_ARGS__)
#define ASSERT_CUSTOM_NE(type, cmp, to_str, actual, expected, ...) _UT_ASSERT_CUSTOM_OP(type, cmp, to_str, actual, expected, !=, not equal to, ##__VA_ARGS__)
#define ASSERT_CUSTOM_GT(type, cmp, to_str, actual, expected, ...) _UT_ASSERT_CUSTOM_OP(type, cmp, to_str, actual, expected, >, greater than, ##__VA_ARGS__)
#define ASSERT_CUSTOM_LT(type, cmp, to_str, actual, expected, ...) _UT_ASSERT_CUSTOM_OP(type, cmp, to_str, actual, expected, <, lower than, ##__VA_ARGS__)
#define ASSERT_CUSTOM_GE(type, cmp, to_str, actual, expected, ...) _UT_ASSERT_CUSTOM_OP(type, cmp, to_str, actual, expected, >=, greater or equal to, ##__VA_ARGS__)
#define ASSERT_CUSTOM_LE(type, cmp, to_str, actual, expected, ...) _UT_ASSERT_CUSTOM_OP(type, cmp, to_str, actual, expected, <=, lower or equal to, ##__VA_ARGS__)

// EXPECTS
#define EXPECT_TRUE(cond) \
  _ut_internal_check_condition((cond), #cond, __FILE__, __LINE__, 0)

#define EXPECT_FALSE(cond) \
  _ut_internal_check_condition(!(cond), "!(" #cond ")", __FILE__, __LINE__, 0)

#define EXPECT_INT_EQ(actual, expected) _UT_EXPECT_OP(intmax_t, "%jd", _ut_internal_cmp_int, actual, expected, ==, equal to)
#define EXPECT_INT_NE(actual, expected) _UT_EXPECT_OP(intmax_t, "%jd", _ut_internal_cmp_int, actual, expected, !=, not equal to)
#define EXPECT_INT_GT(actual, expected) _UT_EXPECT_OP(intmax_t, "%jd", _ut_internal_cmp_int, actual, expected, >, greater than)
#define EXPECT_INT_LT(actual, expected) _UT_EXPECT_OP(intmax_t, "%jd", _ut_internal_cmp_int, actual, expected, <, lower than)
#define EXPECT_INT_GE(actual, expected) _UT_EXPECT_OP(intmax_t, "%jd", _ut_internal_cmp_int, actual, expected, >=, greater or equal to)
#define EXPECT_INT_LE(actual, expected) _UT_EXPECT_OP(intmax_t, "%jd", _ut_internal_cmp_int, actual, expected, <=, lower or equal to)

#define EXPECT_UINT_EQ(actual, expected) _UT_EXPECT_OP(uintmax_t, "%ju", _ut_internal_cmp_uint, actual, expected, ==, equal to)
#define EXPECT_UINT_NE(actual, expected) _UT_EXPECT_OP(uintmax_t, "%ju", _ut_internal_cmp_uint, actual, expected, !=, not equal to)
#define EXPECT_UINT_GT(actual, expected) _UT_EXPECT_OP(uintmax_t, "%ju", _ut_internal_cmp_uint, actual, expected, >, greater than)
#define EXPECT_UINT_LT(actual, expected) _UT_EXPECT_OP(uintmax_t, "%ju", _ut_internal_cmp_uint, actual, expected, <, lower than)
#define EXPECT_UINT_GE(actual, expected) _UT_EXPECT_OP(uintmax_t, "%ju", _ut_internal_cmp_uint, actual, expected, >=, greater or equal to)
#define EXPECT_UINT_LE(actual, expected) _UT_EXPECT_OP(uintmax_t, "%ju", _ut_internal_cmp_uint, actual, expected, <=, lower or equal to)

#define EXPECT_STR_EQ(actual, expected) _UT_EXPECT_OP(const char *, "%s", _ut_internal_cmp_str, actual, expected, ==, equal to)
#define EXPECT_STR_NE(actual, expected) _UT_EXPECT_OP(const char *, "%s", _ut_internal_cmp_str, actual, expected, !=, not equal to)

#define EXPECT_FLT_EQ_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(float, "%.9g", _ut_internal_cmp_flt, actual, expected, ==, equal to, epsilon)
#define EXPECT_FLT_NE_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(float, "%.9g", _ut_internal_cmp_flt, actual, expected, !=, not equal to, epsilon)
#define EXPECT_FLT_GT_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(float, "%.9g", _ut_internal_cmp_flt, actual, expected, >, greater than, epsilon)
#define EXPECT_FLT_LT_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(float, "%.9g", _ut_internal_cmp_flt, actual, expected, <, lower than, epsilon)
#define EXPECT_FLT_GE_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(float, "%.9g", _ut_internal_cmp_flt, actual, expected, >=, greater or equal to, epsilon)
#define EXPECT_FLT_LE_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(float, "%.9g", _ut_internal_cmp_flt, actual, expected, <=, lower or equal to, epsilon)

#define EXPECT_DBL_EQ_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(double, "%.17g", _ut_internal_cmp_dbl, actual, expected, ==, equal to, epsilon)
#define EXPECT_DBL_NE_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(double, "%.17g", _ut_internal_cmp_dbl, actual, expected, !=, not equal to, epsilon)
#define EXPECT_DBL_GT_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(double, "%.17g", _ut_internal_cmp_dbl, actual, expected, >, greater than, epsilon)
#define EXPECT_DBL_LT_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(double, "%.17g", _ut_internal_cmp_dbl, actual, expected, <, lower than, epsilon)
#define EXPECT_DBL_GE_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(double, "%.17g", _ut_internal_cmp_dbl, actual, expected, >=, greater or equal to, epsilon)
#define EXPECT_DBL_LE_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(double, "%.17g", _ut_internal_cmp_dbl, actual, expected, <=, lower or equal to, epsilon)

#define EXPECT_LDBL_EQ_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(long double, "%.21Lg", _ut_internal_cmp_ldbl, actual, expected, ==, equal to, epsilon)
#define EXPECT_LDBL_NE_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(long double, "%.21Lg", _ut_internal_cmp_ldbl, actual, expected, !=, not equal to, epsilon)
#define EXPECT_LDBL_GT_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(long double, "%.21Lg", _ut_internal_cmp_ldbl, actual, expected, >, greater than, epsilon)
#define EXPECT_LDBL_LT_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(long double, "%.21Lg", _ut_internal_cmp_ldbl, actual, expected, <, lower than, epsilon)
#define EXPECT_LDBL_GE_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(long double, "%.21Lg", _ut_internal_cmp_ldbl, actual, expected, >=, greater or equal to, epsilon)
#define EXPECT_LDBL_LE_EPSILON(actual, expected, epsilon) _UT_EXPECT_OP(long double, "%.21Lg", _ut_internal_cmp_ldbl, actual, expected, <=, lower or equal to, epsilon)

#define EXPECT_FLT_EQ(actual, expected) EXPECT_FLT_EQ_EPSILON(actual, expected, _UT_FLT_EPSILON)
#define EXPECT_FLT_NE(actual, expected) EXPECT_FLT_NE_EPSILON(actual, expected, _UT_FLT_EPSILON)
#define EXPECT_FLT_GT(actual, expected) EXPECT_FLT_GT_EPSILON(actual, expected, _UT_FLT_EPSILON)
#define EXPECT_FLT_LT(actual, expected) EXPECT_FLT_LT_EPSILON(actual, expected, _UT_FLT_EPSILON)
#define EXPECT_FLT_GE(actual, expected) EXPECT_FLT_GE_EPSILON(actual, expected, _UT_FLT_EPSILON)
#define EXPECT_FLT_LE(actual, expected) EXPECT_FLT_LE_EPSILON(actual, expected, _UT_FLT_EPSILON)

#define EXPECT_DBL_EQ(actual, expected) EXPECT_DBL_EQ_EPSILON(actual, expected, _UT_DBL_EPSILON)
#define EXPECT_DBL_NE(actual, expected) EXPECT_DBL_NE_EPSILON(actual, expected, _UT_DBL_EPSILON)
#define EXPECT_DBL_GT(actual, expected) EXPECT_DBL_GT_EPSILON(actual, expected, _UT_DBL_EPSILON)
#define EXPECT_DBL_LT(actual, expected) EXPECT_DBL_LT_EPSILON(actual, expected, _UT_DBL_EPSILON)
#define EXPECT_DBL_GE(actual, expected) EXPECT_DBL_GE_EPSILON(actual, expected, _UT_DBL_EPSILON)
#define EXPECT_DBL_LE(actual, expected) EXPECT_DBL_LE_EPSILON(actual, expected, _UT_DBL_EPSILON)

#define EXPECT_LDBL_EQ(actual, expected) EXPECT_LDBL_EQ_EPSILON(actual, expected, _UT_LDBL_EPSILON)
#define EXPECT_LDBL_NE(actual, expected) EXPECT_LDBL_NE_EPSILON(actual, expected, _UT_LDBL_EPSILON)
#define EXPECT_LDBL_GT(actual, expected) EXPECT_LDBL_GT_EPSILON(actual, expected, _UT_LDBL_EPSILON)
#define EXPECT_LDBL_LT(actual, expected) EXPECT_LDBL_LT_EPSILON(actual, expected, _UT_LDBL_EPSILON)
#define EXPECT_LDBL_GE(actual, expected) EXPECT_LDBL_GE_EPSILON(actual, expected, _UT_LDBL_EPSILON)
#define EXPECT_LDBL_LE(actual, expected) EXPECT_LDBL_LE_EPSILON(actual, expected, _UT_LDBL_EPSILON)

#define EXPECT_MEM_EQ(actual, expected, size) _UT_EXPECT_MEM_OP(actual, expected, size, ==, equal to)
#define EXPECT_MEM_NE(actual, expected, size) _UT_EXPECT_MEM_OP(actual, expected, size, !=, not equal to)
#define EXPECT_MEM_GT(actual, expected, size) _UT_EXPECT_MEM_OP(actual, expected, size, >, greater than)
#define EXPECT_MEM_LT(actual, expected, size) _UT_EXPECT_MEM_OP(actual, expected, size, <, lower than)
#define EXPECT_MEM_GE(actual, expected, size) _UT_EXPECT_MEM_OP(actual, expected, size, >=, greater or equal to)
#define EXPECT_MEM_LE(actual, expected, size) _UT_EXPECT_MEM_OP(actual, expected, size, <=, lower or equal to)

void _ut_internal_init_suite(SUITE_INFO *s, const char *name);
void _ut_internal_register_suite(SUITE_INFO *s);
void _ut_internal_register_test(SUITE_INFO *s, TEST_INFO *t);

void _ut_internal_run_test(TEST_INFO *test);
void _ut_internal_run_suite(SUITE_INFO *suite);
void _ut_internal_run_all(void);
void _ut_internal_print_summary(void);

int _ut_internal_check_condition(int condition, const char *condition_string, const char *filename, int linenr, int fatal);

// UTILITY
#define _UT_FLT_EPSILON 10e-5
#define _UT_DBL_EPSILON 10e-9
#define _UT_LDBL_EPSILON 10e-12L
static inline float _ut_internal_fabs(float a) {
  return a < 0 ? -a : a;
}
static inline double _ut_internal_dabs(double a) {
  return a < 0 ? -a : a;
}
static inline long double _ut_internal_ldabs(long double a) {
  return a < 0 ? -a : a;
}

void _ut_internal_hex_dump(const void *loc, size_t bytes, char *buf, size_t bufsize);
const char *_ut_internal_signame(int sig);
void _ut_internal_handle_timeout(int sig);

#if defined(__clang__)
#define _UT_IGNORE_OVERRIDE_START  \
  _Pragma("clang diagnostic push") \
      _Pragma("clang diagnostic ignored \"-Winitializer-overrides\"")
#define _UT_IGNORE_OVERRIDE_STOP \
  _Pragma("clang diagnostic pop")

// 2. Detect standard GCC
#elif defined(__GNUC__)
#define _UT_IGNORE_OVERRIDE_START \
  _Pragma("GCC diagnostic push")  \
      _Pragma("GCC diagnostic ignored \"-Woverride-init\"")
#define _UT_IGNORE_OVERRIDE_STOP \
  _Pragma("GCC diagnostic pop")

// 3. Safe fallback for other compilers (MSVC, etc.)
#else
#define _UT_IGNORE_OVERRIDE_START
#define _UT_IGNORE_OVERRIDE_STOP
#endif

// CMPS
int _ut_internal_cmp_int(intmax_t a, intmax_t b);
int _ut_internal_cmp_uint(uintmax_t a, uintmax_t b);
int _ut_internal_cmp_str(const char *a, const char *b);
int _ut_internal_cmp_mem(const void *a, const void *b, size_t size);

int _ut_internal_cmp_flt(float a, float b, float epsilon);
int _ut_internal_cmp_dbl(double a, double b, double epsilon);
int _ut_internal_cmp_ldbl(long double a, long double b, long double epsilon);
int _ut_internal_report_fail_cmp(const char *filename, int linenr, int fatal, const char *fmt, ...);

#define _UT_DEFAULT_TIMEOUT 5

#define RUN_ALL_TESTS()           \
  do {                            \
    _ut_internal_run_all();       \
    _ut_internal_print_summary(); \
  } while (0)

#define RUN_TEST(suite_name, test_name) \
  _ut_internal_run_test(&(test_##suite_name##_##test_name))

#define RUN_SUITE(suite_name) \
  _ut_internal_run_suite(&(suite_##suite_name));

#define PRINT_SUMMARY() \
  _ut_internal_print_summary();

#define SUITE(s_name, ...)                                           \
  SUITE_INFO suite_##s_name = (SUITE_INFO){                          \
      .name = #s_name,                                               \
      __VA_ARGS__};                                                  \
  void suite_##s_name##_register(void) __attribute__((constructor)); \
  void suite_##s_name##_register(void) {                             \
    _ut_internal_register_suite(&(suite_##s_name));                  \
  }

#define TEST(s_name, t_name, ...)                                               \
  void test_##s_name##_##t_name##_fn(void);                                     \
  void test_##s_name##_##t_name##_body(void);                                   \
  void test_##s_name##_##t_name##_register(void) __attribute__((constructor));  \
  SUITE_INFO suite_##s_name;                                                    \
  _UT_IGNORE_OVERRIDE_START                                                     \
  TEST_INFO test_##s_name##_##t_name = (TEST_INFO){                             \
      .name = #t_name,                                                          \
      .test_fn = test_##s_name##_##t_name##_fn,                                 \
      .timeout = _UT_DEFAULT_TIMEOUT,                                           \
      __VA_ARGS__};                                                             \
  void test_##s_name##_##t_name##_fn(void) {                                    \
    if (suite_##s_name.setup)                                                   \
      suite_##s_name.setup();                                                   \
    if (test_##s_name##_##t_name.setup)                                         \
      test_##s_name##_##t_name.setup();                                         \
                                                                                \
    test_##s_name##_##t_name##_body();                                          \
                                                                                \
    if (test_##s_name##_##t_name.teardown)                                      \
      test_##s_name##_##t_name.teardown();                                      \
                                                                                \
    if (suite_##s_name.teardown)                                                \
      suite_##s_name.teardown();                                                \
  }                                                                             \
  void test_##s_name##_##t_name##_register(void) {                              \
    if (!(suite_##s_name).name) {                                               \
      _ut_internal_init_suite(&(suite_##s_name), #s_name);                      \
      _ut_internal_register_suite(&(suite_##s_name));                           \
    }                                                                           \
    _ut_internal_register_test(&(suite_##s_name), &(test_##s_name##_##t_name)); \
  }                                                                             \
  void test_##s_name##_##t_name##_body(void)

#endif // UNIT_TEST_H_

#ifndef UNIT_TEST_IMPLEMENTATION

#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void _ut_internal_init_suite(SUITE_INFO *s, const char *name) {
  *s = (SUITE_INFO){.name = name};
}

void _ut_internal_register_suite(SUITE_INFO *s) {
  if (!_ut_global_runner.suites) {
    _ut_global_runner.suites = s;
    _ut_global_runner.tail = s;
  } else {
    _ut_global_runner.tail->next = s;
    _ut_global_runner.tail = s;
  }
}

void _ut_internal_register_test(SUITE_INFO *s, TEST_INFO *t) {
  if (!s->tests) {
    s->tests = t;
    s->tests_tail = t;
  } else {
    s->tests_tail->next = t;
    s->tests_tail = t;
  }
}

void _ut_internal_run_test(TEST_INFO *test) {
  _ut_global_runner.current_test = test;
  int *shared_assertions_failed = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (shared_assertions_failed == MAP_FAILED) {
    printf(BOLD_YELLOW "[WARN] " RESET "Failed to create memory map for test %s in suite %s: %s\n", test->name, _ut_global_runner.current_suite->name, strerror(errno));
  }
  _ut_global_runner.current_assertions_failed = shared_assertions_failed;

  pid_t pid = fork();
  if (pid < 0) {
    printf(BOLD_YELLOW "[FORK] " RESET "Failed to create fork for test %s in suite %s: %s\n", test->name, _ut_global_runner.current_suite->name, strerror(errno));
    return;
  }
  if (pid == 0) {
    if (test->timeout)
      alarm(test->timeout);
    test->test_fn();
    exit(0);
  } else {
    int status;
    waitpid(pid, &status, 0);
    int assertions_failed = *shared_assertions_failed;

    // SIGNAL
    if (WIFSIGNALED(status)) {
      if (WTERMSIG(status) == test->signal) {
        if (assertions_failed == 0) {
          printf(BOLD_GREEN "[PASS] " RESET "%s\n", test->name);
          _ut_global_runner.tests_passed++;
        } else {
          _ut_global_runner.tests_failed++;
        }
      } else if (test->signal) {
        if (assertions_failed == 0)
          printf(BOLD_RED "[FAIL] " RESET "%s\n", test->name);
        printf("       ↳ " RED "[KILLED] " RESET "Expected %s but received %s\n", _ut_internal_signame(test->signal), _ut_internal_signame(WTERMSIG(status)));
        _ut_global_runner.tests_failed++;
      } else {
        if (assertions_failed == 0)
          printf(BOLD_RED "[TERM] " RESET "%s\n", test->name);
        printf("       ↳ " RED "[KILLED] " RESET "Unexpected termination using %s\n", _ut_internal_signame(WTERMSIG(status)));
        _ut_global_runner.tests_failed++;
      }
    }

    // EXITED
    else if (WIFEXITED(status) != 0) {
      if (WEXITSTATUS(status) == test->exit_code) {
        if (assertions_failed == 0) {
          printf(BOLD_GREEN "[PASS] " RESET "%s\n", test->name);
          _ut_global_runner.tests_passed++;
        } else {
          _ut_global_runner.tests_failed++;
        }
      } else if (test->exit_code) {
        if (assertions_failed > 0)
          printf(BOLD_RED "[FAIL] " RESET "%s\n", test->name);
        printf("       ↳ " RED "[EXITED] " RESET "Expected exit code %d but got %d\n", test->exit_code, WEXITSTATUS(status));
        _ut_global_runner.tests_failed++;
      } else {
        if (assertions_failed > 0)
          printf(BOLD_RED "[EXIT] " RESET "%s\n", test->name);
        printf("       ↳ " RED "[EXITED] " RESET "Early exit using code %d \n", WEXITSTATUS(status));
        _ut_global_runner.tests_failed++;
      }
    } else if (assertions_failed > 0) {
      _ut_global_runner.tests_failed++;
    } else {
      printf(BOLD_GREEN "[PASS] " RESET "%s\n", test->name);
      _ut_global_runner.tests_passed++;
    }
  }

  munmap(shared_assertions_failed, sizeof(int));
}

void _ut_internal_run_suite(SUITE_INFO *suite) {
  _ut_global_runner.current_suite = suite;
  _ut_global_runner.suites_total++;
  printf("====== SUITE: %s ======\n", suite->name);
  TEST_INFO *test = suite->tests;
  while (test) {
    _ut_internal_run_test(test);
    if (test->assertions_failed > 0)
      suite->tests_failed++;
    else
      suite->tests_passed++;

    test = test->next;
  }
}

void _ut_internal_run_all(void) {
  SUITE_INFO *suite = _ut_global_runner.suites;
  while (suite) {
    _ut_internal_run_suite(suite);

    printf("\n");
    suite = suite->next;
  }
}

void _ut_internal_print_summary(void) {
  printf("=======================================\n"
         "               SUMMARY\n"
         "=======================================\n");
  printf("Total Suites: %d\n", _ut_global_runner.suites_total);
  printf("Total Tests: %d\n", _ut_global_runner.tests_failed + _ut_global_runner.tests_passed);
  printf("Passed: %d\n", _ut_global_runner.tests_passed);
  printf("Failed: %d\n", _ut_global_runner.tests_failed);
  printf("\n");
  if (_ut_global_runner.tests_failed > 0)
    printf("STATUS: " BOLD_RED "FAILED\n");
  else
    printf("PASSED: " BOLD_GREEN "PASSED\n");
  printf(RESET "=======================================\n");
}

int _ut_internal_check_condition(int condition, const char *condition_string, const char *filename, int linenr, int fatal) {
  if (!condition) {
    if (_ut_global_runner.current_test && _ut_global_runner.current_test->assertions_failed == 0)
      printf(BOLD_RED "[FAIL] " RESET "%s\n", _ut_global_runner.current_test->name);

    const char *color = fatal ? RED : YELLOW;
    const char *tag = fatal ? "[ASSERT]" : "[EXPECT]";
    printf("%s       " RESET "↳ %s%s " RESET "%s:%d | %s\n", color, color, tag, filename, linenr, condition_string);

    _ut_global_runner.current_test->assertions_failed++;
    (*(_ut_global_runner.current_assertions_failed))++;
    return 0;
  }

  _ut_global_runner.current_test->assertions_passed++;
  return 1;
}

int _ut_internal_report_fail_cmp(const char *filename, int linenr, int fatal, const char *fmt, ...) {
  if (_ut_global_runner.current_test && _ut_global_runner.current_test->assertions_failed == 0)
    printf(BOLD_RED "[FAIL] " RESET "%s\n", _ut_global_runner.current_test->name);

  const char *color = fatal ? RED : YELLOW;
  const char *tag = fatal ? "[ASSERT]" : "[EXPECT]";
  printf("%s       " RESET "↳ %s%s " RESET "%s:%d | ", color, color, tag, filename, linenr);

  va_list vargs;
  va_start(vargs, fmt);
  vprintf(fmt, vargs);
  va_end(vargs);

  printf("\n");
  (*(_ut_global_runner.current_assertions_failed))++;

  return 0;
}

int _ut_internal_cmp_int(intmax_t a, intmax_t b) {
  return (a > b) - (a < b);
}

int _ut_internal_cmp_uint(uintmax_t a, uintmax_t b) {
  return (a > b) - (a < b);
}

int _ut_internal_cmp_str(const char *a, const char *b) {
  if (a == b)
    return 0;

  if (a == NULL)
    return -1;

  if (b == NULL)
    return 1;

  return strcmp(a, b);
}

int _ut_internal_cmp_mem(const void *a, const void *b, size_t size) {
  if (a == b)
    return 0;
  if (a == NULL)
    return -1;
  if (b == NULL)
    return 1;

  return memcmp(a, b, size);
}
int _ut_internal_cmp_flt(float a, float b, float epsilon) {
  if (a == b)
    return 0;

  if (_ut_internal_fabs(a - b) < epsilon) {
    return 0;
  }

  return (a > b) - (a < b);
}

int _ut_internal_cmp_dbl(double a, double b, double epsilon) {
  if (a == b)
    return 0;

  if (_ut_internal_dabs(a - b) < epsilon) {
    return 0;
  }

  return (a > b) - (a < b);
}
int _ut_internal_cmp_ldbl(long double a, long double b, long double epsilon) {
  if (a == b)
    return 0;

  if (_ut_internal_ldabs(a - b) < epsilon) {
    return 0;
  }

  return (a > b) - (a < b);
}

#define _UT_SIGCASE(sig) \
  case sig:              \
    return #sig;

const char *_ut_internal_signame(int sig) {
  switch (sig) {
    _UT_SIGCASE(SIGHUP)
    _UT_SIGCASE(SIGINT)
    _UT_SIGCASE(SIGQUIT)
    _UT_SIGCASE(SIGILL)
    _UT_SIGCASE(SIGABRT)
    _UT_SIGCASE(SIGFPE)
    _UT_SIGCASE(SIGKILL)
    _UT_SIGCASE(SIGSEGV)
    _UT_SIGCASE(SIGPIPE)
  case SIGALRM:
    return "SIGALRM (timed out)";
    _UT_SIGCASE(SIGTERM)
    _UT_SIGCASE(SIGUSR1)
    _UT_SIGCASE(SIGUSR2)
    _UT_SIGCASE(SIGSTOP)
    _UT_SIGCASE(SIGBUS)
    _UT_SIGCASE(SIGTRAP)
  default:
    return "UNKNOWN SIGNAL";
  }
}

void _ut_internal_hex_dump(const void *loc, size_t bytes, char *buf, size_t bufsize) {
  if (!bufsize || !buf)
    return;

  if (!loc) {
    buf[0] = '\0';
    return;
  }

  const char hex_chars[] = "0123456789abcdef";
  const char *ptr = loc;
  size_t write_idx = 0;

  for (size_t i = 0; i < bytes; i++) {
    if (write_idx + 2 >= bufsize) {
      break;
    }

    unsigned char current_byte = ptr[i];
    buf[write_idx++] = hex_chars[(current_byte >> 4) & 0x0F];
    buf[write_idx++] = hex_chars[current_byte & 0x0F];
  }

  buf[write_idx] = '\0';
}
#endif // UNIT_TEST_IMPLEMENTATION
