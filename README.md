# OverEngine
## Opensource game engine under MIT license

![Logo](https://repository-images.githubusercontent.com/246120634/f6e9f080-a288-11ea-9a12-502646520a32)

## WIP Editor powered by [DearImGui](https://github.com/ocornut/imgui)
* WIP drag and drop support
* WIP Resource management
* ...

![OverEditorScreenShot](https://user-images.githubusercontent.com/53635046/93054565-c20a7980-f67e-11ea-89f5-d72a1e0a315c.PNG)

## Cool things in use:
* [premake](https://github.com/premake/premake-core)
* [CMake](https://cmake.org)
* [spdlog](https://github.com/gabime/spdlog)
* [GLFW](https://github.com/glfw/GLFW)
* [Glad2](https://gen.glad.sh/)
* [DearImGui](https://github.com/ocornut/imgui)
* [glm](https://github.com/g-truc/glm)
* [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
* [stb_rect_pack](https://github.com/nothings/stb/blob/master/stb_rect_pack.h)
* [EnTT](https://github.com/skypjack/entt)
* [box2d](https://github.com/erincatto/box2d)

## Setup development environment for OverEngine
this is my own ideal way to develop OverEngine:
### On Windows
On Windows, I use `GenerateProjectFiles.bat` to generate project files for Visual Studio 2019 using Premake. Premake's VS project generator is fast and clean.

### On Linux
On Linux on the other hand; Preamke's `gmake2` generator is just trash. it's dependency manager is garbage and build's are too slow because of `make`.
I use `CMake`'s `ninja` generator to build. Ninja it super fast and uses all cpu core's. You can use make but its slow.
```bash
# we are in project's root directory
# 'mkdir build' is not needed
cmake -S . -B build -G Ninja

# To build
cmake --build build
# or
cd build
ninja
```

## IDE / Build tool - platform - compiler support
* Visual Studio 2019 - Windows - MSVC
* CMake (with make or ninja) - Linux - GCC / G++

![WindowsBuilt](https://img.shields.io/badge/Windows-built-green?logo=windows)
![LinuxBuilt](https://img.shields.io/badge/Linux-built-green?logo=Linux)
