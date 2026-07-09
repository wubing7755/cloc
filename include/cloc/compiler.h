#ifndef CLOC_COMPILER_H
#define CLOC_COMPILER_H

/*
 * Compiler and platform detection macros.
 *
 * This header provides a reference pattern for portable C code across the
 * four supported compiler families (MSVC, GCC, Clang, AppleClang) and three
 * target platforms (Windows, Linux, macOS).
 */

/* ---- Compiler family detection ---- */

#if defined(_MSC_VER)
#define CLOC_COMPILER_MSVC 1
#elif defined(__clang__) && defined(__APPLE__)
#define CLOC_COMPILER_APPLECLANG 1
#elif defined(__clang__)
#define CLOC_COMPILER_CLANG 1
#elif defined(__GNUC__)
#define CLOC_COMPILER_GCC 1
#else
#warning "Unknown compiler. Portable macros may not be available."
#endif

/* ---- Platform detection ---- */

#if defined(_WIN32)
#define CLOC_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
#define CLOC_PLATFORM_MACOS 1
#elif defined(__linux__)
#define CLOC_PLATFORM_LINUX 1
#endif

/* ---- Portable function attributes ---- */

#if CLOC_COMPILER_MSVC
#define CLOC_NORETURN __declspec(noreturn)
#define CLOC_DEPRECATED(msg) __declspec(deprecated(msg))
#else
#define CLOC_NORETURN __attribute__((__noreturn__))
#define CLOC_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#endif

#endif /* CLOC_COMPILER_H */
