project "OverEngine"
	if (DynamicLink) then
		kind "SharedLib"
	else
		kind "StaticLib"
	end

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
		"../%{IncludeDir.Glad}",
		"../%{IncludeDir.ImGui}",
		"../%{IncludeDir.glm}",
	}

	if (DynamicLink) then
		links
		{
			"GLFW",
			"Glad",
			"ImGui",
		}
	end

	if (IncludeTinyFileDialogs) then
		includedirs "../%{IncludeDir.TFD}"
		if (DynamicLink) then
			links "TinyFileDialogs"
		end
	end

	defines
	{
		"GLFW_INCLUDE_NONE",
		"_CRT_SECURE_NO_WARNINGS"
	}

	if (DynamicLink) then
		defines
		{
			"OE_PROJECT_BUILD_SHARED",
			"OE_BUILD_SHARED",
		}
	else
		defines "OE_BUILD_STATIC"
	end

	filter "system:windows"
		systemversion "latest"
		defines "OE_PLATFORM_WINDOWS"
		staticruntime (StaticRuntime)

		if (DynamicLink) then
			postbuildcommands ("{COPY} %{cfg.buildtarget.relpath} .../bin/" .. outputdir .. "/Sandbox")
			if (IncludeEditor) then
				postbuildcommands ("{COPY} %{cfg.buildtarget.relpath} .../bin/" .. outputdir .. "/OverEditor")
			end
		end

		files
		{
			-- Platform Files
			"src/Platform/Windows/**.h",
			"src/Platform/Windows/**.cpp"
	 	}

	filter "system:linux"
		pic "on"
		systemversion "latest"
		defines "OE_PLATFORM_LINUX"
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

	filter "configurations:Release"
		defines "OE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "OE_DIST"
		runtime "Release"
		optimize "on"
