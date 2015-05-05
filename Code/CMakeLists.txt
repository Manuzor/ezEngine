#cmake

#Clear Targets.txt and Tests.txt
file(WRITE "${CMAKE_BINARY_DIR}/Targets.txt" "")
file(WRITE "${CMAKE_BINARY_DIR}/Tests.txt" "")
file(COPY "${CMAKE_CURRENT_LIST_DIR}/BuildSystem/CMake/ezEngineInstallTargets.cmake" DESTINATION "${CMAKE_BINARY_DIR}/")

set(EZ_INSTALLED_TARGETS "" CACHE INTERNAL "Targets that will be installed.")
set(EZ_EXPORT_NAMESPACE "ez" CACHE STRING "Prefix for all exported targets.")
mark_as_advanced(EZ_EXPORT_NAMESPACE)

include ("${CMAKE_CURRENT_LIST_DIR}/BuildSystem/CMake/CMAKE_GlobProjects.txt")

# Export install rules of all projects
install(EXPORT ezEngine NAMESPACE "${EZ_EXPORT_NAMESPACE}" DESTINATION CMake/)

include(CMakePackageConfigHelpers)
configure_package_config_file("${CMAKE_CURRENT_LIST_DIR}/BuildSystem/CMake/ezEngineConfig.cmake.in"
                              "${CMAKE_BINARY_DIR}/ezEngineConfig.cmake"
                              INSTALL_DESTINATION .)
install(FILES "${CMAKE_BINARY_DIR}/ezEngineConfig.cmake" DESTINATION .)
install(FILES "${CMAKE_BINARY_DIR}/ezEngineInstallTargets.cmake" DESTINATION CMake/)

# Install rule for Code files.
install(DIRECTORY . DESTINATION "Code/")
