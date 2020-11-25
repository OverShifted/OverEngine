project "OverEditor"
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

	includedirs
	{
		"src",
		"%{wks.location}/OverEngine/src",
		"%{wks.location}/OverEngine/vendor",
		"%{includeDir.spdlog}",
		"%{includeDir.imgui}",
		"%{includeDir.lua}",
		"%{includeDir.sol}",
		"%{includeDir.glm}",
		"%{includeDir.entt}",
		"%{includeDir.box2d}",
		"%{includeDir.json}",
		"%{includeDir.fmt}",
		"%{includeDir.yaml_cpp}",
	}

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
