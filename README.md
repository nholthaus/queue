# Queue
[![Build Status](https://travis-ci.org/nholthaus/queue.svg?branch=main)](https://travis-ci.org/github/nholthaus/queue) [![Build status](https://ci.appveyor.com/api/projects/status/dfoks5b44qsgf7ka/branch/master?svg=true)](https://ci.appveyor.com/project/nholthaus/queue/branch/master) ![language](https://img.shields.io/badge/language-c++17-blue.svg) ![license](https://img.shields.io/badge/license-MIT-orange.svg) ![copyright](https://img.shields.io/badge/%C2%A9-Nic_Holthaus-orange.svg) ![msvc2019](https://img.shields.io/badge/MSVC-2019-ff69b4.svg) ![gcc-9.3.0](https://img.shields.io/badge/GCC-9.3.0-ff69b4.svg)

A modern C++ header-only library of various types of queue

## Build Instructions

To build and run the unit tests:

``` bash
mkdir build
cd build
cmake -DBUILD_TESTS=ON ..
make -j
ctest
```

Each class in the library itself is a single header only. You can use the included CMake in a subdirectory and add the interface library, or just copy the `*.h` files (and license) into your project.

## Usage

TODO