# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/victormuryn/esp/esp-idf/components/bootloader/subproject"
  "/Users/victormuryn/Desktop/ucu/poc/esp32-detect-move/cmake-build-debug-esp-idf/bootloader"
  "/Users/victormuryn/Desktop/ucu/poc/esp32-detect-move/cmake-build-debug-esp-idf/bootloader-prefix"
  "/Users/victormuryn/Desktop/ucu/poc/esp32-detect-move/cmake-build-debug-esp-idf/bootloader-prefix/tmp"
  "/Users/victormuryn/Desktop/ucu/poc/esp32-detect-move/cmake-build-debug-esp-idf/bootloader-prefix/src/bootloader-stamp"
  "/Users/victormuryn/Desktop/ucu/poc/esp32-detect-move/cmake-build-debug-esp-idf/bootloader-prefix/src"
  "/Users/victormuryn/Desktop/ucu/poc/esp32-detect-move/cmake-build-debug-esp-idf/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/victormuryn/Desktop/ucu/poc/esp32-detect-move/cmake-build-debug-esp-idf/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/victormuryn/Desktop/ucu/poc/esp32-detect-move/cmake-build-debug-esp-idf/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
