support32Bit = false

-- Windows only. for Linux, "on" is hardcoded
staticRuntime = "on"

workspace "OverEngine"
	startproject "OverEditor"

	configurations
	{
		"Debug",
		"DebugOptimized",
		"Release",
		"Dist"
	}

	if (support32Bit) then
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
includeDir = {}
includeDir["GLFW"]         = "%{wks.location}/OverEngine/vendor/GLFW/include"
includeDir["glad"]         = "%{wks.location}/OverEngine/vendor/glad/include"
includeDir["spdlog"]       = "%{wks.location}/OverEngine/vendor/spdlog/include"
includeDir["imgui"]        = "%{wks.location}/OverEngine/vendor/imgui"
includeDir["glm"]          = "%{wks.location}/OverEngine/vendor/glm"
includeDir["stb_image"]    = "%{wks.location}/OverEngine/vendor/stb_image"
includeDir["stb_rectpack"] = "%{wks.location}/OverEngine/vendor/stb_rectpack"
includeDir["entt"]         = "%{wks.location}/OverEngine/vendor/entt/include"
includeDir["box2d"]        = "%{wks.location}/OverEngine/vendor/box2d/include"
includeDir["json"]         = "%{wks.location}/OverEngine/vendor/json/include"
includeDir["fmt"]          = "%{wks.location}/OverEngine/vendor/fmt/include"
includeDir["yaml_cpp"]     = "%{wks.location}/OverEngine/vendor/yaml-cpp/include"

linkLibs = {
	"GLFW",
	"glad",
	"imgui",
	"box2d",
	"yaml-cpp"
}

group "Dependencies"
	include "OverEngine/vendor/GLFW"
	include "OverEngine/vendor/Glad"
	include "OverEngine/vendor/imgui"
	include "OverEngine/vendor/box2d"
	include "OverEngine/vendor/yaml-cpp"
group ""

include "OverEngine"
include "OverEditor"
include "Sandbox"
