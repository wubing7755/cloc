#include "cli_options.h"

#include "test_assert.h"

#include <stdio.h>

#if defined(_WIN32)
#define CLC_TEST_ABSOLUTE_PATH "C:\\work\\project"
#else
#define CLC_TEST_ABSOLUTE_PATH "/work/project"
#endif

static int test_help_commands_exit_successfully(void) {
    CliOptions options;
    char *help_argv[] = {"codelinecalculator", "help"};
    char *dash_help_argv[] = {"codelinecalculator", "--help"};
    char *short_help_argv[] = {"codelinecalculator", "-h"};

    if (!CODELINECALCULATOR_EXPECT_INT_EQ((int)cli_parse_options(2, help_argv, &options),
                                          (int)CLI_PARSE_EXIT_SUCCESS)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_INT_EQ((int)cli_parse_options(2, dash_help_argv, &options),
                                          (int)CLI_PARSE_EXIT_SUCCESS)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_INT_EQ((int)cli_parse_options(2, short_help_argv, &options),
                                          (int)CLI_PARSE_EXIT_SUCCESS)) {
        return 1;
    }

    return 0;
}

static int test_list_presets_exits_successfully(void) {
    CliOptions options;
    char *argv[] = {"codelinecalculator", "--list-presets"};

    if (!CODELINECALCULATOR_EXPECT_INT_EQ((int)cli_parse_options(2, argv, &options),
                                          (int)CLI_PARSE_EXIT_SUCCESS)) {
        return 1;
    }

    return 0;
}

static int test_default_selection(void) {
    CliOptions options;
    char *argv[] = {"codelinecalculator", CLC_TEST_ABSOLUTE_PATH};

    if (!CODELINECALCULATOR_EXPECT_INT_EQ((int)cli_parse_options(2, argv, &options),
                                          (int)CLI_PARSE_RUN)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_TRUE(options.root_path == argv[1])) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_TRUE(options.selection_name != NULL)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_TRUE(options.suffix_count > 0U)) {
        return 1;
    }

    return 0;
}

static int test_preset_selection(void) {
    CliOptions options;
    char *argv[] = {"codelinecalculator", CLC_TEST_ABSOLUTE_PATH, "--preset", "web"};

    if (!CODELINECALCULATOR_EXPECT_INT_EQ((int)cli_parse_options(4, argv, &options),
                                          (int)CLI_PARSE_RUN)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_TRUE(options.selection_name != NULL)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_TRUE(options.suffixes != NULL)) {
        return 1;
    }

    return 0;
}

static int test_custom_suffixes(void) {
    CliOptions options;
    char *argv[] = {"codelinecalculator", CLC_TEST_ABSOLUTE_PATH, "*.js", ".ts", "css"};

    if (!CODELINECALCULATOR_EXPECT_INT_EQ((int)cli_parse_options(5, argv, &options),
                                          (int)CLI_PARSE_RUN)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_TRUE(options.selection_name != NULL)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_ULL_EQ(options.suffix_count, 3U)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_TRUE(options.suffixes[0] == argv[2])) {
        return 1;
    }

    return 0;
}

static int test_invalid_arguments_fail(void) {
    CliOptions options;
    char *relative_path_argv[] = {"codelinecalculator", "relative"};
    char *unknown_option_argv[] = {"codelinecalculator", "--unknown"};
    char *missing_preset_argv[] = {"codelinecalculator", CLC_TEST_ABSOLUTE_PATH, "--preset"};

    if (!CODELINECALCULATOR_EXPECT_INT_EQ((int)cli_parse_options(2, relative_path_argv, &options),
                                          (int)CLI_PARSE_EXIT_FAILURE)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_INT_EQ((int)cli_parse_options(2, unknown_option_argv, &options),
                                          (int)CLI_PARSE_EXIT_FAILURE)) {
        return 1;
    }
    if (!CODELINECALCULATOR_EXPECT_INT_EQ((int)cli_parse_options(3, missing_preset_argv, &options),
                                          (int)CLI_PARSE_EXIT_FAILURE)) {
        return 1;
    }

    return 0;
}

int main(void) {
    if (test_help_commands_exit_successfully()) {
        return 1;
    }
    if (test_list_presets_exits_successfully()) {
        return 1;
    }
    if (test_default_selection()) {
        return 1;
    }
    if (test_preset_selection()) {
        return 1;
    }
    if (test_custom_suffixes()) {
        return 1;
    }
    if (test_invalid_arguments_fail()) {
        return 1;
    }

    printf("[PASS] cli options tests\n");
    return 0;
}
