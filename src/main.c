#include <cloc/line_counter.h>

#include "cli_options.h"

#include <stdio.h>

int main(int argc, char **argv) {
    CliOptions cli;
    CliParseResult parse_result = CLI_PARSE_EXIT_FAILURE;
    ClocScanOptions options;
    ClocScanResult result;
    ClocStatus status = CLOC_STATUS_OK;

    parse_result = cli_parse_options(argc, argv, &cli);
    if (parse_result == CLI_PARSE_EXIT_SUCCESS) {
        return 0;
    }
    if (parse_result == CLI_PARSE_EXIT_FAILURE) {
        return 1;
    }

    options.root_path = cli.root_path;
    options.suffixes = cli.suffixes;
    options.suffix_count = cli.suffix_count;

    status = cloc_count_source_lines(&options, &result);
    if (status != CLOC_STATUS_OK) {
        fprintf(stderr, "Failed to count source lines: %s\n", cloc_status_name(status));
        return 1;
    }

    printf("Project: %s\n", cli.root_path);
    printf("Selection: %s\n", cli.selection_name);
    cli_print_suffixes(stdout, cli.suffixes, cli.suffix_count);
    printf("Total source lines: %llu\n", result.total_lines);
    printf("Matched source files: %llu\n", result.matched_files);
    printf("Visited files: %llu\n", result.visited_files);
    printf("Skipped directories: %llu\n", result.skipped_directories);

    if (result.failed_entries > 0U) {
        fprintf(stderr, "Warning: failed entries: %llu\n", result.failed_entries);
        return 2;
    }

    return 0;
}
