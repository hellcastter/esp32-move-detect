# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# added
set(CMAKE_CXX_STANDARD 17)


#idf_build_set_property(COMPILE_OPTIONS  APPEND)
#message($ENV{IDF_PATH}/tools/cmake/project.cmake)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
include_directories(./components)
project(main)
