#include <codelinecalculator/line_counter.h>
#include <codelinecalculator/source_preset.h>
#include <codelinecalculator/version.h>

int main(void) {
    const char *suffixes[] = {".no-such-source-suffix"};
    CodeLineCalculatorScanOptions options = {".", suffixes, 1U};
    CodeLineCalculatorScanResult result;

    if (!codelinecalculator_default_source_preset()) {
        return 1;
    }

    if (CODELINECALCULATOR_VERSION_MAJOR < 0) {
        return 1;
    }

    return codelinecalculator_count_source_lines(&options, &result) == CODELINECALCULATOR_STATUS_OK
               ? 0
               : 1;
}
