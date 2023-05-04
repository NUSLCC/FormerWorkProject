# use cmake 3 which supports targets
cmake_minimum_required(VERSION 3.5)

# ===================================================================
# PROJECT SETUP
# ===================================================================
project(rdk_client VERSION)
set(CMAKE_VERBOSE_MAKEFILE ON)

# ===================================================================
# PROJECT DEPENDENCIES
# ===================================================================
# find_package(Threads REQUIRED)

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
# PROJECT EXECUTABLES
# ===================================================================
# add executables
add_executable(robot_states_publisher src/robot_states_publisher.cpp ${GENERATED_IDL_GUI_SOURCE})

# add path to idl header files and flexiv rdk header files
target_include_directories(robot_states_publisher
    PUBLIC
    ${GENERATED_IDL_GUI_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/flexiv_rdk/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/flexiv_middleware2/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# find and link dependencies
target_link_libraries(robot_states_publisher
    flexiv::flexiv_middleware2
    ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/flexiv_rdk/lib/libFlexivRdk.x86_64-linux-gnu.a
)

# ===================================================================
# CMAKE COMPILE CONFIG
# ===================================================================
# target_compile_features(kostal_qt_gui PRIVATE cxx_std_14)
# target_compile_options(kostal_qt_gui PRIVATE -Werror)