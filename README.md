# OverEngine
Tiny little game engine

![linux-build](https://github.com/OverShifted/OverEngine/workflows/linux-build/badge.svg)
![windows-build](https://github.com/OverShifted/OverEngine/workflows/windows-build/badge.svg)

<!-- ![Logo](https://repository-images.githubusercontent.com/246120634/f6e9f080-a288-11ea-9a12-502646520a32) -->
<!-- ![Demo](https://user-images.githubusercontent.com/53635046/212471698-c1f55569-1f98-4c1b-b29d-0e8fa02a00af.png) -->
![Demo](https://user-images.githubusercontent.com/53635046/222447282-e7c00296-1a72-469d-b8db-61c08aab3063.png)

## Build Instructions
### Windows
Run `GenerateProjectFiles.bat` and open the generated solution file in Visual Studio 2022

### Linux
Build with CMake:
```sh
git clone https://github.com/OverShifted/OverEngine
cd OverEngine
cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build
```
## Credits
* [spdlog](https://github.com/gabime/spdlog)
* [GLFW](https://github.com/glfw/GLFW)
* [Glad2](https://gen.glad.sh/)
* [DearImGui](https://github.com/ocornut/imgui)
* [glm](https://github.com/g-truc/glm)
* [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
* [EnTT](https://github.com/skypjack/entt)
* [box2d](https://github.com/erincatto/box2d)
* [yaml-cpp](https://github.com/jbeder/yaml-cpp)
* [wren](https://github.com/wren-lang/wren)
* [wrenpp](https://github.com/Nelarius/wrenpp)

Contributions are welcome.

