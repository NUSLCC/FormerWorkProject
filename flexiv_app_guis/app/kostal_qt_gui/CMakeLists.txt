# use cmake 3 which supports targets
cmake_minimum_required(VERSION 3.5)

# ===================================================================
# PROJECT SETUP
# ===================================================================
project(kostal_qt_gui VERSION)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ===================================================================
# MIDDLEWARE2 IDL GENERATION
# ===================================================================
# if you build this script solely, please uncomment the following
# include(${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/flexiv_middleware2/cmake/flexiv_middleware2_fastrtpsgen.cmake)
find_package(flexiv_middleware2 REQUIRED)
flexiv_middleware2_fastrtpsgen(${CMAKE_CURRENT_SOURCE_DIR}/../../idl
    gui_msgs/msg
    ${CMAKE_CURRENT_BINARY_DIR}/generated
    GENERATED_IDL_GUI_HEADER
    GENERATED_IDL_GUI_SOURCE
    GENERATED_IDL_GUI_INCLUDE_DIRECTORIES)

# ===================================================================
# PROJECT DEPENDENCIES
# ===================================================================
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# ===================================================================
# PROJECT EXECUTABLES
# ===================================================================
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src QT_SOURCE)
set(ALL_GUI_SOURCE ${QT_SOURCE} ${GENERATED_IDL_GUI_SOURCE})
qt5_add_resources(ALL_GUI_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/qt_flexiv_theme/resources.qrc)
add_executable(kostal_qt_gui ${ALL_GUI_SOURCE})
target_include_directories(kostal_qt_gui
    PUBLIC
    ${GENERATED_IDL_GUI_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/flexiv_middleware2/include
)
target_link_libraries(kostal_qt_gui
    flexiv::flexiv_middleware2
    Qt5::Widgets
)

# ===================================================================
# CMAKE COMPILE CONFIG
# ===================================================================
# target_compile_features(kostal_qt_gui PRIVATE cxx_std_14)
# target_compile_options(kostal_qt_gui PRIVATE -Werror)