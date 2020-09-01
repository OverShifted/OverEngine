set(OE_CROSS_PLATFORM_FILES
	"src/OverEngine.h"
	# "src/pcheader.cpp"
	"src/pcheader.h"
	
	"vendor/stb_image/stb_image.h"
	"vendor/stb_image/stb_image.cpp"

	"vendor/stb_rectpack/stb_rectpack.h"
	"vendor/stb_rectpack/stb_rectpack.cpp"

	"vendor/tinyfiledialogs/tinyfiledialogs.h"
	"vendor/tinyfiledialogs/tinyfiledialogsBuild.cpp"

	# .h
	"src/OverEngine/Assets/Asset.h"
	"src/OverEngine/Assets/AssetResource.h"
	"src/OverEngine/Core/Application.h"
	"src/OverEngine/Core/Core.h"
	"src/OverEngine/Core/EntryPoint.h"
	"src/OverEngine/Core/Init.h"
	"src/OverEngine/Core/Log.h"
	"src/OverEngine/Core/String.h"
	"src/OverEngine/Core/Window.h"
	"src/OverEngine/Core/FileSystem/FileSystem.h"
	"src/OverEngine/Core/FileSystem/VirtualFileSystem.h"
	"src/OverEngine/Core/Math/Color.h"
	"src/OverEngine/Core/Math/Math.h"
	"src/OverEngine/Core/Math/Transform.h"
	"src/OverEngine/Core/Memory/Allocator.h"
	"src/OverEngine/Core/Structs/List.h"
	"src/OverEngine/Core/Time/Time.h"
	"src/OverEngine/Core/Time/TimeStep.h"
	"src/OverEngine/Debug/Instrumentor.h"
	"src/OverEngine/Events/ApplicationEvent.h"
	"src/OverEngine/Events/Event.h"
	"src/OverEngine/Events/KeyEvent.h"
	"src/OverEngine/Events/MouseEvent.h"
	"src/OverEngine/ImGui/ExtraImGui.h"
	"src/OverEngine/ImGui/ImGuiBinding.h"
	"src/OverEngine/ImGui/ImGuiLayer.h"
	"src/OverEngine/Input/Input.h"
	"src/OverEngine/Input/InputSystem.h"
	"src/OverEngine/Input/KeyCodes.h"
	"src/OverEngine/Layers/Layer.h"
	"src/OverEngine/Layers/LayerStack.h"
	"src/OverEngine/Physics/PhysicsBody2D.h"
	"src/OverEngine/Physics/PhysicsCollider2D.h"
	"src/OverEngine/Physics/PhysicsMaterial2D.h"
	"src/OverEngine/Physics/PhysicsShape2D.h"
	"src/OverEngine/Physics/PhysicsWorld2D.h"
	"src/OverEngine/Renderer/Buffer.h"
	"src/OverEngine/Renderer/Camera.h"
	"src/OverEngine/Renderer/FrameBuffer.h"
	"src/OverEngine/Renderer/RenderCommand.h"
	"src/OverEngine/Renderer/Renderer.h"
	"src/OverEngine/Renderer/Renderer2D.h"
	"src/OverEngine/Renderer/RendererAPI.h"
	"src/OverEngine/Renderer/RendererContext.h"
	"src/OverEngine/Renderer/Shader.h"
	"src/OverEngine/Renderer/Texture.h"
	"src/OverEngine/Renderer/TextureEnums.h"
	"src/OverEngine/Renderer/TextureManager.h"
	"src/OverEngine/Renderer/VertexArray.h"
	"src/OverEngine/Renderer/GAPI/GTexture.h"
	"src/OverEngine/Scene/Components.h"
	"src/OverEngine/Scene/Entity.h"
	"src/OverEngine/Scene/Scene.h"
	"src/OverEngine/Scripting/Script.h"
	"src/OverEngine/Scripting/Lua/LuaApi.h"
	"src/OverEngine/Scripting/Lua/LuaScript.h"

	# .cpp
	"src/OverEngine/Assets/Asset.cpp"
	"src/OverEngine/Assets/AssetResource.cpp"
	"src/OverEngine/Core/Application.cpp"
	"src/OverEngine/Core/Init.cpp"
	"src/OverEngine/Core/Log.cpp"
	"src/OverEngine/Core/String.cpp"
	"src/OverEngine/Core/FileSystem/FileSystem.cpp"
	"src/OverEngine/Core/FileSystem/VirtualFileSystem.cpp"
	"src/OverEngine/Core/Math/Transform.cpp"
	"src/OverEngine/Core/Time/Time.cpp"
	"src/OverEngine/ImGui/ExtraImGui.cpp"
	"src/OverEngine/ImGui/ImGuiBinding.cpp"
	"src/OverEngine/ImGui/ImGuiLayer.cpp"
	"src/OverEngine/Input/InputSystem.cpp"
	"src/OverEngine/Layers/Layer.cpp"
	"src/OverEngine/Layers/LayerStack.cpp"
	"src/OverEngine/Physics/PhysicsBody2D.cpp"
	"src/OverEngine/Physics/PhysicsCollider2D.cpp"
	"src/OverEngine/Physics/PhysicsShape2D.cpp"
	"src/OverEngine/Physics/PhysicsWorld2D.cpp"
	"src/OverEngine/Renderer/Buffer.cpp"
	"src/OverEngine/Renderer/Camera.cpp"
	"src/OverEngine/Renderer/FrameBuffer.cpp"
	"src/OverEngine/Renderer/RenderCommand.cpp"
	"src/OverEngine/Renderer/Renderer.cpp"
	"src/OverEngine/Renderer/Renderer2D.cpp"
	"src/OverEngine/Renderer/RendererAPI.cpp"
	"src/OverEngine/Renderer/RendererContext.cpp"
	"src/OverEngine/Renderer/Shader.cpp"
	"src/OverEngine/Renderer/Texture.cpp"
	"src/OverEngine/Renderer/TextureManager.cpp"
	"src/OverEngine/Renderer/VertexArray.cpp"
	"src/OverEngine/Renderer/GAPI/GTexture.cpp"
	"src/OverEngine/Scene/Components.cpp"
	"src/OverEngine/Scene/Entity.cpp"
	"src/OverEngine/Scene/Scene.cpp"
	"src/OverEngine/Scripting/Lua/LuaApi.cpp"
	"src/OverEngine/Scripting/Lua/LuaScript.cpp"
)

