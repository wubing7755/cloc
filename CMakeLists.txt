cmake_minimum_required(VERSION 3.21)

project(CodeLineCalculator
    VERSION 0.1.0
    DESCRIPTION "A C11 command-line tool for counting project source lines"
    LANGUAGES C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CODELINECALCULATOR_IS_TOP_LEVEL_DEFAULT ON)
else()
    set(CODELINECALCULATOR_IS_TOP_LEVEL_DEFAULT OFF)
endif()

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(CODELINECALCULATOR_ENABLE_ASAN "Enable AddressSanitizer where supported" OFF)
option(CODELINECALCULATOR_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer where supported" OFF)
option(CODELINECALCULATOR_ENABLE_COVERAGE "Enable coverage flags where supported" OFF)
option(CODELINECALCULATOR_BUILD_CLI "Build the command-line executable" ${CODELINECALCULATOR_IS_TOP_LEVEL_DEFAULT})
option(CODELINECALCULATOR_BUILD_TESTING "Build CTest tests" ${CODELINECALCULATOR_IS_TOP_LEVEL_DEFAULT})
option(CODELINECALCULATOR_INSTALL "Generate install rules" ${CODELINECALCULATOR_IS_TOP_LEVEL_DEFAULT})

set(CODELINECALCULATOR_GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/include")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/codelinecalculator_version.h.in"
    "${CODELINECALCULATOR_GENERATED_INCLUDE_DIR}/codelinecalculator/version.h"
    @ONLY
)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerOptions.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Sources.cmake")

add_library(codelinecalculator_core ${CODELINECALCULATOR_CORE_SOURCES})
add_library(CodeLineCalculator::codelinecalculator_core ALIAS codelinecalculator_core)

target_compile_features(codelinecalculator_core PUBLIC c_std_11)

target_compile_definitions(codelinecalculator_core
    PRIVATE
        CODELINECALCULATOR_BUILDING_LIBRARY
)
if(BUILD_SHARED_LIBS)
    target_compile_definitions(codelinecalculator_core PUBLIC CODELINECALCULATOR_SHARED)
endif()
set_target_properties(codelinecalculator_core PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_include_directories(codelinecalculator_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CODELINECALCULATOR_GENERATED_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)
codelinecalculator_target_defaults(codelinecalculator_core)

if(CODELINECALCULATOR_BUILD_CLI)
    add_executable(codelinecalculator ${CODELINECALCULATOR_CLI_SOURCES})
    target_link_libraries(codelinecalculator PRIVATE codelinecalculator_core)
    codelinecalculator_target_defaults(codelinecalculator)
    set_target_properties(codelinecalculator PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

if(CODELINECALCULATOR_BUILD_TESTING)
    enable_testing()
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Tests.cmake")
endif()

if(CODELINECALCULATOR_INSTALL)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Packaging.cmake")
endif()
