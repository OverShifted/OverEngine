project "OverEngine"
	kind "StaticLib"

	language "C++"
	cppdialect "C++17"

	targetname "OverEngine"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir("../bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pcheader.h"
	pchsource "src/pcheader.cpp"

	files
	{
		-- Source Files
		"src/*.h",
		"src/*.cpp",
		"src/OverEngine/**.h",
		"src/OverEngine/**.cpp",

		-- stb_image
		"vendor/stb_image/stb_image.h",
		"vendor/stb_image/stb_image.cpp",

		-- stb_rectpack
		"vendor/stb_rectpack/stb_rectpack.h",
		"vendor/stb_rectpack/stb_rectpack.cpp",

		-- tinyfiledialogs
		"vendor/tinyfiledialogs/tinyfiledialogs.h",
		"vendor/tinyfiledialogs/tinyfiledialogsBuild.cpp",

		-- Resources
		"res/**.h",

		-- RendererAPI Files
		"src/Platform/OpenGL/**.h",
		"src/Platform/OpenGL/**.cpp",
	 }

	includedirs
	{
		"src",
		"res",
		"vendor",
		"../%{IncludeDir.spdlog}",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.glad}",
		"../%{IncludeDir.imgui}",
		"../%{IncludeDir.glm}",
		"../%{IncludeDir.lua}",
		"../%{IncludeDir.stb_image}",
		"../%{IncludeDir.stb_rectpack}",
		"../%{IncludeDir.entt}",
		"../%{IncludeDir.box2d}",
		"../%{IncludeDir.json}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.yaml_cpp}",
	}

	defines
	{
		"GLFW_INCLUDE_NONE",
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime (StaticRuntime)

		files
		{
			-- Platform Files
			"src/Platform/Windows/**.h",
			"src/Platform/Windows/**.cpp"
	 	}

	filter "system:linux"
		pic "on"
		systemversion "latest"
		staticruntime "on"

		files
		{
			-- Platform Files
			"src/Platform/Linux/**.h",
			"src/Platform/Linux/**.cpp"
	 	}

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
