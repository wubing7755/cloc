#include "cli_options.h"

#include <cloc/source_preset.h>

#include "internal_utils.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int is_token_separator(char value) {
    return value == ' ' || value == '\t' || value == '\r' || value == '\n' || value == ',' ||
           value == ';';
}

static int string_equal(const char *left, const char *right) {
    return left && right && strcmp(left, right) == 0;
}

static int is_help_argument(const char *argument) {
    return string_equal(argument, "--help") || string_equal(argument, "-h") ||
           string_equal(argument, "help");
}

static int is_interactive_argument(const char *argument) {
    return string_equal(argument, "--interactive") || string_equal(argument, "-i") ||
           string_equal(argument, "interactive");
}

static int is_list_presets_argument(const char *argument) {
    return string_equal(argument, "--list-presets") || string_equal(argument, "presets");
}

static int is_option_argument(const char *argument) {
    return argument && argument[0] == '-' && argument[1] == '-';
}

static int is_absolute_path(const char *path) {
    if (!path || path[0] == '\0') {
        return 0;
    }

#if CLOC_PLATFORM_WINDOWS
    if (path[1] != '\0' && path[2] != '\0' && clc_ascii_is_alpha(path[0]) && path[1] == ':' &&
        clc_path_is_separator(path[2])) {
        return 1;
    }
    return clc_path_is_separator(path[0]) && clc_path_is_separator(path[1]) && path[2] != '\0';
#else
    return path[0] == '/';
#endif
}

static char *trim_left(char *text) {
    while (*text == ' ' || *text == '\t' || *text == '\r' || *text == '\n') {
        ++text;
    }
    return text;
}

static void trim_right(char *text) {
    size_t length = strlen(text);
    while (length > 0U && is_token_separator(text[length - 1U])) {
        text[length - 1U] = '\0';
        --length;
    }
}

static char *trim(char *text) {
    char *start = trim_left(text);
    trim_right(start);
    return start;
}

static int copy_string(char *target, size_t target_size, const char *value) {
    const size_t value_length = strlen(value);

    if (value_length >= target_size) {
        return 0;
    }

    memcpy(target, value, value_length + 1U);
    return 1;
}

static int parse_positive_size(const char *text, size_t *value) {
    size_t parsed = 0U;
    size_t index = 0U;

    if (!text || text[0] == '\0') {
        return 0;
    }

    while (text[index] != '\0') {
        size_t digit = 0U;
        if (!clc_ascii_is_digit(text[index])) {
            return 0;
        }
        digit = (size_t)(text[index] - '0');
        if (parsed > (SIZE_MAX - digit) / 10U) {
            return 0;
        }
        parsed = parsed * 10U + digit;
        ++index;
    }

    if (parsed == 0U) {
        return 0;
    }

    *value = parsed;
    return 1;
}

static const char *display_program_name(const char *program_name) {
    const char *display_name = program_name;
    size_t index = 0U;

    if (!program_name || program_name[0] == '\0') {
        return "cloc";
    }

    while (program_name[index] != '\0') {
        if (clc_path_is_separator(program_name[index])) {
            display_name = program_name + index + 1U;
        }
        ++index;
    }

    return display_name[0] == '\0' ? "cloc" : display_name;
}

static void print_usage(FILE *stream, const char *program_name) {
    const char *name = display_program_name(program_name);

    fprintf(stream, "Cloc - count source-code lines in a project.\n\n");
    fprintf(stream, "Quick start:\n");
    fprintf(stream, "  %s C:\\work\\project\n", name);
    fprintf(stream, "  %s --interactive\n\n", name);
    fprintf(stream, "Usage:\n");
    fprintf(stream, "  %s <absolute-project-path>\n", name);
    fprintf(stream, "  %s <absolute-project-path> --preset <name>\n", name);
    fprintf(stream, "  %s <absolute-project-path> <suffix...>\n", name);
    fprintf(stream, "  %s --interactive\n", name);
    fprintf(stream, "  %s --list-presets\n", name);
    fprintf(stream, "  %s --help\n\n", name);
    fprintf(stream, "Examples:\n");
    fprintf(stream, "  %s C:\\work\\project --preset web\n", name);
    fprintf(stream, "  %s C:\\work\\project --preset dotnet\n", name);
    fprintf(stream, "  %s C:\\work\\project *.c *.h *.js *.ts *.cs *.razor *.css\n\n", name);
    fprintf(stream, "Presets:\n");
    fprintf(stream, "  common   Default. Common source files.\n");
    fprintf(stream, "  c        C source and header files.\n");
    fprintf(stream, "  cpp      C and C++ source/header files.\n");
    fprintf(stream, "  web      JavaScript, TypeScript, CSS, HTML, Vue, and Svelte files.\n");
    fprintf(stream, "  dotnet   C#, Razor, CSHTML, and XAML files.\n\n");
    fprintf(stream, "Notes:\n");
    fprintf(stream, "  Project path must be absolute.\n");
    fprintf(stream, "  Suffixes may be written as *.js, .js, or js.\n");
    fprintf(stream, "  Use the 'Total source lines' output value for copyright forms.\n");
}

