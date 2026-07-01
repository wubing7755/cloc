#ifndef CODELINECALCULATOR_SOURCE_PRESET_H
#define CODELINECALCULATOR_SOURCE_PRESET_H

#include <codelinecalculator/export.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CodeLineCalculatorSourcePreset {
    const char *name;
    const char *description;
    const char *const *suffixes;
    size_t suffix_count;
} CodeLineCalculatorSourcePreset;

/* Return the default preset used when users do not choose a suffix list. */
CODELINECALCULATOR_API const CodeLineCalculatorSourcePreset *
codelinecalculator_default_source_preset(void);

/* Find a preset by name. Matching is case-insensitive. */
CODELINECALCULATOR_API const CodeLineCalculatorSourcePreset *
codelinecalculator_find_source_preset(const char *name);

/* Return all built-in presets. If count is non-NULL, it receives the item count. */
CODELINECALCULATOR_API const CodeLineCalculatorSourcePreset *
codelinecalculator_source_presets(size_t *count);

#ifdef __cplusplus
}
#endif

#endif /* CODELINECALCULATOR_SOURCE_PRESET_H */
