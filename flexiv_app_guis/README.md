Flexiv App Guis
===============

This is a repository contains different UI applications and rdk programs that use flexiv_middleware2 as their communication infrastructure. 

How to build this repo
----------------------------------------------

1. First, make sure you install qt5 and spdlog with:

        sudo apt-get install qt5-default
        sudo apt-get install libspdlog-dev
    (Optional) If you need to use QT creator, you can install qt5 creator with:

        sudo apt-get install qtcreator

2. Second, make sure you build and install **flexiv_thirdparty2** in your environment:

        export flexiv_thirdparty2_path=<absolute path to flexiv_thirdparty2>

3. Build with default shell script: build_for_linux.sh:

        bash build_for_linux.sh
    There are some options in CMakeLists.txt to select which applications you want to build, the default is ON.

How to create messages that are ROS compatible
----------------------------------------------

It is possible to skip step 1-5, and write the .idl directly.

1. [Install ROS 2 via Debian Packages](https://docs.ros.org/en/foxy/Installation/Linux-Install-Debians.html)
2. Create .msg files, that describes the message data. The .msg file format is defined by ROS.
3. Create a package.xml file in the same directory as the .msg files. An example can be found at [ros_app/src/pub_sub_msgs/package.xml](ros_app/src/pub_sub_msgs/package.xml)
4. Generate a .idl file from the .msg file. The .idl file format is defined by the DDS Standard.

        ros2 run rosidl_adapter msg2idl.py <msg_dir>/*.msg

5. Manually edit the .idl, to remove typdef arrays types. 
    This is a ROS bug, that causes redefinition errors when running fastrtpsgen.
    For example, replace: 

        typedef float float__7[7];
        float__7 my_array;

    with:

        float my_array[7];

6. Generate a .h and .cxx file from the .idl file. These are specific to the DDS implementation.

        fastddsgen -typeros2 <idl_dir>/*.idl -d <output_dir> -replace

    In your project's CMakeLists.txt, you can the fastrtpsgen cmake function, defined by find_package(flexiv_middleware2)

        flexiv_middlewar2_fastrtpsgen(idl_files include_directories output_directory generated_header generated_source)

flexiv_middleware2 automatically adds a "rt/" prefix to all topic names, to make them ros2 compatible.
See [topic and service names](https://design.ros2.org/articles/topic_and_service_names.html).
