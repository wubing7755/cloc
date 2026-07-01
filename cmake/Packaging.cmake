include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CODELINECALCULATOR_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/CodeLineCalculator")

install(TARGETS codelinecalculator_core
    EXPORT CodeLineCalculatorTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CODELINECALCULATOR_GENERATED_INCLUDE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CodeLineCalculatorConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CodeLineCalculatorConfig.cmake"
    INSTALL_DESTINATION "${CODELINECALCULATOR_INSTALL_CMAKEDIR}"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CodeLineCalculatorConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
)

install(EXPORT CodeLineCalculatorTargets
    NAMESPACE CodeLineCalculator::
    DESTINATION ${CODELINECALCULATOR_INSTALL_CMAKEDIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CodeLineCalculatorConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CodeLineCalculatorConfigVersion.cmake"
    DESTINATION ${CODELINECALCULATOR_INSTALL_CMAKEDIR}
)
