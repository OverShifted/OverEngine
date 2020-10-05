cmake_minimum_required(VERSION 3.16)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_CXX_STANDARD 17)
project(OverEngine
        VERSION 0.0
        DESCRIPTION "Opensource game engine under MIT license"
        LANGUAGES C CXX)

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
add_subdirectory(Sandbox)
