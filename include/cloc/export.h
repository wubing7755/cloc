#ifndef CLOC_EXPORT_H
#define CLOC_EXPORT_H

#if defined(_WIN32) && defined(CLOC_SHARED)
#if defined(CLOC_BUILDING_LIBRARY)
#define CLOC_API __declspec(dllexport)
#else
#define CLOC_API __declspec(dllimport)
#endif
#else
#define CLOC_API
#endif

#endif /* CLOC_EXPORT_H */
