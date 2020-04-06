workspace "OverEngine"
	-- architecture "x64"
	startproject "OverPlayerExec"
	platforms {"x64", "x86"}
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	filter "platforms:x64"
		architecture "x64"
	
	filter "platforms:x32"
		architecture "x32"

--            debug/release       OS              x64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include directories related to Solution folder
IncludeDir = {}
IncludeDir["GLFW"]  = "OverEngine/vendor/GLFW/include"
IncludeDir["Glad"]  = "OverEngine/vendor/Glad/include" -------------------------------
IncludeDir["ImGui"] = "OverEngine/vendor/imgui"
IncludeDir["glm"]   = "OverEngine/vendor/glm"

group "Dependencies"
include "OverEngine/vendor/GLFW"
include "OverEngine/vendor/Glad" -----------------------------------
include "OverEngine/vendor/imgui"
group ""

project "OverEngine"
	location "OverEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pcheader.h"
	pchsource "OverEngine/src/pcheader.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
 	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"GLFW",
		"Glad", ---------------------------------
		"ImGui",
		"opengl32.lib"
	}

	defines
	{
		--"OE_PROJECT_BUILD_SHARED",
		--"OE_BUILD_SHARED",
		"OE_BUILD_STATIC",
		"GLFW_INCLUDE_NONE",
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "system:windows"
		systemversion "latest"
		defines "OE_PLATFORM_WINDOWS"

		-- postbuildcommands
		-- {
			--("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/OverPlayerExec")--, NO DLL
			--("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/OverEditorExec")
		-- }

	filter "configurations:Debug"
		defines "OE_DEBUG"
		--buildoptions "/MDd"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "OE_RELEASE"
		--buildoptions "/MD"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "OE_DIST"
		runtime "Release"
		--buildoptions "/MD"
		optimize "on"



project "OverPlayerExec"
	location "OverPlayerExec"
	kind "ConsoleApp" -- "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"OverEngine/src",
		"OverEngine/vendor/spdlog/include",
		"OverEngine/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"OverEngine"
	}

	defines
	{
		--"OE_BUILD_SHARED",
		"OE_BUILD_STATIC"
	}

	filter "system:windows"
		systemversion "latest"
		defines "OE_PLATFORM_WINDOWS"

	filter "configurations:Debug"
		defines "OE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "OE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "OE_DIST"
		runtime "Release"
		optimize "on"


-- project "OverEditor"
-- 	location "OverEditor"
-- 	kind "SharedLib"
-- 	language "C++"

-- 	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
-- 	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

-- 	files
-- 	{
-- 		"%{prj.name}/src/**.h",
-- 		"%{prj.name}/src/**.cpp"
-- 	}

-- 	-- include {  }

-- 	filter "system:windows"
-- 		cppdialect "C++17"
-- 		staticruntime "on"
-- 		systemversion "latest"

-- 		defines
-- 		{
-- 			"OE_PLATFORM_WINDOWS",
-- 			"OE_BUILD_DYNAMIC"
-- 		}

-- 		-- postbuildcommands
-- 		-- {
-- 		-- 	("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/OverEditorExec")
-- 		-- }
-- 	filter "configurations:Debug"
-- 		defines "OE_DEBUG"
-- 		buildoptions "/MDd"
-- 		symbols "on"

-- 	filter "configurations:Release"
-- 		defines "OE_RELEASE"
-- 		buildoptions "/MD"
-- 		optimize "on"

-- 	filter "configurations:Dist"
-- 		defines "OE_DIST"
-- 		buildoptions "/MD"
-- 		optimize "on"

-- --

-- project "OverEditorExec"
-- 	location "OverEditorExec"
-- 	kind "ConsoleApp"
-- 	language "C++"

-- 	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
-- 	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

-- 	files
-- 	{
-- 		"%{prj.name}/src/**.h",
-- 		"%{prj.name}/src/**.cpp"
-- 	}

-- 	--includedirs { "OverEngine/src" }

-- 	links
-- 	{
-- 		--"OverEngine",
-- 		--"OverEditor"
-- 	}

-- 	filter "system:windows"
-- 		cppdialect "C++17"
-- 		staticruntime "on"
-- 		systemversion "latest"

-- 		defines
-- 		{
-- 			"OE_PLATFORM_WINDOWS"
-- 		}

-- 	filter "configurations:Debug"
-- 		defines "OE_DEBUG"
-- 		buildoptions "/MDd"
-- 		symbols "on"

-- 	filter "configurations:Release"
-- 		defines "OE_RELEASE"
-- 		buildoptions "/MD"
-- 		optimize "on"

-- 	filter "configurations:Dist"
-- 		defines "OE_DIST"
-- 		buildoptions "/MD"
-- 		optimize "on"