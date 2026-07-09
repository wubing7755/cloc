cmake_minimum_required(VERSION 3.21)

project(ClocSubprojectSmoke LANGUAGES C)

if(NOT DEFINED CLOC_SOURCE_DIR)
    message(FATAL_ERROR "CLOC_SOURCE_DIR must point at the Cloc source tree")
endif()

add_subdirectory("${CLOC_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/cloc-standard")

if(NOT TARGET Cloc::cloc_core)
    message(FATAL_ERROR "Expected Cloc::cloc_core alias target")
endif()

foreach(option_name IN ITEMS
        CLOC_BUILD_CLI
        CLOC_BUILD_TESTING
        CLOC_INSTALL)
    if(${option_name})
        message(FATAL_ERROR "${option_name} should default to OFF for subproject builds")
    endif()
endforeach()

foreach(target_name IN ITEMS
        cloc
        cloc_tests)
    if(TARGET ${target_name})
        message(FATAL_ERROR "${target_name} should not be created by default for subproject builds")
    endif()
endforeach()

add_executable(subproject_smoke main.c)
target_link_libraries(subproject_smoke PRIVATE Cloc::cloc_core)

enable_testing()
add_test(NAME subproject_smoke COMMAND subproject_smoke)
