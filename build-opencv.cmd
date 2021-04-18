
rem Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

rem choco install -y 7zip

rem choco install -y git

rem choco install -y cmake.install --installargs '"ADD_CMAKE_TO_PATH=System"'

rem choco install -y python3

rem choco install -y visualstudio2019community

rem choco install -y visualstudio2019-workload-nativedesktop

git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git

mkdir build
cd build

cmake -D CMAKE_BUILD_TYPE=Release ^
      -D OPENCV_EXTRA_MODULES_PATH=..\opencv_contrib\modules ^
      -D BUILD_TESTS=OFF ^
      -D BUILD_EXAMPLES=OFF ^
      -D INSTALL_PYTHON_EXAMPLES=OFF ^
      -D OPENCV_ENABLE_NONFREE=ON ^
      ..\opencv

cmake --build . --config release
cmake --build . --target install --config release

cd install

7z a ..\opencv-windows-dlls-x64.zip . -xr!*.pdb
