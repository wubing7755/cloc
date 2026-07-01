cmake_minimum_required(VERSION 3.21)

project(CodeLineCalculatorSubprojectSmoke LANGUAGES C)

if(NOT DEFINED CODELINECALCULATOR_SOURCE_DIR)
    message(FATAL_ERROR "CODELINECALCULATOR_SOURCE_DIR must point at the CodeLineCalculator source tree")
endif()

add_subdirectory("${CODELINECALCULATOR_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/codelinecalculator-standard")

if(NOT TARGET CodeLineCalculator::codelinecalculator_core)
    message(FATAL_ERROR "Expected CodeLineCalculator::codelinecalculator_core alias target")
endif()

foreach(option_name IN ITEMS
        CODELINECALCULATOR_BUILD_CLI
        CODELINECALCULATOR_BUILD_TESTING
        CODELINECALCULATOR_INSTALL)
    if(${option_name})
        message(FATAL_ERROR "${option_name} should default to OFF for subproject builds")
    endif()
endforeach()

foreach(target_name IN ITEMS
        codelinecalculator
        codelinecalculator_tests)
    if(TARGET ${target_name})
        message(FATAL_ERROR "${target_name} should not be created by default for subproject builds")
    endif()
endforeach()

add_executable(subproject_smoke main.c)
target_link_libraries(subproject_smoke PRIVATE CodeLineCalculator::codelinecalculator_core)

enable_testing()
add_test(NAME subproject_smoke COMMAND subproject_smoke)
