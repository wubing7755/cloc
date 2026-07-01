#include <codelinecalculator/line_counter.h>
#include <codelinecalculator/source_preset.h>

int main() {
    const CodeLineCalculatorSourcePreset *preset = codelinecalculator_default_source_preset();
    return preset && preset->suffix_count > 0 &&
                   codelinecalculator_status_name(CODELINECALCULATOR_STATUS_OK)
               ? 0
               : 1;
}
