#ifndef TERMINAL_COLORS_H_
#define TERMINAL_COLORS_H_

// --- Utility ---
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

// GLOBAL STATE

typedef void (*_UT_INTERNAL_RUNNABLE)(void);

typedef struct TEST_INFO {
  int assertions_passed;
  int assertions_failed;
  const char *name;
  _UT_INTERNAL_RUNNABLE test_fn;

  _UT_INTERNAL_RUNNABLE setup;
  _UT_INTERNAL_RUNNABLE teardown;

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

#define _UT_RUN_OP(type, fmt, cmp, actual, expected, op, rel, fatal)                                                \
  do {                                                                                                              \
    type a = (actual);                                                                                              \
    type b = (expected);                                                                                            \
    int res = cmp(a, b);                                                                                            \
    if (!(res op 0)) {                                                                                              \
      _ut_internal_report_fail_cmp(__FILE__, __LINE__, fatal,                                                       \
                                   "Expected " BOLD_MAGENTA #actual " " RESET "(" BOLD_MAGENTA fmt RESET ") " RESET \
                                   "to be " #rel " " BOLD_CYAN #expected " " RESET "(" BOLD_CYAN fmt RESET ")",     \
                                   a, b);                                                                           \
      if (fatal)                                                                                                    \
        return;                                                                                                     \
    } else {                                                                                                        \
      _ut_global_runner.current_test->assertions_passed++;                                                          \
    }                                                                                                               \
  } while (0)
#define _UT_ASSERT_OP(type, fmt, cmp, actual, expected, op, rel) _UT_RUN_OP(type, fmt, cmp, actual, expected, op, rel, 1)
#define _UT_EXPECT_OP(type, fmt, cmp, actual, expected, op, rel) _UT_RUN_OP(type, fmt, cmp, actual, expected, op, rel, 0)

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

void _ut_internal_init_suite(SUITE_INFO *s, const char *name);
void _ut_internal_register_suite(SUITE_INFO *s);
void _ut_internal_register_test(SUITE_INFO *s, TEST_INFO *t);

void _ut_internal_run_test(TEST_INFO *test);
void _ut_internal_run_suite(SUITE_INFO *suite);
void _ut_internal_run_all(void);
void _ut_internal_print_summary(void);

int _ut_internal_same_sign(long long a, long long b);
int _ut_internal_check_condition(int condition, const char *condition_string, const char *filename, int linenr, int fatal);

// CMPS
int _ut_internal_cmp_int(intmax_t a, intmax_t b);
int _ut_internal_cmp_uint(uintmax_t a, uintmax_t b);
int _ut_internal_cmp_str(const char *a, const char *b);
int _ut_internal_report_fail_cmp(const char *filename, int linenr, int fatal, const char *fmt, ...);

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
  TEST_INFO test_##s_name##_##t_name = (TEST_INFO){                             \
      .name = #t_name,                                                          \
      .test_fn = test_##s_name##_##t_name##_fn,                                 \
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
  test->test_fn();
  if (test->assertions_failed > 0) {
    _ut_global_runner.tests_failed++;
  } else {
    printf(BOLD_GREEN "[PASS] " RESET "%s\n", test->name);
    _ut_global_runner.tests_passed++;
  }
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
  _ut_global_runner.current_test->assertions_failed++;

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
#endif // UNIT_TEST_IMPLEMENTATION
