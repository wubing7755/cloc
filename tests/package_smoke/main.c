#include <cloc/line_counter.h>
#include <cloc/source_preset.h>
#include <cloc/version.h>

int main(void) {
    const char *suffixes[] = {".no-such-source-suffix"};
    ClocScanOptions options = {".", suffixes, 1U};
    ClocScanResult result;
    const ClocSourcePreset *preset = cloc_default_source_preset();

    if (CLOC_VERSION_MAJOR < 0 || CLOC_VERSION_MINOR < 0 || CLOC_VERSION_PATCH < 0) {
        return 1;
    }

    if (!preset || !cloc_find_source_preset("common")) {
        return 1;
    }

    if (cloc_count_source_lines(&options, &result) != CLOC_STATUS_OK) {
        return 1;
    }

    if (result.matched_files != 0U || result.failed_entries != 0U) {
        return 1;
    }

    if (!cloc_status_name(CLOC_STATUS_OK)) {
        return 1;
    }

    return 0;
}
