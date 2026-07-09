#ifndef CLOC_SOURCE_PRESET_H
#define CLOC_SOURCE_PRESET_H

#include <cloc/export.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ClocSourcePreset {
    const char *name;
    const char *description;
    const char *const *suffixes;
    size_t suffix_count;
} ClocSourcePreset;

/* Return the default preset used when users do not choose a suffix list. */
CLOC_API const ClocSourcePreset *cloc_default_source_preset(void);

/* Find a preset by name. Matching is case-insensitive. */
CLOC_API const ClocSourcePreset *cloc_find_source_preset(const char *name);

/* Return all built-in presets. If count is non-NULL, it receives the item count. */
CLOC_API const ClocSourcePreset *cloc_source_presets(size_t *count);

#ifdef __cplusplus
}
#endif

#endif /* CLOC_SOURCE_PRESET_H */
