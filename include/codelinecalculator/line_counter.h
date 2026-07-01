#ifndef CODELINECALCULATOR_LINE_COUNTER_H
#define CODELINECALCULATOR_LINE_COUNTER_H

#include <codelinecalculator/export.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CodeLineCalculatorStatus {
    CODELINECALCULATOR_STATUS_OK = 0,
    CODELINECALCULATOR_STATUS_INVALID_ARGUMENT = 1,
    CODELINECALCULATOR_STATUS_OUT_OF_MEMORY = 2,
    CODELINECALCULATOR_STATUS_IO_ERROR = 3,
    CODELINECALCULATOR_STATUS_OVERFLOW = 4
} CodeLineCalculatorStatus;

/*
 * Scan options for a source-line count.
 *
 * root_path must point at the project directory to scan. Suffix entries may be
 * written as "c", ".c", or "*.c"; matching is case-insensitive.
 */
typedef struct CodeLineCalculatorScanOptions {
    const char *root_path;
    const char *const *suffixes;
    size_t suffix_count;
} CodeLineCalculatorScanOptions;

/*
 * Physical-line count result.
 *
 * total_lines includes blank and comment lines. Empty files contribute zero
 * lines; a non-empty final line without a trailing newline is counted.
 */
typedef struct CodeLineCalculatorScanResult {
    unsigned long long total_lines;
    unsigned long long matched_files;
    unsigned long long visited_files;
    unsigned long long skipped_directories;
    unsigned long long failed_entries;
} CodeLineCalculatorScanResult;

/*
 * Recursively count source lines under options->root_path.
 *
 * Common VCS, dependency, and build-output directories are skipped. Unreadable
 * non-root entries increment failed_entries and scanning continues.
 */
CODELINECALCULATOR_API CodeLineCalculatorStatus codelinecalculator_count_source_lines(
    const CodeLineCalculatorScanOptions *options, CodeLineCalculatorScanResult *result);

/* Return a stable English label for a status value. */
CODELINECALCULATOR_API const char *codelinecalculator_status_name(CodeLineCalculatorStatus status);

#ifdef __cplusplus
}
#endif

#endif /* CODELINECALCULATOR_LINE_COUNTER_H */
