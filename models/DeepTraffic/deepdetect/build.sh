#!/bin/sh

current_dir=$PWD
echo $current_dir

if [ "$1" == "tflite" ]; then

    #https://www.tensorflow.org/lite/guide/build_cmake?hl=zh-cn
    mkdir tflite_build
    cd tflite_build
    cmake ../../../../lib/tensorflow/tensorflow/lite/c -DTFLITE_C_BUILD_SHARED_LIBS:BOOL=OFF -DBUILD_PROTOBUF:BOOL=OFF
    cmake --build . -j
    
    exit 1
fi

if [ "$1" == "make" ]; then

    cd $current_dir/
    make
    
    exit 1
fi

if [ "$1" == "clean" ]; then

    cd $current_dir/
    make clean

    exit 1
fi
