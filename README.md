# OverEngine
## Opensource game engine under MIT license

![linux-build](https://github.com/OverShifted/OverEngine/workflows/linux-build/badge.svg)
![windows-build](https://github.com/OverShifted/OverEngine/workflows/windows-build/badge.svg)

![Logo](https://repository-images.githubusercontent.com/246120634/f6e9f080-a288-11ea-9a12-502646520a32)

## Game Editor
* Work in progress 🚧
* Drag and drop support
* Asset management
* ...

### New:
![Screenshot 2020-11-10 143415](https://user-images.githubusercontent.com/53635046/98666307-16ba3000-2362-11eb-9b85-fa64c0153776.png)
### Old:
![OverEditorScreenShot](https://user-images.githubusercontent.com/53635046/93054565-c20a7980-f67e-11ea-89f5-d72a1e0a315c.PNG)

## Standalone OverEngine Application (Sandbox Demo)
![Sandbox Demo](https://user-images.githubusercontent.com/53635046/98666301-1326a900-2362-11eb-9231-e6509e95d573.png)
![Sandbox Demo](https://user-images.githubusercontent.com/53635046/98665068-68fa5180-2360-11eb-88be-da2482cbb210.png)

## Used third-party libraries and tools:
* [premake](https://github.com/premake/premake-core) Project file generator
* [CMake](https://cmake.org) Build system
* [spdlog](https://github.com/gabime/spdlog) Logger
* [GLFW](https://github.com/glfw/GLFW) Windowing
* [Glad2](https://gen.glad.sh/) OpenGL loader
* [DearImGui](https://github.com/ocornut/imgui) GUI library
* [glm](https://github.com/g-truc/glm) Math library
* [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) Image loader
* [stb_rect_pack](https://github.com/nothings/stb/blob/master/stb_rect_pack.h) Rectangle packer
* [EnTT](https://github.com/skypjack/entt) Entity Component System (ECS)
* [box2d](https://github.com/erincatto/box2d) 2D physics library
* [yaml-cpp](https://github.com/jbeder/yaml-cpp) YAML parser and emitter

## Build Instructions
this is my own ideal way to develop OverEngine:
### On Windows
On Windows, I use `GenerateProjectFiles.bat` to generate project files for Visual Studio 2019 using Premake. Premake's VS project generator is fast and clean.

### On Linux
On Linux I prefer using `CMake` because it is just better and cleaner than `premake`'s `gmake2` generator.
You can use any generator but `Ninja` and `Unix Makefiles` generators are my favorite ones.
```bash
# we are in project's root directory
# 'mkdir build' is not needed
# run this command:
cmake -S . -B build -G Ninja
# or this one (not both)
cmake -S . -B build -G "Unix Makefiles"

# To build
cmake --build build
# or (not both)
cd build
ninja
```

## IDE / Build tool - platform - compiler support
* Visual Studio 2019 - Windows - MSVC
* CMake (with make or ninja) - Linux - GCC / G++

## How to contribute
1. Create a fork using the button in the top right corner
2. Clone your fork on your computer
3. Apply your modifications on your local clone. You can search the codebase for `TODO:`s and `FIXME:`s to find "help-wanted" parts. You can also try to fix bugs that you've found.
4. Commit and push your changes to your own fork
5. Apply a pull request to `OverShifted/OverEngine` and wait for your request to be accepted (or rejected)
