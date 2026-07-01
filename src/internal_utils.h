#ifndef CLC_INTERNAL_UTILS_H
#define CLC_INTERNAL_UTILS_H

#include <codelinecalculator/compiler.h>

#include <stddef.h>
#include <string.h>

#define CLC_ARRAY_COUNT(values) (sizeof(values) / sizeof((values)[0]))

static inline int clc_path_is_separator(char value) {
#if CODELINECALCULATOR_PLATFORM_WINDOWS
    return value == '\\' || value == '/';
#else
    return value == '/';
#endif
}

static inline int clc_ascii_is_alpha(char value) {
    return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z');
}

static inline int clc_ascii_is_digit(char value) {
    return value >= '0' && value <= '9';
}

static inline char clc_ascii_to_lower(char value) {
    if (value >= 'A' && value <= 'Z') {
        return (char)(value - 'A' + 'a');
    }

    return value;
}

static inline int clc_ascii_equal_ignore_case(char left, char right) {
    return clc_ascii_to_lower(left) == clc_ascii_to_lower(right);
}

static inline int clc_ascii_equal_ignore_case_n(const char *left, const char *right,
                                                size_t length) {
    size_t index = 0U;

    for (index = 0U; index < length; ++index) {
        if (!clc_ascii_equal_ignore_case(left[index], right[index])) {
            return 0;
        }
    }

    return 1;
}

static inline int clc_string_equal_ignore_case(const char *left, const char *right) {
    size_t index = 0U;

    if (!left || !right) {
        return 0;
    }

    while (left[index] != '\0' && right[index] != '\0') {
        if (!clc_ascii_equal_ignore_case(left[index], right[index])) {
            return 0;
        }
        ++index;
    }

    return left[index] == '\0' && right[index] == '\0';
}

static inline int clc_string_starts_with(const char *value, const char *prefix) {
    const size_t prefix_length = strlen(prefix);
    return strncmp(value, prefix, prefix_length) == 0;
}

#endif /* CLC_INTERNAL_UTILS_H */