static void print_presets(FILE *stream) {
    size_t count = 0U;
    size_t index = 0U;
    const ClocSourcePreset *presets = cloc_source_presets(&count);

    fprintf(stream, "Available presets:\n");
    for (index = 0U; index < count; ++index) {
        fprintf(stream, "  %zu. %-7s %s\n", index + 1U, presets[index].name,
                presets[index].description);
    }
    fprintf(stream, "  %zu. custom  Custom suffix list\n", count + 1U);
}

static void select_preset(CliOptions *options, const ClocSourcePreset *preset) {
    options->selection_name = preset->name;
    options->suffixes = preset->suffixes;
    options->suffix_count = preset->suffix_count;
}

static int parse_suffix_tokens(char *text, CliOptions *options) {
    size_t count = 0U;
    char *cursor = text;

    while (*cursor != '\0') {
        while (*cursor != '\0' && is_token_separator(*cursor)) {
            *cursor = '\0';
            ++cursor;
        }

        if (*cursor == '\0') {
            break;
        }

        if (count == CLI_MAX_CUSTOM_SUFFIXES) {
            return 0;
        }

        options->custom_suffixes[count] = cursor;
        ++count;

        while (*cursor != '\0' && !is_token_separator(*cursor)) {
            ++cursor;
        }
    }

    if (count == 0U) {
        return 0;
    }

    options->selection_name = "custom";
    options->suffixes = options->custom_suffixes;
    options->suffix_count = count;
    return 1;
}

static int suffix_arguments_are_valid(int argc, char **argv, int first_suffix_index) {
    int index = 0;
    for (index = first_suffix_index; index < argc; ++index) {
        if (!argv[index] || argv[index][0] == '\0') {
            return 0;
        }
    }
    return 1;
}

static int configure_custom_suffixes_from_argv(int argc, char **argv, CliOptions *options) {
    if (!suffix_arguments_are_valid(argc, argv, 2)) {
        fprintf(stderr, "Suffix arguments must not be empty.\n");
        return 0;
    }

    options->selection_name = "custom";
    options->suffixes = (const char *const *)&argv[2];
    options->suffix_count = (size_t)argc - 2U;
    return 1;
}

static int configure_path(CliOptions *options, const char *path, int copy_to_storage) {
    if (!is_absolute_path(path)) {
        fprintf(stderr, "Project path must be absolute: %s\n", path);
        return 0;
    }

    if (copy_to_storage) {
        if (!copy_string(options->root_path_storage, sizeof(options->root_path_storage), path)) {
            fprintf(stderr, "Project path is too long.\n");
            return 0;
        }
        options->root_path = options->root_path_storage;
    } else {
        options->root_path = path;
    }

    return 1;
}

static int read_prompt_line(const char *prompt, char *buffer, int buffer_size) {
    printf("%s", prompt);
    fflush(stdout);

    if (!fgets(buffer, buffer_size, stdin)) {
        fprintf(stderr, "Failed to read input.\n");
        return 0;
    }

    return 1;
}

static int configure_interactive_path(CliOptions *options) {
    int attempt = 0;
    char input[CLI_INPUT_SIZE];

    for (attempt = 0; attempt < 3; ++attempt) {
        char *path = NULL;
        if (!read_prompt_line("Project absolute path: ", input, CLI_INPUT_SIZE)) {
            return 0;
        }

        path = trim(input);
        if (path[0] == '\0') {
            fprintf(stderr, "Project path is required.\n");
            continue;
        }

        if (configure_path(options, path, 1)) {
            return 1;
        }
    }

    return 0;
}

static int configure_interactive_custom_suffixes(CliOptions *options) {
    char *suffixes = NULL;

    if (!read_prompt_line("Custom suffixes: ", options->custom_suffix_storage, CLI_INPUT_SIZE)) {
        return 0;
    }

    suffixes = trim(options->custom_suffix_storage);
    if (suffixes != options->custom_suffix_storage) {
        memmove(options->custom_suffix_storage, suffixes, strlen(suffixes) + 1U);
    }

    if (!parse_suffix_tokens(options->custom_suffix_storage, options)) {
        fprintf(stderr, "Enter one or more suffixes separated by spaces, commas, or semicolons.\n");
        return 0;
    }

    return 1;
}

