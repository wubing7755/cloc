#include <codelinecalculator/line_counter.h>

#include <codelinecalculator/compiler.h>

#include "../internal_utils.h"

#if CODELINECALCULATOR_PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ScanContext {
    const CodeLineCalculatorScanOptions *options;
    CodeLineCalculatorScanResult *result;
} ScanContext;

static int is_dot_directory(const char *name) {
    return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

static int is_ignored_directory_name(const char *name) {
    static const char *const ignored_names[] = {
        ".git",    ".hg",          ".svn", ".vs", ".idea",  "build", "out",   "dist",
        "install", "node_modules", "bin",  "obj", "target", ".next", ".nuxt", "coverage",
    };
    static const char *const ignored_prefixes[] = {"cmake-build-"};
    size_t index = 0U;

    for (index = 0U; index < CLC_ARRAY_COUNT(ignored_names); ++index) {
        if (strcmp(name, ignored_names[index]) == 0) {
            return 1;
        }
    }

    for (index = 0U; index < CLC_ARRAY_COUNT(ignored_prefixes); ++index) {
        if (clc_string_starts_with(name, ignored_prefixes[index])) {
            return 1;
        }
    }

    return 0;
}

static int add_ull(unsigned long long *target, unsigned long long value) {
    if (*target > ULLONG_MAX - value) {
        return 0;
    }

    *target += value;
    return 1;
}

static char *join_path(const char *directory, const char *name) {
    const size_t directory_length = strlen(directory);
    const size_t name_length = strlen(name);
    const int needs_separator =
        directory_length > 0 && !clc_path_is_separator(directory[directory_length - 1]);
    const size_t separator_length = needs_separator ? 1U : 0U;
    char *path = NULL;

    if (directory_length > SIZE_MAX - separator_length ||
        directory_length + separator_length > SIZE_MAX - name_length ||
        directory_length + separator_length + name_length > SIZE_MAX - 1U) {
        return NULL;
    }

    path = (char *)malloc(directory_length + separator_length + name_length + 1U);
    if (!path) {
        return NULL;
    }

    memcpy(path, directory, directory_length);
    if (needs_separator) {
#if CODELINECALCULATOR_PLATFORM_WINDOWS
        path[directory_length] = '\\';
#else
        path[directory_length] = '/';
#endif
    }
    memcpy(path + directory_length + separator_length, name, name_length);
    path[directory_length + separator_length + name_length] = '\0';
    return path;
}

static const char *normalized_suffix_pattern(const char *suffix) {
    if (suffix[0] == '*' && suffix[1] == '.') {
        return suffix + 1;
    }

    return suffix;
}

static int path_matches_suffix(const char *path, const char *suffix) {
    const size_t path_length = strlen(path);
    const char *normalized_suffix = normalized_suffix_pattern(suffix);
    const size_t suffix_length = strlen(normalized_suffix);

    if (suffix_length == 0U) {
        return 0;
    }

    if (normalized_suffix[0] == '.') {
        if (path_length < suffix_length) {
            return 0;
        }
        return clc_ascii_equal_ignore_case_n(path + path_length - suffix_length, normalized_suffix,
                                             suffix_length);
    }

    if (path_length < suffix_length + 1U) {
        return 0;
    }
    if (path[path_length - suffix_length - 1U] != '.') {
        return 0;
    }

    return clc_ascii_equal_ignore_case_n(path + path_length - suffix_length, normalized_suffix,
                                         suffix_length);
}

static int path_matches_any_suffix(const char *path, const CodeLineCalculatorScanOptions *options) {
    size_t index = 0;
    for (index = 0; index < options->suffix_count; ++index) {
        if (path_matches_suffix(path, options->suffixes[index])) {
            return 1;
        }
    }
    return 0;
}

static CodeLineCalculatorStatus count_file_lines(const char *path, unsigned long long *line_count) {
    FILE *file = fopen(path, "rb");
    int saw_any_byte = 0;
    int last_was_newline = 1;
    int ch = 0;

    *line_count = 0U;

    if (!file) {
        return CODELINECALCULATOR_STATUS_IO_ERROR;
    }

    while ((ch = fgetc(file)) != EOF) {
        saw_any_byte = 1;
        if (ch == '\n') {
            if (!add_ull(line_count, 1U)) {
                (void)fclose(file);
                return CODELINECALCULATOR_STATUS_OVERFLOW;
            }
            last_was_newline = 1;
        } else {
            last_was_newline = 0;
        }
    }

    if (ferror(file)) {
        (void)fclose(file);
        return CODELINECALCULATOR_STATUS_IO_ERROR;
    }

    if (fclose(file) != 0) {
        return CODELINECALCULATOR_STATUS_IO_ERROR;
    }

    if (saw_any_byte && !last_was_newline && !add_ull(line_count, 1U)) {
        return CODELINECALCULATOR_STATUS_OVERFLOW;
    }

    return CODELINECALCULATOR_STATUS_OK;
}

static CodeLineCalculatorStatus visit_file(const char *path, ScanContext *context) {
    unsigned long long file_lines = 0U;
    CodeLineCalculatorStatus status = CODELINECALCULATOR_STATUS_OK;
    CodeLineCalculatorScanResult *result = context->result;

    if (!add_ull(&result->visited_files, 1U)) {
        return CODELINECALCULATOR_STATUS_OVERFLOW;
    }

    if (!path_matches_any_suffix(path, context->options)) {
        return CODELINECALCULATOR_STATUS_OK;
    }

    status = count_file_lines(path, &file_lines);
    if (status == CODELINECALCULATOR_STATUS_IO_ERROR) {
        if (!add_ull(&result->failed_entries, 1U)) {
            return CODELINECALCULATOR_STATUS_OVERFLOW;
        }
        return CODELINECALCULATOR_STATUS_OK;
    }
    if (status != CODELINECALCULATOR_STATUS_OK) {
        return status;
    }

    if (!add_ull(&result->matched_files, 1U) || !add_ull(&result->total_lines, file_lines)) {
        return CODELINECALCULATOR_STATUS_OVERFLOW;
    }

    return CODELINECALCULATOR_STATUS_OK;
}

static CodeLineCalculatorStatus walk_directory(const char *directory, ScanContext *context,
                                               int is_root);

#if CODELINECALCULATOR_PLATFORM_WINDOWS
static CodeLineCalculatorStatus walk_directory(const char *directory, ScanContext *context,
                                               int is_root) {
    WIN32_FIND_DATAA find_data;
    HANDLE find_handle = INVALID_HANDLE_VALUE;
    char *pattern = join_path(directory, "*");
    CodeLineCalculatorStatus status = CODELINECALCULATOR_STATUS_OK;

    if (!pattern) {
        return CODELINECALCULATOR_STATUS_OUT_OF_MEMORY;
    }

    find_handle = FindFirstFileA(pattern, &find_data);
    free(pattern);

    if (find_handle == INVALID_HANDLE_VALUE) {
        if (is_root) {
            return CODELINECALCULATOR_STATUS_IO_ERROR;
        }
        if (!add_ull(&context->result->failed_entries, 1U)) {
            return CODELINECALCULATOR_STATUS_OVERFLOW;
        }
        return CODELINECALCULATOR_STATUS_OK;
    }

    do {
        const char *name = find_data.cFileName;
        char *child_path = NULL;
        const int is_directory =
            (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;

        if (is_dot_directory(name)) {
            continue;
        }

        if (is_directory && is_ignored_directory_name(name)) {
            if (!add_ull(&context->result->skipped_directories, 1U)) {
                status = CODELINECALCULATOR_STATUS_OVERFLOW;
                break;
            }
            continue;
        }

        child_path = join_path(directory, name);
        if (!child_path) {
            status = CODELINECALCULATOR_STATUS_OUT_OF_MEMORY;
            break;
        }

        if (is_directory) {
            if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) ==
                FILE_ATTRIBUTE_REPARSE_POINT) {
                if (!add_ull(&context->result->skipped_directories, 1U)) {
                    status = CODELINECALCULATOR_STATUS_OVERFLOW;
                }
            } else {
                status = walk_directory(child_path, context, 0);
            }
        } else {
            status = visit_file(child_path, context);
        }

        free(child_path);

        if (status != CODELINECALCULATOR_STATUS_OK) {
            break;
        }
    } while (FindNextFileA(find_handle, &find_data) != 0);

    if (status == CODELINECALCULATOR_STATUS_OK && GetLastError() != ERROR_NO_MORE_FILES) {
        if (!add_ull(&context->result->failed_entries, 1U)) {
            status = CODELINECALCULATOR_STATUS_OVERFLOW;
        }
    }

    (void)FindClose(find_handle);
    return status;
}
#else
static CodeLineCalculatorStatus walk_directory(const char *directory, ScanContext *context,
                                               int is_root) {
    DIR *dir = opendir(directory);
    struct dirent *entry = NULL;
    CodeLineCalculatorStatus status = CODELINECALCULATOR_STATUS_OK;

    if (!dir) {
        if (is_root) {
            return CODELINECALCULATOR_STATUS_IO_ERROR;
        }
        if (!add_ull(&context->result->failed_entries, 1U)) {
            return CODELINECALCULATOR_STATUS_OVERFLOW;
        }
        return CODELINECALCULATOR_STATUS_OK;
    }

    while ((entry = readdir(dir)) != NULL) {
        char *child_path = NULL;
        struct stat child_stat;

        if (is_dot_directory(entry->d_name)) {
            continue;
        }

        child_path = join_path(directory, entry->d_name);
        if (!child_path) {
            status = CODELINECALCULATOR_STATUS_OUT_OF_MEMORY;
            break;
        }

        if (lstat(child_path, &child_stat) != 0) {
            if (!add_ull(&context->result->failed_entries, 1U)) {
                status = CODELINECALCULATOR_STATUS_OVERFLOW;
            }
            free(child_path);
            if (status != CODELINECALCULATOR_STATUS_OK) {
                break;
            }
            continue;
        }

        if (S_ISDIR(child_stat.st_mode)) {
            if (is_ignored_directory_name(entry->d_name)) {
                if (!add_ull(&context->result->skipped_directories, 1U)) {
                    status = CODELINECALCULATOR_STATUS_OVERFLOW;
                }
            } else {
                status = walk_directory(child_path, context, 0);
            }
        } else if (S_ISREG(child_stat.st_mode)) {
            status = visit_file(child_path, context);
        }

        free(child_path);

        if (status != CODELINECALCULATOR_STATUS_OK) {
            break;
        }
    }

    if (closedir(dir) != 0 && status == CODELINECALCULATOR_STATUS_OK) {
        if (!add_ull(&context->result->failed_entries, 1U)) {
            status = CODELINECALCULATOR_STATUS_OVERFLOW;
        }
    }

    return status;
}
#endif

