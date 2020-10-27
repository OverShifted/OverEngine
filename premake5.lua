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
IncludeDir["GLFW"]         = "%{wks.location}/OverEngine/vendor/GLFW/include"
IncludeDir["glad"]         = "%{wks.location}/OverEngine/vendor/glad/include"
IncludeDir["spdlog"]       = "%{wks.location}/OverEngine/vendor/spdlog/include"
IncludeDir["imgui"]        = "%{wks.location}/OverEngine/vendor/imgui"
IncludeDir["glm"]          = "%{wks.location}/OverEngine/vendor/glm"
IncludeDir["lua"]          = "%{wks.location}/OverEngine/vendor/lua/src"
IncludeDir["stb_image"]    = "%{wks.location}/OverEngine/vendor/stb_image"
IncludeDir["stb_rectpack"] = "%{wks.location}/OverEngine/vendor/stb_rectpack"
IncludeDir["entt"]         = "%{wks.location}/OverEngine/vendor/entt/include"
IncludeDir["box2d"]        = "%{wks.location}/OverEngine/vendor/box2d/include"
IncludeDir["json"]         = "%{wks.location}/OverEngine/vendor/json/include"
IncludeDir["fmt"]          = "%{wks.location}/OverEngine/vendor/fmt/include"
IncludeDir["yaml_cpp"]     = "%{wks.location}/OverEngine/vendor/yaml-cpp/include"

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
