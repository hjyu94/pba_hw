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
cd HW5\Debug
.\HW5.exe
```

### Run executable file directly without build (Windows)
- HW5: pha_hw\executable\HW5\HW5.exe
