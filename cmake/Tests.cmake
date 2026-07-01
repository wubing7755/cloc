function(codelinecalculator_add_test target)
    cmake_parse_arguments(CODELINECALCULATOR_TEST
        ""
        ""
        "SOURCES;LIBS;INCLUDES"
        ${ARGN}
    )

    if(NOT CODELINECALCULATOR_TEST_SOURCES)
        message(FATAL_ERROR "codelinecalculator_add_test(${target}) requires SOURCES")
    endif()

    add_executable(${target}
        ${CODELINECALCULATOR_TEST_SOURCES}
    )

    target_include_directories(${target}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/support
            ${CODELINECALCULATOR_TEST_INCLUDES}
    )

    if(CODELINECALCULATOR_TEST_LIBS)
        target_link_libraries(${target} PRIVATE ${CODELINECALCULATOR_TEST_LIBS})
    endif()

    codelinecalculator_target_defaults(${target})

    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    set(test_working_directory "${CMAKE_CURRENT_BINARY_DIR}/test-work/${target}")
    file(MAKE_DIRECTORY "${test_working_directory}")

    add_test(NAME ${target} COMMAND $<TARGET_FILE:${target}>)
    set_tests_properties(${target} PROPERTIES
        WORKING_DIRECTORY "${test_working_directory}"
    )
    list(APPEND CODELINECALCULATOR_TEST_TARGETS ${target})
    set(CODELINECALCULATOR_TEST_TARGETS "${CODELINECALCULATOR_TEST_TARGETS}" PARENT_SCOPE)
endfunction()

set(CODELINECALCULATOR_TEST_TARGETS "")

codelinecalculator_add_test(codelinecalculator_line_counter_tests
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_line_counter.c
    LIBS
        codelinecalculator_core
)

codelinecalculator_add_test(codelinecalculator_source_preset_tests
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_source_preset.c
    LIBS
        codelinecalculator_core
)

codelinecalculator_add_test(codelinecalculator_cli_options_tests
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cli_options.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cli_options.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    LIBS
        codelinecalculator_core
)

add_custom_target(codelinecalculator_tests
    DEPENDS
        ${CODELINECALCULATOR_TEST_TARGETS}
)
