#!/bin/bash

# Dockerize me!

set -x

#idea: https://learnopencv.com/install-opencv-on-android-tiny-and-optimized/
#idea: https://amin-ahmadi.com/2019/02/03/how-to-build-opencv-4-x-for-native-android-development/

#idea: https://gist.github.com/david-serrano/359b4ca3df2aeb31eca224990ba486f2


sudo apt-get -y update
sudo apt-get install -y ninja-build ant
sudo apt-get -y install cmake

export ANDROID_SDK_NDK_DIR="$HOME/Android/Sdk/ndk/22.1.7171670"
export ANDROID_STANDALONE_TOOLCHAIN=/tmp/my-android-toolchain/

# # we use the prebuild toolchain
# export ANDROID_STANDALONE_TOOLCHAIN=${ANDROID_SDK_NDK_DIR}/toolchains/arm-linux-androideabi-4.9
# unset ANDROID_NDK

if [ ! -d ${ANDROID_STANDALONE_TOOLCHAIN} ]; then
    ${ANDROID_SDK_NDK_DIR}/build/tools/make_standalone_toolchain.py \
    --arch arm  \
    --api 24  \
    --install-dir ${ANDROID_STANDALONE_TOOLCHAIN}
fi

git clone https://github.com/opencv/opencv.git || echo "already there?"
git clone https://github.com/opencv/opencv_contrib.git || echo "already there?"

cd opencv
git pull
cd ..
cd opencv_contrib
git pull
cd ..

mkdir -p build-arm
cd build-arm

cmake -D CMAKE_BUILD_TYPE=Release \
    -D OPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules \
    -D CMAKE_TOOLCHAIN_FILE=../opencv/platforms/android/android.toolchain.cmake \
    -D BUILD_TESTS=OFF \
    -D BUILD_EXAMPLES=OFF \
    -D OPENCV_ENABLE_NONFREE=ON \
    -D ANDROID_STL=gnustl_shared \
    -D ANDROID_NATIVE_API_LEVEL=24 \
    ../opencv

make -j20
