#include <cloc/source_preset.h>

#include "test_assert.h"

#include <stdio.h>

static int test_default_preset(void) {
    const ClocSourcePreset *preset = cloc_default_source_preset();

    if (!CLOC_EXPECT_TRUE(preset != NULL)) {
        return 1;
    }
    if (!CLOC_EXPECT_TRUE(preset->name != NULL)) {
        return 1;
    }
    if (!CLOC_EXPECT_TRUE(preset->suffixes != NULL)) {
        return 1;
    }
    if (!CLOC_EXPECT_TRUE(preset->suffix_count > 0U)) {
        return 1;
    }
    if (!CLOC_EXPECT_TRUE(cloc_find_source_preset("common") == preset)) {
        return 1;
    }

    return 0;
}

static int test_find_preset(void) {
    const ClocSourcePreset *web = cloc_find_source_preset("web");
    const ClocSourcePreset *dotnet = cloc_find_source_preset("DOTNET");

    if (!CLOC_EXPECT_TRUE(web != NULL)) {
        return 1;
    }
    if (!CLOC_EXPECT_TRUE(dotnet != NULL)) {
        return 1;
    }
    if (!CLOC_EXPECT_TRUE(cloc_find_source_preset("unknown") == NULL)) {
        return 1;
    }
    if (!CLOC_EXPECT_TRUE(cloc_find_source_preset(NULL) == NULL)) {
        return 1;
    }

    return 0;
}

static int test_all_presets_are_usable(void) {
    size_t count = 0U;
    size_t index = 0U;
    const ClocSourcePreset *presets = cloc_source_presets(&count);

    if (!CLOC_EXPECT_TRUE(presets != NULL)) {
        return 1;
    }
    if (!CLOC_EXPECT_TRUE(count >= 5U)) {
        return 1;
    }

    for (index = 0U; index < count; ++index) {
        if (!CLOC_EXPECT_TRUE(presets[index].name != NULL)) {
            return 1;
        }
        if (!CLOC_EXPECT_TRUE(presets[index].description != NULL)) {
            return 1;
        }
        if (!CLOC_EXPECT_TRUE(presets[index].suffixes != NULL)) {
            return 1;
        }
        if (!CLOC_EXPECT_TRUE(presets[index].suffix_count > 0U)) {
            return 1;
        }
    }

    return 0;
}

int main(void) {
    if (test_default_preset()) {
        return 1;
    }
    if (test_find_preset()) {
        return 1;
    }
    if (test_all_presets_are_usable()) {
        return 1;
    }

    printf("[PASS] source preset tests\n");
    return 0;
}