static int configure_interactive_selection(CliOptions *options) {
    size_t count = 0U;
    const ClocSourcePreset *presets = cloc_source_presets(&count);
    char input[CLI_INPUT_SIZE];
    char *choice = NULL;
    size_t choice_number = 0U;
    const ClocSourcePreset *preset = NULL;

    printf("\nChoose source selection:\n");
    print_presets(stdout);
    printf("\nPress Enter for common, or enter a preset number/name.\n");

    if (!read_prompt_line("Selection: ", input, CLI_INPUT_SIZE)) {
        return 0;
    }

    choice = trim(input);
    if (choice[0] == '\0') {
        select_preset(options, cloc_default_source_preset());
        return 1;
    }

    if (parse_positive_size(choice, &choice_number)) {
        if (choice_number >= 1U && choice_number <= count) {
            select_preset(options, &presets[choice_number - 1U]);
            return 1;
        }

        if (choice_number == count + 1U) {
            return configure_interactive_custom_suffixes(options);
        }

        fprintf(stderr, "Selection number is out of range.\n");
        return 0;
    }

    if (string_equal(choice, "custom")) {
        return configure_interactive_custom_suffixes(options);
    }

    preset = ccloc_find_source_preset(choice);
    if (!preset) {
        fprintf(stderr, "Unknown preset: %s\n", choice);
        return 0;
    }

    select_preset(options, preset);
    return 1;
}

static CliParseResult configure_interactive(CliOptions *options) {
    printf("Cloc interactive mode\n\n");

    if (!configure_interactive_path(options)) {
        return CLI_PARSE_EXIT_FAILURE;
    }

    if (!configure_interactive_selection(options)) {
        return CLI_PARSE_EXIT_FAILURE;
    }

    return CLI_PARSE_RUN;
}

CliParseResult cli_parse_options(int argc, char **argv, CliOptions *options) {
    const ClocSourcePreset *preset = NULL;

    if (!options || argc < 1 || !argv || !argv[0]) {
        return CLI_PARSE_EXIT_FAILURE;
    }

    memset(options, 0, sizeof(*options));
    select_preset(options, cloc_default_source_preset());

    if (argc == 2 && is_help_argument(argv[1])) {
        print_usage(stdout, argv[0]);
        return CLI_PARSE_EXIT_SUCCESS;
    }

    if (argc == 2 && is_list_presets_argument(argv[1])) {
        print_presets(stdout);
        return CLI_PARSE_EXIT_SUCCESS;
    }

    if (argc == 2 && is_interactive_argument(argv[1])) {
        return configure_interactive(options);
    }

    if (argc < 2) {
        print_usage(stderr, argv[0]);
        return CLI_PARSE_EXIT_FAILURE;
    }

    if (is_option_argument(argv[1])) {
        fprintf(stderr, "Unknown option: %s\n", argv[1]);
        return CLI_PARSE_EXIT_FAILURE;
    }

    if (!configure_path(options, argv[1], 0)) {
        return CLI_PARSE_EXIT_FAILURE;
    }

    if (argc == 3 && strcmp(argv[2], "--preset") == 0) {
        fprintf(stderr, "--preset requires a preset name.\n");
        return CLI_PARSE_EXIT_FAILURE;
    }

    if (argc > 2 && strcmp(argv[2], "--preset") == 0) {
        if (argc > 4) {
            fprintf(stderr, "--preset cannot be combined with custom suffix arguments.\n");
            return CLI_PARSE_EXIT_FAILURE;
        }

        preset = ccloc_find_source_preset(argv[3]);
        if (!preset) {
            fprintf(stderr, "Unknown preset: %s\n", argv[3]);
            print_presets(stderr);
            return CLI_PARSE_EXIT_FAILURE;
        }

        select_preset(options, preset);
        options->root_path = argv[1];
    } else if (argc > 2) {
        if (is_option_argument(argv[2])) {
            fprintf(stderr, "Unknown option: %s\n", argv[2]);
            return CLI_PARSE_EXIT_FAILURE;
        }

        if (!configure_custom_suffixes_from_argv(argc, argv, options)) {
            return CLI_PARSE_EXIT_FAILURE;
        }
    }

    return CLI_PARSE_RUN;
}

void cli_print_suffixes(FILE *stream, const char *const *suffixes, size_t suffix_count) {
    size_t index = 0;

    fprintf(stream, "Suffixes: ");
    for (index = 0; index < suffix_count; ++index) {
        fprintf(stream, "%s%s", index == 0U ? "" : ", ", suffixes[index]);
    }
    fprintf(stream, "\n");
}
