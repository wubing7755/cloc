#include <cloc/source_preset.h>

#include "../internal_utils.h"

static const char *const kCommonSuffixes[] = {
    ".c",      ".h",   ".cpp",    ".hpp",  ".cc",  ".cxx", ".inl",  ".cs",   ".razor",
    ".cshtml", ".js",  ".jsx",    ".ts",   ".tsx", ".css", ".scss", ".sass", ".html",
    ".htm",    ".vue", ".svelte", ".java", ".kt",  ".kts", ".go",   ".rs",   ".py",
    ".php",    ".rb",  ".swift",  ".m",    ".mm",  ".sql", ".sh",   ".ps1",  ".xaml",
};

static const char *const kCSuffixes[] = {
    ".c",
    ".h",
};

static const char *const kCppSuffixes[] = {
    ".c", ".h", ".cpp", ".hpp", ".cc", ".cxx", ".inl",
};

static const char *const kWebSuffixes[] = {
    ".js", ".jsx", ".ts", ".tsx", ".css", ".scss", ".sass", ".html", ".htm", ".vue", ".svelte",
};

static const char *const kDotNetSuffixes[] = {
    ".cs",
    ".razor",
    ".cshtml",
    ".xaml",
};

static const ClocSourcePreset kSourcePresets[] = {
    {"common", "Common source files across mainstream project types", kCommonSuffixes,
     CLC_ARRAY_COUNT(kCommonSuffixes)},
    {"c", "C source and header files", kCSuffixes, CLC_ARRAY_COUNT(kCSuffixes)},
    {"cpp", "C and C++ source/header files", kCppSuffixes, CLC_ARRAY_COUNT(kCppSuffixes)},
    {"web", "JavaScript, TypeScript, CSS, HTML, Vue, and Svelte files", kWebSuffixes,
     CLC_ARRAY_COUNT(kWebSuffixes)},
    {"dotnet", ".NET, Razor, and XAML files", kDotNetSuffixes, CLC_ARRAY_COUNT(kDotNetSuffixes)},
};

const ClocSourcePreset *cloc_default_source_preset(void) {
    return &kSourcePresets[0];
}

const ClocSourcePreset *cloc_find_source_preset(const char *name) {
    size_t index = 0;

    for (index = 0; index < CLC_ARRAY_COUNT(kSourcePresets); ++index) {
        if (clc_string_equal_ignore_case(name, kSourcePresets[index].name)) {
            return &kSourcePresets[index];
        }
    }

    return NULL;
}

const ClocSourcePreset *cloc_source_presets(size_t *count) {
    if (count) {
        *count = CLC_ARRAY_COUNT(kSourcePresets);
    }
    return kSourcePresets;
}
