project "Sandbox"		
	kind "ConsoleApp"

	language "C++"
	cppdialect "C++17"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir("../bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",

		"assets/shaders/**.glsl"
	}

	defines {
		"YAML_CPP_STATIC_DEFINE"
	}

	includedirs (clientIncludes)

	links "OverEngine"
	links (linkLibs)

	filter "system:windows"
		systemversion "latest"
		staticruntime (staticRuntime)
		files "res/**.rc"
		
	filter "system:linux"
		pic "on"
		systemversion "latest"
		staticruntime "on"
		links { "dl", "pthread" }

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
