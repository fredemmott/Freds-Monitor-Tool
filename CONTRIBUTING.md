This project can be built using CMake, with Visual Studio 2022; it is primarily developed with CLang, but can also be built with MSVC.

# Building

1. Install Visual Studio 2022, including C++ support. The community edition is fine
2. Check out this repository
3. `git submodule update --init --recursive`
4. `mkdir build`
5. `cd build`
6. `cmake ..`
7. `cmake --build . --config Debug`

You can replace steps 4-7 with your favorite CMake-and-C++ workflow, e.g. Visual Studio Code's CMake support.