#include <cloc/line_counter.h>
#include <cloc/source_preset.h>

int main() {
    const ClocSourcePreset *preset = cloc_default_source_preset();
    return preset && preset->suffix_count > 0 && cloc_status_name(CLOC_STATUS_OK) ? 0 : 1;
}
