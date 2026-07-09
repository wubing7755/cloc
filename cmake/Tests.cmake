function(cloc_add_test target)
    cmake_parse_arguments(CLOC_TEST
        ""
        ""
        "SOURCES;LIBS;INCLUDES"
        ${ARGN}
    )

    if(NOT CLOC_TEST_SOURCES)
        message(FATAL_ERROR "cloc_add_test(${target}) requires SOURCES")
    endif()

    add_executable(${target}
        ${CLOC_TEST_SOURCES}
    )

    target_include_directories(${target}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/support
            ${CLOC_TEST_INCLUDES}
    )

    if(CLOC_TEST_LIBS)
        target_link_libraries(${target} PRIVATE ${CLOC_TEST_LIBS})
    endif()

    cloc_target_defaults(${target})

    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    set(test_working_directory "${CMAKE_CURRENT_BINARY_DIR}/test-work/${target}")
    file(MAKE_DIRECTORY "${test_working_directory}")

    add_test(NAME ${target} COMMAND $<TARGET_FILE:${target}>)
    set_tests_properties(${target} PROPERTIES
        WORKING_DIRECTORY "${test_working_directory}"
    )
    list(APPEND CLOC_TEST_TARGETS ${target})
    set(CLOC_TEST_TARGETS "${CLOC_TEST_TARGETS}" PARENT_SCOPE)
endfunction()

set(CLOC_TEST_TARGETS "")

cloc_add_test(cloc_line_counter_tests
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_line_counter.c
    LIBS
        cloc_core
)

cloc_add_test(cloc_source_preset_tests
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_source_preset.c
    LIBS
        cloc_core
)

cloc_add_test(cloc_cli_options_tests
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cli_options.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cli_options.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    LIBS
        cloc_core
)

add_custom_target(cloc_tests
    DEPENDS
        ${CLOC_TEST_TARGETS}
)
