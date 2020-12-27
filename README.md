# Visible Polygon Estimator

## Setup

Conda is a helpful utility to manage environments on multiple platforms including macos.
Install [Conda on MacOS](https://docs.conda.io/projects/conda/en/latest/user-guide/install/macos.html)

1. Create new environment (to be performed only once)

```shell
conda create --name vpe
```

2. Activate the conda environment (to be performed in each new shell)

```shell
conda activate vpe
```

Set `CMAKE_PREFIX_PATH` to `CONDA_PREFIX` in the shell as well as IDE to using the libraries installed in conda env

3. Install essential tools and dependencies

Install essential tools and dependencies for librealsense2 (version: 2.39.0) using the
[guide](https://github.com/IntelRealSense/librealsense/blob/v2.39.0/doc/installation_osx.md)
provided by realsense. Prefer install tools and dependencies into a clean environment using conda.

cmake: using conda

```shell
conda install -c anaconda cmake
```

libusb: using conda

```shell
conda install -c conda-forge libusb
```

pkg-config: using conda

```shell
conda install -c conda-forge pkg-config
```

4. Clone and build third party dependencies

apriltag (version 3.13.0): build from source

```shell
git submodule add https://github.com/AprilRobotics/apriltag.git third-party/apriltag
git checkout v3.1.3
```

Commented out line 64-95 in `apriltag/CMakeLists.txt` to ignore python wrappers

```shell
cmake -DCMAKE_PREFIX_PATH=$CONDA_PREFIX -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX -S . -B build
cmake --build build --target install
```

librealsense2 (version: 2.39.0): build from source using the
[guide](https://github.com/IntelRealSense/librealsense/blob/v2.39.0/doc/installation_osx.md)
provided by realsense

Clone and checkout tag v2.39

```shell
git submodule add https://github.com/IntelRealSense/librealsense.git third-party/librealsense
cd third-party/librealsense
git checkout v2.39.0
cmake -DCMAKE_PREFIX_PATH=$CONDA_PREFIX -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX -S . -B build
cmake --build build --target install
cd ${CONDA_PREFIX}/lib
install_name_tool -change libusb-1.0.0.dylib @rpath/libusb-1.0.0.dylib librealsense2.dylib
```

Set `DYLD_LIBRARY_PATH` and `PATH` to include the right libraries into path. Run the following script after activating
the conda env

```shell
source macos_env.sh
```

## Build

Make sure to configure IDE and shell to use appropriate `CMAKE_PREFIX_PATH` `DYLD_LIBRARY_PATH` and `PATH`. Build using
CMake.

```shell
conda activate vpe
source macos_env.sh
cmake -DCMAKE_PREFIX_PATH=$CONDA_PREFIX -S . -B build
cmake --build build
```