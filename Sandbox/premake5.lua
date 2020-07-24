project "Sandbox"		
	if (SandboxHideConsole) then
		kind "WindowedApp"
	else
		kind "ConsoleApp"
	end

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
		"../OverEngine/src",
		"../OverEngine/res",
		"../OverEngine/vendor",
		"../%{IncludeDir.spdlog}",
		"../%{IncludeDir.glm}"
	}
	
	if (IncludeTinyFileDialogs) then
		includedirs "../%{IncludeDir.TFD}"
		if not (DynamicLink) then
			links "TinyFileDialogs"
		end
	end

	links "OverEngine"
	if (DynamicLink) then
		defines "OE_BUILD_SHARED"
	else
		links (LinkLibs)
		defines "OE_BUILD_STATIC"
	end

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}
		
	filter "system:windows"
		systemversion "latest"
		staticruntime (StaticRuntime)
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