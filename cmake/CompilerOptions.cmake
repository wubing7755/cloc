function(codelinecalculator_target_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
        )
    endif()
endfunction()

function(codelinecalculator_target_sanitizers target)
    if(CODELINECALCULATOR_ENABLE_ASAN)
        if(MSVC)
            target_compile_options(${target} PRIVATE /fsanitize=address)
            target_link_options(${target} PRIVATE /fsanitize=address)
        else()
            target_compile_options(${target} PRIVATE -fsanitize=address)
            target_link_options(${target} PRIVATE -fsanitize=address)
        endif()
    endif()

    if(CODELINECALCULATOR_ENABLE_UBSAN)
        if(NOT MSVC)
            target_compile_options(${target} PRIVATE -fsanitize=undefined)
            target_link_options(${target} PRIVATE -fsanitize=undefined)
        else()
            message(WARNING
                "CODELINECALCULATOR_ENABLE_UBSAN is ON, but UBSAN is not configured for MSVC in this project.")
        endif()
    endif()
endfunction()

function(codelinecalculator_target_coverage target)
    if(NOT CODELINECALCULATOR_ENABLE_COVERAGE)
        return()
    endif()

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_compile_options(${target} PRIVATE --coverage -O0 -g)
        target_link_options(${target} PRIVATE --coverage)
    else()
        message(WARNING
            "CODELINECALCULATOR_ENABLE_COVERAGE is ON, but coverage flags are only configured for GCC and Clang-like compilers.")
    endif()
endfunction()

function(codelinecalculator_target_defaults target)
    target_compile_features(${target} PRIVATE c_std_11)
    codelinecalculator_target_warnings(${target})
    codelinecalculator_target_sanitizers(${target})
    codelinecalculator_target_coverage(${target})
endfunction()
