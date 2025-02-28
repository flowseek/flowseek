#
# Copyright 2020 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


# Builds the detect_cnn Tensorflow Lite example.

cmake_minimum_required(VERSION 3.16)

project(deepdetect C CXX)

set(TENSORFLOW_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../../../lib/tensorflow")
add_subdirectory(
  "${TENSORFLOW_SOURCE_DIR}/tensorflow/lite"
  "${CMAKE_CURRENT_BINARY_DIR}/build" EXCLUDE_FROM_ALL)

# 使用 pkg-config 获取 OpenCV 4 的编译标志
execute_process(COMMAND pkg-config --cflags opencv4
                OUTPUT_VARIABLE OPENCV_CFLAGS
                OUTPUT_STRIP_TRAILING_WHITESPACE)
# 使用 pkg-config 获取 OpenCV 4 的链接选项
execute_process(COMMAND pkg-config --libs opencv4
                OUTPUT_VARIABLE OPENCV_LIBS
                OUTPUT_STRIP_TRAILING_WHITESPACE)

# 添加SQLite3源文件
set(SQLITE3_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../../../lib/sqlite3")
set(SQLITE3_SOURCES 
    ${SQLITE3_SOURCE_DIR}/sqlite3.c
    ${SQLITE3_SOURCE_DIR}/sqlite3.h
)

add_executable(detect_cnn deepdetect.cc ${SQLITE3_SOURCES})

# 将编译标志传递给目标
target_compile_options(detect_cnn PRIVATE ${OPENCV_CFLAGS})
target_link_libraries(detect_cnn tensorflow-lite ${OPENCV_LIBS})
