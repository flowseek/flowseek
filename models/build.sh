#!/bin/sh
#bazel build --config=elinux_aarch64 -c opt //tensorflow/lite/c:libtensorflowlite_c.so
#bazel build --config=elinux_aarch64 -c opt //tensorflow/lite:libtensorflowlite.so

current_dir=$PWD
echo $current_dir

if [ "$1" == "build" ]; then

    #https://www.tensorflow.org/lite/guide/build_cmake?hl=zh-cn
    mkdir tflite_build
    cd tflite_build
    #cmake ../../../../lib/tensorflow/tensorflow/lite -DTFLITE_C_BUILD_SHARED_LIBS:BOOL=OFF -DTFLITE_ENABLE_XNNPACK=OFF -DTFLITE_ENABLE_RUY=OFF -DTFLITE_ENABLE_MMAP=OFF
    #ccmake ../../lib/tensorflow/tensorflow/lite -DTFLITE_C_BUILD_SHARED_LIBS:BOOL=OFF -DTFLITE_ENABLE_XNNPACK=OFF -DTFLITE_ENABLE_RUY=OFF -DTFLITE_ENABLE_GPU=OFF
    ccmake ../../lib/tensorflow/tensorflow/lite -DTFLITE_ENABLE_XNNPACK=OFF -DTFLITE_ENABLE_RUY=OFF -DTFLITE_ENABLE_GPU=OFF
    
    cmake --build . -j 6
    #cmake --build . --target clean

    exit 1
fi


if [ "$1" == "rebuild" ]; then

    cd $current_dir/tflite_build
    cmake --build build --clean-first

    exit 1
fi

if [ "$1" == "make" ]; then

    make -C $current_dir/DeepTraffic/deepdetect

    exit 1
fi

if [ "$1" == "clean" ]; then

    make -C $current_dir/DeepTraffic/deepdetect clean

    exit 1
fi

if [ "$1" == "run" ]; then

    export LD_LIBRARY_PATH=$current_dir/tflite_build:$current_dir/tflite_build/_deps/abseil-cpp-build/absl/base:$current_dir/tflite_build/_deps/abseil-cpp-build/absl/strings:$current_dir/tflite_build/_deps/abseil-cpp-build/absl/log:$LD_LIBRARY_PATH  
    cd $current_dir/DeepTraffic/deepdetect

    # 检查目录是否存在
    if [ ! -d "$2" ]; then
        echo "Directory $2 does not exist!"
        exit 1
    fi

    # 循环检测并处理flow开头的文件
    while true; do
        # 查找以flow开头的文件
        flow_files=$(find "$2" -type f -name "flow_*")
        
        if [ -n "$flow_files" ]; then
            # 如果找到flow开头的文件，执行推理
            ./x86_64-pc-linux-gnu-deepdetect "$2"
            # 等待1秒避免频繁检测
            sleep 1
        else
            # 如果没有找到文件，等待5秒后再次检测
            echo "No flow_* files found, waiting..."
            sleep 5
        fi
    done

    exit 1
fi
