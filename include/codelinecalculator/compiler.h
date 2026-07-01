#ifndef CODELINECALCULATOR_COMPILER_H
#define CODELINECALCULATOR_COMPILER_H

/*
 * Compiler and platform detection macros.
 *
 * This header provides a reference pattern for portable C code across the
 * four supported compiler families (MSVC, GCC, Clang, AppleClang) and three
 * target platforms (Windows, Linux, macOS).
 */

/* ---- Compiler family detection ---- */

#if defined(_MSC_VER)
#define CODELINECALCULATOR_COMPILER_MSVC 1
#elif defined(__clang__) && defined(__APPLE__)
#define CODELINECALCULATOR_COMPILER_APPLECLANG 1
#elif defined(__clang__)
#define CODELINECALCULATOR_COMPILER_CLANG 1
#elif defined(__GNUC__)
#define CODELINECALCULATOR_COMPILER_GCC 1
#else
#warning "Unknown compiler. Portable macros may not be available."
#endif

/* ---- Platform detection ---- */

#if defined(_WIN32)
#define CODELINECALCULATOR_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
#define CODELINECALCULATOR_PLATFORM_MACOS 1
#elif defined(__linux__)
#define CODELINECALCULATOR_PLATFORM_LINUX 1
#endif

/* ---- Portable function attributes ---- */

#if CODELINECALCULATOR_COMPILER_MSVC
#define CODELINECALCULATOR_NORETURN __declspec(noreturn)
#define CODELINECALCULATOR_DEPRECATED(msg) __declspec(deprecated(msg))
#else
#define CODELINECALCULATOR_NORETURN __attribute__((__noreturn__))
#define CODELINECALCULATOR_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#endif

#endif /* CODELINECALCULATOR_COMPILER_H */
