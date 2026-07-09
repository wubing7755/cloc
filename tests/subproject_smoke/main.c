#include <cloc/line_counter.h>
#include <cloc/source_preset.h>
#include <cloc/version.h>

int main(void) {
    const char *suffixes[] = {".no-such-source-suffix"};
    ClocScanOptions options = {".", suffixes, 1U};
    ClocScanResult result;

    if (!cloc_default_source_preset()) {
        return 1;
    }

    if (CLOC_VERSION_MAJOR < 0) {
        return 1;
    }

    return cloc_count_source_lines(&options, &result) == CLOC_STATUS_OK ? 0 : 1;
}
