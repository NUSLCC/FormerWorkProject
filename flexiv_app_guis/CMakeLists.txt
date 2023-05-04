cmake_minimum_required(VERSION 3.5)

# ===================================================================
# PROJECT SETUP
# ===================================================================
project(flexiv_app_guis)

# ===================================================================
# PROJECT OPTIONS FOR APP
# ===================================================================
option(BUILD_APP_KOSTAL_QT_GUI "Build kostal gui apps" ON)
option(BUILD_APP_RDK_CLIENT "Build rdk client apps" ON)

# set default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)

# ===================================================================
# PROJECT CONFIG
# ===================================================================
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/tools/set_default_build_type.cmake)
set_default_build_type()

# create a header file that defines the library version and git version
# this will replace @varable@ with the cmake value ${variable}
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/tools/FlexivGitVersion.cmake)
FlexivGitVersion()

# ===================================================================
# PROJECT DEPENDENCIES
# ===================================================================
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED)

# ===================================================================
# INCLUDE MIDDLEWARE2 CMAKE FUNCTIONS
# ===================================================================
# Both add_subdirectory(flexiv_middleware2) and find_package(flexiv_middleware2)
# will include flexiv_middleware2_fastrtpsgen.cmake
include(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/flexiv_middleware2/cmake/flexiv_middleware2_fastrtpsgen.cmake)

# ===================================================================
# PROJECT APPS
# ===================================================================
# Add all app-level CMake projects as subdirectories,
if(${BUILD_APP_KOSTAL_QT_GUI})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app/kostal_qt_gui)
endif()

if(${BUILD_APP_RDK_CLIENT})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app/rdk_client)
endif()