cmake_minimum_required(VERSION 3.21)

project(Cloc
    VERSION 0.1.0
    DESCRIPTION "A C11 command-line tool for counting project source lines"
    LANGUAGES C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CLOC_IS_TOP_LEVEL_DEFAULT ON)
else()
    set(CLOC_IS_TOP_LEVEL_DEFAULT OFF)
endif()

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(CLOC_ENABLE_ASAN "Enable AddressSanitizer where supported" OFF)
option(CLOC_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer where supported" OFF)
option(CLOC_ENABLE_COVERAGE "Enable coverage flags where supported" OFF)
option(CLOC_BUILD_CLI "Build the command-line executable" ${CLOC_IS_TOP_LEVEL_DEFAULT})
option(CLOC_BUILD_TESTING "Build CTest tests" ${CLOC_IS_TOP_LEVEL_DEFAULT})
option(CLOC_INSTALL "Generate install rules" ${CLOC_IS_TOP_LEVEL_DEFAULT})

set(CLOC_GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/include")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cloc_version.h.in"
    "${CLOC_GENERATED_INCLUDE_DIR}/cloc/version.h"
    @ONLY
)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerOptions.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Sources.cmake")

add_library(cloc_core ${CLOC_CORE_SOURCES})
add_library(Cloc::cloc_core ALIAS cloc_core)

target_compile_features(cloc_core PUBLIC c_std_11)

target_compile_definitions(cloc_core
    PRIVATE
        CLOC_BUILDING_LIBRARY
        $<$<NOT:$<BOOL:${MSVC}>>:_POSIX_C_SOURCE=200809L>
)
if(BUILD_SHARED_LIBS)
    target_compile_definitions(cloc_core PUBLIC CLOC_SHARED)
endif()
set_target_properties(cloc_core PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_include_directories(cloc_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CLOC_GENERATED_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)
cloc_target_defaults(cloc_core)

if(CLOC_BUILD_CLI)
    add_executable(cloc ${CLOC_CLI_SOURCES})
    target_link_libraries(cloc PRIVATE cloc_core)
    cloc_target_defaults(cloc)
    set_target_properties(cloc PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

if(CLOC_BUILD_TESTING)
    enable_testing()
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Tests.cmake")
endif()

if(CLOC_INSTALL)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Packaging.cmake")
endif()