set(OE_OGL_FILES
	# .h
	"src/Platform/OpenGL/OpenGLBuffer.h"
	"src/Platform/OpenGL/OpenGLContext.h"
	"src/Platform/OpenGL/OpenGLFrameBuffer.h"
	"src/Platform/OpenGL/OpenGLIntermediateShader.h"
	"src/Platform/OpenGL/OpenGLRendererAPI.h"
	"src/Platform/OpenGL/OpenGLShader.h"
	"src/Platform/OpenGL/OpenGLTexture.h"
	"src/Platform/OpenGL/OpenGLVertexArray.h"

	# .cpp
	"src/Platform/OpenGL/OpenGLBuffer.cpp"
	"src/Platform/OpenGL/OpenGLContext.cpp"
	"src/Platform/OpenGL/OpenGLFrameBuffer.cpp"
	"src/Platform/OpenGL/OpenGLImGuiImplBuild.cpp"
	"src/Platform/OpenGL/OpenGLIntermediateShader.cpp"
	"src/Platform/OpenGL/OpenGLRendererAPI.cpp"
	"src/Platform/OpenGL/OpenGLShader.cpp"
	"src/Platform/OpenGL/OpenGLTexture.cpp"
	"src/Platform/OpenGL/OpenGLVertexArray.cpp"
)

set(OE_OS_WINDOWS_FILES
	# .h
	"src/Platform/Windows/WindowsTime.h"
	"src/Platform/Windows/WindowsWindow.h"

	# .cpp
	"src/Platform/Windows/WindowsImGuiImplBuild.cpp"
	"src/Platform/Windows/WindowsInput.cpp"
	"src/Platform/Windows/WindowsTime.cpp"
	"src/Platform/Windows/WindowsWindow.cpp"
)

set(OE_OS_LINUX_FILES
	# .h
	"src/Platform/Linux/LinuxTime.h"
	"src/Platform/Linux/LinuxWindow.h"

	# .cpp
	"src/Platform/Linux/LinuxImGuiImplBuild.cpp"
	"src/Platform/Linux/LinuxInput.cpp"
	"src/Platform/Linux/LinuxTime.cpp"
	"src/Platform/Linux/LinuxWindow.cpp"
)

if(WIN32)
	set(OE_OS_FILES ${OE_OS_WINDOWS_FILES})
elseif("${CMAKE_SYSTEM}" MATCHES "Linux")
	set(OE_OS_FILES ${OE_OS_LINUX_FILES})
endif()


add_library(OverEngine ${OE_CROSS_PLATFORM_FILES} ${OE_OS_FILES} ${OE_OGL_FILES})
target_precompile_headers(OverEngine PUBLIC "src/pcheader.h")

include_directories(
	"src"
	"res"
	"vendor"
	"vendor/GLFW/include"
	"vendor/Glad/include"
	"vendor/spdlog/include"
	"vendor/imgui"
	"vendor/glm"
	"vendor/lua/src"
	"vendor/stb_image"
	"vendor/stb_rectpack"
	"vendor/entt/include"
	"vendor/box2d/include"
	"vendor/json/include"
)

add_compile_definitions(
	"_CRT_SECURE_NO_WARNINGS"
	"GLFW_INCLUDE_NONE"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions("OE_DEBUG")
endif()
	
if (CMAKE_BUILD_TYPE STREQUAL "Release")
	add_compile_definitions("OE_RELEASE")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	add_compile_definitions("OE_DEBUG")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
	add_compile_definitions("OE_RELEASE")
endif()

target_link_libraries(OverEngine PUBLIC glfw Glad ImGui lua box2d)