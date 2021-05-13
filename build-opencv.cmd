
rem run this as elevated powershell
rem Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

rem close the powershell / open elevated powershell or cmd and run all this cmds

rem choco install -y 7zip
rem choco install -y git
rem choco install -y cmake.install --installargs '"ADD_CMAKE_TO_PATH=System"'
rem choco install -y python3
rem choco install -y visualstudio2019community
rem choco install -y visualstudio2019-workload-nativedesktop
rem
rem ^^^ you might need to reboot Windows at this point

mkdir opencv
cd opencv

git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git

cd opencv
git pull
cd ..
cd opencv_contrib
git pull
cd ..

mkdir build
cd build

cmake -D CMAKE_BUILD_TYPE=Release ^
      -D BUILD_SHARED_LIBS=OFF ^
      -D OPENCV_EXTRA_MODULES_PATH=..\opencv_contrib\modules ^
      -D BUILD_TESTS=OFF ^
      -D BUILD_EXAMPLES=OFF ^
      -D BUILD_DOCS=OFF ^
      -D INSTALL_PYTHON_EXAMPLES=OFF ^
      -D BUILD_PERF_TESTS=OFF ^
      -D OPENCV_ENABLE_NONFREE=ON ^
      -D INSTALL_CREATE_DISTRIB=ON ^
      -D BUILD_opencv_mcc=ON ^
      ..\opencv

cmake --build . --config debug --parallel 20
cmake --build . --config release --parallel 20

cmake --build . --target install --config debug --parallel 20
cmake --build . --target install --config release --parallel 20

cd install

7z a ..\opencv.zip . -xr!*.pdb

rem mkdir C:\tools
rem copy ..\opencv.zip C:\tools
rem cd C:\tools
rem 7z x opencv.zip -oopencv

rem create an environment variable "OPENCV_DIR=C:\tools\opencv"
rem add %OPENCV_DIR%\x64\vc16\bin to %PATH%