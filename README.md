### Requirements
- cmake  
- vcpkg

Set cmake_toolchain_file as environment variable.

In my case,
```
$> echo $Env:cmake_toolchain_file
C:\Users\hjeong\dev\tool\vcpkg\scripts\buildsystems\vcpkg.cmake
```

### Build and execute source code (Windows)
```shell
cd pha_hw
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="$Env:cmake_toolchain_file" -S ../ -B ./
cmake --build .
cd HW{number}/Debug
./HW{number}.exe
```

### Run executable file directly without build (Windows)
- HW3: pha_hw/release/HW3/HW3.exe