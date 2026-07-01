#ifndef CLC_CLI_OPTIONS_H
#define CLC_CLI_OPTIONS_H

#include <stddef.h>
#include <stdio.h>

enum { CLI_INPUT_SIZE = 4096, CLI_MAX_CUSTOM_SUFFIXES = 64 };

typedef enum CliParseResult {
    CLI_PARSE_RUN = 0,
    CLI_PARSE_EXIT_SUCCESS = 1,
    CLI_PARSE_EXIT_FAILURE = 2
} CliParseResult;

typedef struct CliOptions {
    const char *root_path;
    const char *selection_name;
    const char *const *suffixes;
    size_t suffix_count;
    char root_path_storage[CLI_INPUT_SIZE];
    char custom_suffix_storage[CLI_INPUT_SIZE];
    const char *custom_suffixes[CLI_MAX_CUSTOM_SUFFIXES];
} CliOptions;

CliParseResult cli_parse_options(int argc, char **argv, CliOptions *options);

void cli_print_suffixes(FILE *stream, const char *const *suffixes, size_t suffix_count);

#endif /* CLC_CLI_OPTIONS_H */
