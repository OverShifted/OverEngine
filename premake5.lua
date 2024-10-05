support32Bit = false

-- Windows only. for Linux, "on" is hardcoded
staticRuntime = "on"

workspace "OverEngine"
	startproject "Sandbox"

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


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include directories relative to Solution folder
includeDir = {}
includeDir["GLFW"]     = "%{wks.location}/OverEngine/vendor/GLFW/include"
includeDir["glad"]     = "%{wks.location}/OverEngine/vendor/glad/include"
includeDir["spdlog"]   = "%{wks.location}/OverEngine/vendor/spdlog/include"
includeDir["imgui"]    = "%{wks.location}/OverEngine/vendor/imgui"
includeDir["glm"]      = "%{wks.location}/OverEngine/vendor/glm"
includeDir["stb"]      = "%{wks.location}/OverEngine/vendor/stb"
includeDir["entt"]     = "%{wks.location}/OverEngine/vendor/entt/include"
includeDir["box2d"]    = "%{wks.location}/OverEngine/vendor/box2d/include"
includeDir["json"]     = "%{wks.location}/OverEngine/vendor/json/include"
includeDir["fmt"]      = "%{wks.location}/OverEngine/vendor/fmt/include"
includeDir["yaml_cpp"] = "%{wks.location}/OverEngine/vendor/yaml-cpp/include"
includeDir["wren"]     = "%{wks.location}/OverEngine/vendor/wren/src/include"
includeDir["wren_vm"]  = "%{wks.location}/OverEngine/vendor/wren/src/vm"

linkLibs = {
	"GLFW",
	"glad",
	"imgui",
	"box2d",
	"yaml-cpp",
	"wren",
}

clientIncludes = {
	"%{wks.location}/OverEngine/src",
	"%{wks.location}/OverEngine/vendor",
	"%{includeDir.spdlog}",
	"%{includeDir.imgui}",
	"%{includeDir.glm}",
	"%{includeDir.stb}",
	"%{includeDir.entt}",
	"%{includeDir.box2d}",
	"%{includeDir.json}",
	"%{includeDir.fmt}",
	"%{includeDir.yaml_cpp}",
	"%{includeDir.wren}",
	"%{includeDir.wren_vm}",
}

group "Dependencies"
	include "OverEngine/vendor/GLFW"
	include "OverEngine/vendor/glad"
	include "OverEngine/vendor/imgui"
	include "OverEngine/vendor/box2d"
	include "OverEngine/vendor/yaml-cpp"
	include "OverEngine/vendor/wren"
group ""

include "OverEngine"
-- include "OverEditor"
include "Sandbox"
