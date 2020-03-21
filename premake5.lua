workspace "OverEngine"
	architecture "x64"
	configurations
	{
		"Debug",
		"Realease",
		"Dist"
	}
	startproject "OverPlayerExec"

--            debug/realease       OS              x64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- include directories related to Solution folder
IncludeDir = {}
IncludeDir["GLFW"] = "OverEngine/vendor/GLFW/include"

include "OverEngine/vendor/GLFW"

project "OverEngine"
	location "OverEngine"
	kind "SharedLib"
	language "C++"

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
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"OE_PLATFORM_WINDOWS",
			"OE_BUILD_DYNAMIC"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/OverPlayerExec")--,
			--("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/OverEditorExec")
		}

	filter "configurations:Debug"
		defines "OE_DEBUG"
		symbols "On"

	filter "configurations:Realease"
		defines "OE_REALEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "OE_DIST"
		optimize "On"



project "OverPlayerExec"
	location "OverPlayerExec"
	kind "ConsoleApp"
	language "C++"

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
		"OverEngine/vendor/spdlog/include"
	}

	links
	{
		"OverEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"OE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "OE_DEBUG"
		symbols "On"

	filter "configurations:Realease"
		defines "OE_REALEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "OE_DIST"
		optimize "On"


project "OverEditor"
	location "OverEditor"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	-- include {  }

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"OE_PLATFORM_WINDOWS",
			"OE_BUILD_DYNAMIC"
		}

		-- postbuildcommands
		-- {
		-- 	("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/OverEditorExec")
		-- }
	filter "configurations:Debug"
		defines "OE_DEBUG"
		symbols "On"

	filter "configurations:Realease"
		defines "OE_REALEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "OE_DIST"
		optimize "On"

--

project "OverEditorExec"
	location "OverEditorExec"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	--includedirs { "OverEngine/src" }

	links
	{
		"OverEngine",
		"OverEditor"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"OE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "OE_DEBUG"
		symbols "On"

	filter "configurations:Realease"
		defines "OE_REALEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "OE_DIST"
		optimize "On"























-- workspace "OverEngine"
-- 	architecture "x64"

-- 	configurations
-- 	{
-- 		"Debug",
-- 		"Release",
-- 		"Dist"
-- 	}

-- outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


-- project "OverEngine"
-- 	location "OverEngine"
-- 	kind "SharedLib"
-- 	language "C++"

-- 	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
-- 	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

-- 	files
-- 	{
-- 		"%{prj.name}/src/**.h",
-- 		"%{prj.name}/src/**.cpp"
-- 	}

	-- includedirs
	-- {
	-- 	"%{prj.name}/src"
	-- }

	-- links
	-- {
	-- 	"GLFW",
	-- 	"opengl32.lib"
	-- }

	-- filter "system:windows"
	-- 	cppdialect "C++17"
	-- 	staticruntime "On"
	-- 	systemversion "latest"

	-- 	defines
	-- 	{
	-- 		"OE_PLATFORM_WINDOWS",
	-- 		"OE_BUILD_DYNAMIC"
	-- 	}

	-- 	postbuildcommands
	-- 	{
	-- 		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/OverPlayerExec")
	-- 	}

	-- filter "configurations:Debug"
	-- 	defines "OE_DEBUG"
	-- 	symbols "On"

	-- filter "configurations:Release"
	-- 	defines "OE_RELEASE"
	-- 	optimize "On"

	-- filter "configurations:Dist"
	-- 	defines "OE_DIST"
	-- 	optimize "On"

-- project "OverPlayerExec"
-- 	location "OverPlayerExec"
-- 	kind "ConsoleApp"
-- 	language "C++"

-- 	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
-- 	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

-- 	files
-- 	{
-- 		"%{prj.name}/src/**.h",
-- 		"%{prj.name}/src/**.cpp"
-- 	}

-- 	includedirs
-- 	{
-- 		"OverEngine/src"
-- 	}

-- 	links
-- 	{
-- 		"OverEngine"
-- 	}

-- 	filter "system:windows"
-- 		cppdialect "C++17"
-- 		staticruntime "On"
-- 		systemversion "latest"

-- 		defines
-- 		{
-- 			"OE_PLATFORM_WINDOWS"
-- 		}

-- 	filter "configurations:Debug"
-- 		defines "OE_DEBUG"
-- 		symbols "On"

-- 	filter "configurations:Release"
-- 		defines "OE_RELEASE"
-- 		optimize "On"

-- 	filter "configurations:Dist"
-- 		defines "OE_DIST"
-- 		optimize "On"
