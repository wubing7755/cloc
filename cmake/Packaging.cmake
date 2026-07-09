include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CLOC_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/cloc")

install(TARGETS cloc_core
    EXPORT clocTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CLOC_GENERATED_INCLUDE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/clocConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/clocConfig.cmake"
    INSTALL_DESTINATION "${CLOC_INSTALL_CMAKEDIR}"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/clocConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
)

install(EXPORT clocTargets
    NAMESPACE cloc::
    DESTINATION ${CLOC_INSTALL_CMAKEDIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/clocConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/clocConfigVersion.cmake"
    DESTINATION ${CLOC_INSTALL_CMAKEDIR}
)
