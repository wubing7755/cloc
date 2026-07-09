#ifndef CLOC_LINE_COUNTER_H
#define CLOC_LINE_COUNTER_H

#include <cloc/export.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ClocStatus {
    CLOC_STATUS_OK = 0,
    CLOC_STATUS_INVALID_ARGUMENT = 1,
    CLOC_STATUS_OUT_OF_MEMORY = 2,
    CLOC_STATUS_IO_ERROR = 3,
    CLOC_STATUS_OVERFLOW = 4
} ClocStatus;

/*
 * Scan options for a source-line count.
 *
 * root_path must point at the project directory to scan. Suffix entries may be
 * written as "c", ".c", or "*.c"; matching is case-insensitive.
 */
typedef struct ClocScanOptions {
    const char *root_path;
    const char *const *suffixes;
    size_t suffix_count;
} ClocScanOptions;

/*
 * Physical-line count result.
 *
 * total_lines includes blank and comment lines. Empty files contribute zero
 * lines; a non-empty final line without a trailing newline is counted.
 */
typedef struct ClocScanResult {
    unsigned long long total_lines;
    unsigned long long matched_files;
    unsigned long long visited_files;
    unsigned long long skipped_directories;
    unsigned long long failed_entries;
} ClocScanResult;

/*
 * Recursively count source lines under options->root_path.
 *
 * Common VCS, dependency, and build-output directories are skipped. Unreadable
 * non-root entries increment failed_entries and scanning continues.
 */
CLOC_API ClocStatus cloc_count_source_lines(
    const ClocScanOptions *options, ClocScanResult *result);

/* Return a stable English label for a status value. */
CLOC_API const char *cloc_status_name(ClocStatus status);

#ifdef __cplusplus
}
#endif

#endif /* CLOC_LINE_COUNTER_H */
