### Contents
- HW1: Rendering Obj Files using OpenGL
- HW2: ODE solver for a spring-damper system (a.k.a. Harmonic oscillator)
- HW3: Bead on the Wire (constraint force)
- HW4: Collision Detection between Spheres
- HW5: Rigid Body Dynamics: Part 1 Colliding Contact
- HW6: Rigid Body Dynamics Part 2: Resting Contact

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
cd HW#\Debug
.\HW#.exe
```

### Run executable file directly without build (Windows)
- pha_hw\executable\HW#.exe
