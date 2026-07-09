#ifndef CLOC_TEST_ASSERT_H
#define CLOC_TEST_ASSERT_H

#include <stdio.h>

static inline int ct_expect_true(int condition, const char *expr, const char *file, int line) {
    if (!condition) {
        fprintf(stderr, "%s:%d: EXPECT_TRUE failed: %s\n", file, line, expr);
        return 0;
    }

    return 1;
}

static inline int ct_expect_int_eq(int actual, int expected, const char *actual_expr,
                                   const char *expected_expr, const char *file, int line) {
    if (actual != expected) {
        fprintf(stderr, "%s:%d: EXPECT_INT_EQ failed: %s != %s\n", file, line, actual_expr,
                expected_expr);
        fprintf(stderr, "actual=%d expected=%d\n", actual, expected);
        return 0;
    }

    return 1;
}

static inline int ct_expect_ull_eq(unsigned long long actual, unsigned long long expected,
                                   const char *actual_expr, const char *expected_expr,
                                   const char *file, int line) {
    if (actual != expected) {
        fprintf(stderr, "%s:%d: EXPECT_ULL_EQ failed: %s != %s\n", file, line, actual_expr,
                expected_expr);
        fprintf(stderr, "actual=%llu expected=%llu\n", actual, expected);
        return 0;
    }

    return 1;
}

#define CLOC_EXPECT_TRUE(e) ct_expect_true((e), #e, __FILE__, __LINE__)
#define CLOC_EXPECT_INT_EQ(a, e) ct_expect_int_eq((a), (e), #a, #e, __FILE__, __LINE__)
#define CLOC_EXPECT_ULL_EQ(a, e) ct_expect_ull_eq((a), (e), #a, #e, __FILE__, __LINE__)

#endif /* CLOC_TEST_ASSERT_H */
