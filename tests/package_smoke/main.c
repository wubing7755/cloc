#include <codelinecalculator/line_counter.h>
#include <codelinecalculator/source_preset.h>
#include <codelinecalculator/version.h>

int main(void) {
    const char *suffixes[] = {".no-such-source-suffix"};
    CodeLineCalculatorScanOptions options = {".", suffixes, 1U};
    CodeLineCalculatorScanResult result;
    const CodeLineCalculatorSourcePreset *preset = codelinecalculator_default_source_preset();

    if (CODELINECALCULATOR_VERSION_MAJOR < 0 || CODELINECALCULATOR_VERSION_MINOR < 0 ||
        CODELINECALCULATOR_VERSION_PATCH < 0) {
        return 1;
    }

    if (!preset || !codelinecalculator_find_source_preset("common")) {
        return 1;
    }

    if (codelinecalculator_count_source_lines(&options, &result) != CODELINECALCULATOR_STATUS_OK) {
        return 1;
    }

    if (result.matched_files != 0U || result.failed_entries != 0U) {
        return 1;
    }

    if (!codelinecalculator_status_name(CODELINECALCULATOR_STATUS_OK)) {
        return 1;
    }

    return 0;
}
