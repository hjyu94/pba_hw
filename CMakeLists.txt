# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("PBA_HW")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include sub-projects.
# add_subdirectory ("HW1")
add_subdirectory ("HW2")
add_subdirectory ("HW3")
add_subdirectory ("HW4")
add_subdirectory ("HW5")
add_subdirectory ("HW6")
