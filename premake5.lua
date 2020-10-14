_32BitSupport = false

-- Windows only. for Linux, "on" is hardcoded
StaticRuntime = "on"

SandboxHideConsole    = false
OverEditorHideConsole = false

workspace "OverEngine"
	startproject "OverEditor"

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
		-- Enable multi threaded compilation in VisualStudio
		"MultiProcessorCompile"
	}


--            configurations       OS              x86_64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include directories related to Solution folder
IncludeDir = {}
IncludeDir["GLFW"]         = "OverEngine/vendor/GLFW/include"
IncludeDir["glad"]         = "OverEngine/vendor/glad/include"
IncludeDir["spdlog"]       = "OverEngine/vendor/spdlog/include"
IncludeDir["imgui"]        = "OverEngine/vendor/imgui"
IncludeDir["glm"]          = "OverEngine/vendor/glm"
IncludeDir["lua"]          = "OverEngine/vendor/lua/src"
IncludeDir["stb_image"]    = "OverEngine/vendor/stb_image"
IncludeDir["stb_rectpack"] = "OverEngine/vendor/stb_rectpack"
IncludeDir["entt"]         = "OverEngine/vendor/entt/include"
IncludeDir["box2d"]        = "OverEngine/vendor/box2d/include"
IncludeDir["json"]         = "OverEngine/vendor/json/include"
IncludeDir["fmt"]          = "OverEngine/vendor/fmt/include"
IncludeDir["yaml-cpp"]     = "OverEngine/vendor/yaml-cpp/include"

LinkLibs = {
	"GLFW",
	"glad",
	"imgui",
	"lua",
	"box2d",
	"yaml-cpp"
}

group "Dependencies"
	include "OverEngine/vendor/GLFW"
	include "OverEngine/vendor/Glad"
	include "OverEngine/vendor/imgui"
	include "OverEngine/vendor/box2d"
	include "OverEngine/vendor/lua"
	include "OverEngine/vendor/yaml-cpp"
group ""

include "OverEngine"
include "OverEditor"
include "Sandbox"
