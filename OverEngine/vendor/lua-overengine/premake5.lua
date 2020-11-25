project "lua-overengine"
	kind "StaticLib"

	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"lua/src/lua.hpp",
		"lua/src/**.h",
		"lua/src/**.c",

		"sol/include/**.hpp"
	}

	excludes
	{
		"lua/src/luac.c"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"sol/include",
		"lua/src"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime (staticRuntime)

	filter "system:linux"
		pic "on"
		systemversion "latest"
		staticruntime "on"

		filter "configurations:Debug"
		defines "OE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:DebugOptimized"
		defines "OE_DEBUG"
		runtime "Debug"
		symbols "on"
		optimize "on"

	filter "configurations:Release"
		defines "OE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "OE_DIST"
		runtime "Release"
		optimize "on"