static int scan_options_are_valid(const CodeLineCalculatorScanOptions *options) {
    size_t index = 0;

    if (!options || !options->root_path || options->root_path[0] == '\0' || !options->suffixes ||
        options->suffix_count == 0U) {
        return 0;
    }

    for (index = 0; index < options->suffix_count; ++index) {
        if (!options->suffixes[index] || options->suffixes[index][0] == '\0') {
            return 0;
        }
    }

    return 1;
}

CodeLineCalculatorStatus
codelinecalculator_count_source_lines(const CodeLineCalculatorScanOptions *options,
                                      CodeLineCalculatorScanResult *result) {
    ScanContext context;

    if (!result) {
        return CODELINECALCULATOR_STATUS_INVALID_ARGUMENT;
    }

    memset(result, 0, sizeof(*result));

    if (!scan_options_are_valid(options)) {
        return CODELINECALCULATOR_STATUS_INVALID_ARGUMENT;
    }

    context.options = options;
    context.result = result;
    return walk_directory(options->root_path, &context, 1);
}

const char *codelinecalculator_status_name(CodeLineCalculatorStatus status) {
    switch (status) {
    case CODELINECALCULATOR_STATUS_OK:
        return "ok";
    case CODELINECALCULATOR_STATUS_INVALID_ARGUMENT:
        return "invalid argument";
    case CODELINECALCULATOR_STATUS_OUT_OF_MEMORY:
        return "out of memory";
    case CODELINECALCULATOR_STATUS_IO_ERROR:
        return "I/O error";
    case CODELINECALCULATOR_STATUS_OVERFLOW:
        return "numeric overflow";
    default:
        return "unknown status";
    }
}
