#include <codelinecalculator/compiler.h>
#include <codelinecalculator/line_counter.h>

#include "test_assert.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#if CODELINECALCULATOR_PLATFORM_WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

static int create_directory(const char *path) {
#if CODELINECALCULATOR_PLATFORM_WINDOWS
    if (_mkdir(path) == 0) {
#else
    if (mkdir(path, 0777) == 0) {
#endif
        return 1;
    }

    return errno == EEXIST;
}

static int create_new_directory(const char *path) {
#if CODELINECALCULATOR_PLATFORM_WINDOWS
    return _mkdir(path) == 0;
#else
    return mkdir(path, 0777) == 0;
#endif
}

static int make_path(char *buffer, size_t buffer_size, const char *directory, const char *name) {
    const int written = snprintf(buffer, buffer_size, "%s/%s", directory, name);
    return written >= 0 && (size_t)written < buffer_size;
}

static int write_text_file(const char *path, const char *content) {
    FILE *file = fopen(path, "wb");
    if (!file) {
        return 0;
    }

    if (fputs(content, file) == EOF) {
        (void)fclose(file);
        return 0;
    }

    return fclose(file) == 0;
}

static int create_unique_root(char *buffer, size_t buffer_size) {
    int attempt = 0;
    for (attempt = 0; attempt < 100; ++attempt) {
        const int written = snprintf(buffer, buffer_size, "line_counter_test_data_%lu_%d",
                                     (unsigned long)time(NULL), attempt);
        if (written < 0 || (size_t)written >= buffer_size) {
            return 0;
        }
        if (create_new_directory(buffer)) {
            return 1;
        }
        if (errno != EEXIST) {
            return 0;
        }
    }
    return 0;
}

static int create_fixture_tree(char *root, size_t root_size) {
    char path[512];
    char nested[512];
    char build[512];
    char git[512];
    char node_modules[512];
    char bin[512];

    if (!create_unique_root(root, root_size)) {
        return 0;
    }

    if (!make_path(nested, sizeof(nested), root, "nested") || !create_directory(nested)) {
        return 0;
    }
    if (!make_path(build, sizeof(build), root, "build") || !create_directory(build)) {
        return 0;
    }
    if (!make_path(git, sizeof(git), root, ".git") || !create_directory(git)) {
        return 0;
    }
    if (!make_path(node_modules, sizeof(node_modules), root, "node_modules") ||
        !create_directory(node_modules)) {
        return 0;
    }
    if (!make_path(bin, sizeof(bin), root, "bin") || !create_directory(bin)) {
        return 0;
    }

    if (!make_path(path, sizeof(path), root, "main.c") ||
        !write_text_file(path, "int main(void) {\n    return 0;\n}\n")) {
        return 0;
    }
    if (!make_path(path, sizeof(path), root, "util.h") ||
        !write_text_file(path, "#pragma once\nint add(int left, int right);")) {
        return 0;
    }
    if (!make_path(path, sizeof(path), root, "empty.c") || !write_text_file(path, "")) {
        return 0;
    }
    if (!make_path(path, sizeof(path), root, "notes.txt") ||
        !write_text_file(path, "this file should not be counted\n")) {
        return 0;
    }
    if (!make_path(path, sizeof(path), nested, "tool.cpp") ||
        !write_text_file(path, "int tool(void) { return 1; }\n")) {
        return 0;
    }
    if (!make_path(path, sizeof(path), nested, "app.js") ||
        !write_text_file(path, "const answer = 42;\nconsole.log(answer);\n")) {
        return 0;
    }
    if (!make_path(path, sizeof(path), nested, "types.ts") ||
        !write_text_file(path, "export const answer: number = 42;")) {
        return 0;
    }
    if (!make_path(path, sizeof(path), nested, "Program.cs") ||
        !write_text_file(path, "class Program {\n}\n")) {
        return 0;
    }
    if (!make_path(path, sizeof(path), nested, "Counter.RAZOR") ||
        !write_text_file(path, "<h1>Counter</h1>\n@code {\n}\n")) {
        return 0;
    }
    if (!make_path(path, sizeof(path), nested, "site.css") ||
        !write_text_file(path, "body {\n    color: #000;\n}\n")) {
        return 0;
    }
    if (!make_path(path, sizeof(path), build, "generated.c") ||
        !write_text_file(path, "generated\ncode\nignored\n")) {
        return 0;
    }
    if (!make_path(path, sizeof(path), git, "internal.h") || !write_text_file(path, "ignored\n")) {
        return 0;
    }
    if (!make_path(path, sizeof(path), node_modules, "dependency.js") ||
        !write_text_file(path, "dependency\ncode\nignored\n")) {
        return 0;
    }
    if (!make_path(path, sizeof(path), bin, "generated.cs") ||
        !write_text_file(path, "generated\ncode\nignored\n")) {
        return 0;
    }

    return 1;
}

static int test_invalid_arguments(void) {
    CodeLineCalculatorScanResult result;
    const char *suffixes[] = {".c"};
    CodeLineCalculatorScanOptions options = {"", suffixes, 1U};

    if (!CODELINECALCULATOR_EXPECT_INT_EQ((int)codelinecalculator_count_source_lines(NULL, &result),
                                          (int)CODELINECALCULATOR_STATUS_INVALID_ARGUMENT)) {
        return 1;
    }

    if (!CODELINECALCULATOR_EXPECT_INT_EQ(
            (int)codelinecalculator_count_source_lines(&options, &result),
            (int)CODELINECALCULATOR_STATUS_INVALID_ARGUMENT)) {
        return 1;
    }

    if (!CODELINECALCULATOR_EXPECT_INT_EQ(
            (int)codelinecalculator_count_source_lines(&options, NULL),
            (int)CODELINECALCULATOR_STATUS_INVALID_ARGUMENT)) {
        return 1;
    }

    return 0;
}

static int test_counts_matching_suffixes(void) {
    char root[256];
    const char *suffixes[] = {".c", "h"};
    CodeLineCalculatorScanOptions options;
    CodeLineCalculatorScanResult result;

    if (!CODELINECALCULATOR_EXPECT_TRUE(create_fixture_tree(root, sizeof(root)))) {
        return 1;
    }

    options.root_path = root;
    options.suffixes = suffixes;
    options.suffix_count = 2U;

    if (!CODELINECALCULATOR_EXPECT_INT_EQ(
            (int)codelinecalculator_count_source_lines(&options, &result),
            (int)CODELINECALCULATOR_STATUS_OK)) {
        return 1;
    }

    if (!CODELINECALCULATOR_EXPECT_ULL_EQ(result.total_lines, 5U)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_ULL_EQ(result.matched_files, 3U)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_ULL_EQ(result.failed_entries, 0U)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_ULL_EQ(result.skipped_directories, 4U)) {
        return 1;
    }

    return 0;
}

static int test_counts_user_selected_suffix(void) {
    char root[256];
    const char *suffixes[] = {"cpp"};
    CodeLineCalculatorScanOptions options;
    CodeLineCalculatorScanResult result;

    if (!CODELINECALCULATOR_EXPECT_TRUE(create_fixture_tree(root, sizeof(root)))) {
        return 1;
    }

    options.root_path = root;
    options.suffixes = suffixes;
    options.suffix_count = 1U;

    if (!CODELINECALCULATOR_EXPECT_INT_EQ(
            (int)codelinecalculator_count_source_lines(&options, &result),
            (int)CODELINECALCULATOR_STATUS_OK)) {
        return 1;
    }

    if (!CODELINECALCULATOR_EXPECT_ULL_EQ(result.total_lines, 1U)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_ULL_EQ(result.matched_files, 1U)) {
        return 1;
    }

    return 0;
}

static int test_counts_wildcard_common_suffixes(void) {
    char root[256];
    const char *suffixes[] = {"*.js", "*.ts", "*.cs", "*.razor", "*.css"};
    CodeLineCalculatorScanOptions options;
    CodeLineCalculatorScanResult result;

    if (!CODELINECALCULATOR_EXPECT_TRUE(create_fixture_tree(root, sizeof(root)))) {
        return 1;
    }

    options.root_path = root;
    options.suffixes = suffixes;
    options.suffix_count = 5U;

    if (!CODELINECALCULATOR_EXPECT_INT_EQ(
            (int)codelinecalculator_count_source_lines(&options, &result),
            (int)CODELINECALCULATOR_STATUS_OK)) {
        return 1;
    }

    if (!CODELINECALCULATOR_EXPECT_ULL_EQ(result.total_lines, 11U)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_ULL_EQ(result.matched_files, 5U)) {
        return 1;
    }

    return 0;
}

int main(void) {
    if (test_invalid_arguments()) {
        return 1;
    }
    if (test_counts_matching_suffixes()) {
        return 1;
    }
    if (test_counts_user_selected_suffix()) {
        return 1;
    }
    if (test_counts_wildcard_common_suffixes()) {
        return 1;
    }

    printf("[PASS] line counter tests\n");
    return 0;
}
