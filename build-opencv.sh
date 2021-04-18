#!/bin/bash

set -x

# Dockerize me!

IS_RASPBERRY=$(grep Pi /proc/device-tree/model 2>/dev/null && echo 1)


sudo apt-get -y update
# compiler and build tools
sudo apt-get -y install git build-essential cmake pkg-config checkinstall
# development files for system wide image codecs
sudo apt-get -y install libjpeg-dev libpng-dev libtiff-dev
# Protobuf library and tools for dnn module
sudo apt-get -y install libprotobuf-dev protobuf-compiler
# development files for V4L2 to enable web cameras support in videoio module
sudo apt-get -y install libv4l-dev
# FFmpeg development files to enable video decoding and encoding in videoio module
sudo apt-get -y install libavcodec-dev libavformat-dev libswscale-dev
# development files for GTK 2.0 UI library for highgui module
sudo apt-get -y install libgtk2.0-dev
# Eigen library needed for some modules in contrib repository
sudo apt-get -y install libeigen3-dev
# Numpy and Python3 development files for Python bindings
sudo apt-get -y install python3-dev python3-pip
sudo -H pip3 install numpy

sudo apt-get -y install cmake

git clone https://github.com/giuliomoro/checkinstall
cd checkinstall
sudo make install
cd ..
sudo rm -rf checkinstall

git clone https://github.com/opencv/opencv.git || echo "already there?"
git clone https://github.com/opencv/opencv_contrib.git || echo "already there?"

mkdir -p build
cd build

if [ ! -z "$IS_RASPBERRY" ]
then
	echo "On Raspberry"
	export PI_OPTIONS="-D ENABLE_NEON=ON -D ENABLE_VFPV3=ON -D CMAKE_SHARED_LINKER_FLAGS=-latomic"
else
	export PI_OPTIONS=""
fi

cmake -D CMAKE_BUILD_TYPE=Release \
    -D OPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules \
    -D BUILD_TESTS=OFF \
    -D BUILD_EXAMPLES=OFF \
    -D INSTALL_PYTHON_EXAMPLES=OFF \
    -D BUILD_OPENCV_JAVA=OFF \
    -D OPENCV_ENABLE_NONFREE=ON \
    ${PI_OPTIONS} \
     ../opencv

if [ ! -z "$IS_RASPBERRY" ]
then
	echo "On Raspberry"
	make -j$(nproc)
else
	# it's 2021..
	make -j20
fi

echo "Self build OpenCV (with Contrib/Non-Free)" > description-pak

# add depends

sudo checkinstall \
        -y \
        --type="debian" \
        --pkgname="opencv"\
        --pkgsource="opencv-build"\
        --provides="opencv" \
	--nodoc \
	--deldoc=yes \
	--exclude=doc \
	--exclude=./doc \
        make install
