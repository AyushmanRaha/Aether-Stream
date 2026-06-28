include_guard(GLOBAL)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(AETHER_STREAM_PACKAGE_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/AetherStream")

install(
  TARGETS aether_stream
  EXPORT AetherStreamTargets
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/AetherStreamConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/AetherStreamConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/AetherStreamConfig.cmake"
  INSTALL_DESTINATION "${AETHER_STREAM_PACKAGE_DESTINATION}"
)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/AetherStreamConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/AetherStreamConfigVersion.cmake"
  DESTINATION "${AETHER_STREAM_PACKAGE_DESTINATION}"
)

install(
  EXPORT AetherStreamTargets
  NAMESPACE aether::
  FILE AetherStreamTargets.cmake
  DESTINATION "${AETHER_STREAM_PACKAGE_DESTINATION}"
)
