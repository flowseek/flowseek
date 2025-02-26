#!/bin/sh

current_dir=$PWD
echo $current_dir

if [ "$1" == "check" ]; then

    git submodule update --init --recursive

    cd $current_dir/lib/libpcap
    git checkout 2852d15aeee0a493f01b82ac163d7510441bfaac
    
    cd $current_dir/lib/nDPI
    git checkout 294b7e18897d4c4cdf2f584dabb87b37015b2337
    
    cd $current_dir/lib/tensorflow
    git checkout e41890cca91372308073c3d74c8b15993e17b632

    cd $current_dir
    git submodule status

    exit 1
fi

if [ "$1" == "lib" ]; then

    cd $current_dir/lib/libpcap
    ./autogen.sh 
    ./configure --disable-shared --disable-dbus  --without-libnl
    make
    
    cd $current_dir/lib/nDPI
    ./autogen.sh --with-only-libndpi
    ./configure 
    make
    
    exit 1
fi

if [ "$1" == "make" ]; then

    cd $current_dir/httpserver
    make
    
    cd $current_dir/
    make
    
    exit 1
fi

if [ "$1" == "clean" ]; then

    cd $current_dir/httpserver
    make clean
    
    cd $current_dir/
    make clean

    exit 1
fi
