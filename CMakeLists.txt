cmake_minimum_required( VERSION 3.11 )

if( TARGET gputils )
  return()
endif( TARGET gputils )

OPTION( BUILD_UNIT_TESTS "Build unit tests for the library" ON )
OPTION( INSTALL_LIBRARY "Build and install library and header files." ON )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
set(INSTALL_PATH ${INSTALL_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/externals/install/")

include(function-git_version)
GIT_VERSION(gputils)
message( STATUS "gputils version: ${gputils_VERSION}")
project(gputils VERSION ${gputils_VERSION})
# need to run configure_file twice
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/src/gputils/version.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/include/gputils/version.h")
configure_file( "${CMAKE_CURRENT_BINARY_DIR}/include/gputils/version.h"
                "${CMAKE_CURRENT_BINARY_DIR}/include/gputils/version.h")

# common dependencies
find_package( Boost COMPONENTS filesystem REQUIRED )
find_package( hdf5 REQUIRED )
find_package( libInterpolate REQUIRED )

string( REGEX REPLACE "^lib" "" LIB_NAME ${PROJECT_NAME} )
add_library( ${LIB_NAME} )
add_library( ${PROJECT_NAME}::${LIB_NAME} ALIAS ${LIB_NAME} )
target_sources(
${LIB_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/gputils/io.cpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/gputils/transformations.cpp>
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/gputils/io.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/gputils/structs.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/gputils/transformations.hpp>
)
target_include_directories( ${LIB_NAME} INTERFACE
  $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(${LIB_NAME}
                        PUBLIC
                          Boost::boost
                          Boost::filesystem
                          libInterpolate::Interpolate
                          hdf5::hdf5_cpp-static
                     )

if( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} AND BUILD_UNIT_TESTS)
enable_testing()
add_subdirectory( testing )
endif()






if( INSTALL_LIBRARY )

# Install and Packaging
install( TARGETS ${LIB_NAME} EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(DIRECTORY src/
  DESTINATION include
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
  DESTINATION include
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION cmake
)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
"include(CMakeFindDependencyMacro)
find_package( Boost COMPONENTS filesystem )
find_package( hdf5 )
find_package( libInterpolate )
include(\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Targets.cmake)
"
  )
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${${PROJECT_NAME}_VERSION}
  COMPATIBILITY SameMajorVersion
  )
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION cmake
  )




INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "")
SET(CPACK_PACKAGE_VENDOR "")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")
if( ${PROJECT_NAME}_VERSION_MAJOR)
  SET(CPACK_PACKAGE_VERSION_MAJOR ${${PROJECT_NAME}_VERSION_MAJOR})
endif()
if( ${PROJECT_NAME}_VERSION_MINOR)
  SET(CPACK_PACKAGE_VERSION_MINOR ${${PROJECT_NAME}_VERSION_MINOR})
endif()
if( ${PROJECT_NAME}_VERSION_PATCH)
  SET(CPACK_PACKAGE_VERSION_PATCH ${${PROJECT_NAME}_VERSION_PATCH})
endif()
INCLUDE(CPack)

endif()


