# Kostal Project
## Kostal

**NOTE:** This is a private project for kostal, please refer to chenchen.liu@flexiv.com or jiajun.tian@flexiv.com for more details.
## Flexiv RDK

![CMake Badge](https://github.com/flexivrobotics/flexiv_rdk/actions/workflows/cmake.yml/badge.svg)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

Flexiv RDK (Robot Development Kit) is a powerful toolkit as an add-on to the Flexiv software platform. It enables the users to create complex applications with APIs that provide low-level real-time access to the robot.

## License

Flexiv RDK is licensed under the [Apache 2.0 license](https://www.apache.org/licenses/LICENSE-2.0.html).

## References

**[Flexiv RDK Manual](https://flexivrobotics.github.io/)** is the main reference and contains important information on how to properly set up the system and use Flexiv RDK library.

[API documentation](https://flexivrobotics.github.io/flexiv_rdk/) contains details about available APIs, generated from Doxygen.

## Run example programs

**NOTE:** the instruction below is only a quick reference, assuming you've already gone through the Flexiv RDK Manual.

### C++ interface

1. Configure and build example programs:

        cd flexiv_rdk
        mkdir build && cd build
        cmake ..
        make -j4