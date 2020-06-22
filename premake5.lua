_32BitSupport = false

-- Windows only. for Linux, "on" is hardcoded
StaticRuntime = "on"

SandboxHideConsole        = false
DynamicLink               = false
OverEditorExecHideConsole = false

IncludeEditor        = true
IncludeFontFormatter = false

workspace "OverEngine"
	startproject "Sandbox"

	configurations
	{
		"Debug",
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
IncludeDir["GLFW"]    = "OverEngine/vendor/GLFW/include"
IncludeDir["Glad"]    = "OverEngine/vendor/Glad/include"
IncludeDir["spdlog"]  = "OverEngine/vendor/spdlog/include"
IncludeDir["ImGui"]   = "OverEngine/vendor/imgui"
IncludeDir["glm"]     = "OverEngine/vendor/glm"
LinkLibs = {
	"GLFW",
	"Glad",
	"ImGui",
}

group "Dependencies"
	include "OverEngine/vendor/GLFW"
	include "OverEngine/vendor/Glad"
	include "OverEngine/vendor/imgui"

	if (IncludeFontFormatter) then
		include "FontFormatter_premake5.lua"
	end
group ""

include "OverEngine"
include "Sandbox"

if (IncludeEditor) then
	include "OverEditor"
end
