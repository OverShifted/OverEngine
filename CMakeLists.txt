cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
project(OverEngine)

# vendor
add_subdirectory(OverEngine/vendor/GLFW)
add_subdirectory(OverEngine/vendor/Glad)
add_subdirectory(OverEngine/vendor/imgui)
add_subdirectory(OverEngine/vendor/lua)
set(BOX2D_BUILD_TESTBED 0)
add_subdirectory(OverEngine/vendor/box2d)

# OverEngine
add_subdirectory(OverEngine)
add_subdirectory(OverEditor)
# add_subdirectory(Sandbox)