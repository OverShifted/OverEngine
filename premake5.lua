_32BitSupport = false

-- Windows only. for Linux, "on" is hardcoded
StaticRuntime = "on"

SandboxHideConsole        = false
DynamicLink               = false
OverEditorExecHideConsole = false

IncludeEditor = true
IncludeLua    = true

workspace "OverEngine"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"DebugOptimized",
		"Release",
		"Dist"
	}

	if (_32BitSupport) then
		platforms {"x86_64", "x86"}
		filter "platforms:x86_64"
			architecture "x86_64"
		filter "platforms:x86"
			architecture "x32"
	else
		architecture "x86_64"
	end

	flags
	{
		"MultiProcessorCompile"
	}


--            configurations       OS              x86_64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include directories related to Solution folder
IncludeDir = {}
IncludeDir["GLFW"]          = "OverEngine/vendor/GLFW/include"
IncludeDir["Glad"]          = "OverEngine/vendor/Glad/include"
IncludeDir["spdlog"]        = "OverEngine/vendor/spdlog/include"
IncludeDir["ImGui"]         = "OverEngine/vendor/imgui"
IncludeDir["glm"]           = "OverEngine/vendor/glm"
IncludeDir["lua"]           = "OverEngine/vendor/lua/src"
IncludeDir["stb_image"]     = "OverEngine/vendor/stb_image"
IncludeDir["stb_rectpack"]  = "OverEngine/vendor/stb_rectpack"
IncludeDir["entt"]          = "OverEngine/vendor/entt/include"
IncludeDir["Box2D"]         = "OverEngine/vendor/box2d/include"
IncludeDir["json"]          = "OverEngine/vendor/json/include"

LinkLibs = {
	"GLFW",
	"Glad",
	"ImGui",
	"Lua",
	"Box2D"
}

group "Dependencies"
	include "OverEngine/vendor/GLFW"
	include "OverEngine/vendor/Glad"
	include "OverEngine/vendor/imgui"
	include "OverEngine/vendor/box2d"
	if (IncludeLua) then
		include "OverEngine/vendor/lua"
	end
group ""

include "OverEngine"
include "Sandbox"

if (IncludeEditor) then
	include "OverEditor"
end
