project "OverEngine"
	location "OverEngine"

	if (DynamicLink) then
		kind "SharedLib"
	else
		kind "StaticLib"
	end

	language "C++"
	cppdialect "C++17"

	targetname "LibOverEngine"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pcheader.h"
	pchsource "OverEngine/src/pcheader.cpp"

	files
	{
		-- Source Files
		"%{prj.name}/src/*.h",
		"%{prj.name}/src/*.cpp",
		"%{prj.name}/src/OverEngine/**.h",
		"%{prj.name}/src/OverEngine/**.cpp",

		-- Resources
		"%{prj.name}/res/**.h",

		-- RendererAPI Files
		"%{prj.name}/src/Platform/OpenGL/**.h",
		"%{prj.name}/src/Platform/OpenGL/**.cpp",
	 }

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/res",
		"%{prj.name}/vendor",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
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
		includedirs "%{IncludeDir.TFD}"
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
			postbuildcommands ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/OverPlayerExec")
			if (IncludeEditor) then
				postbuildcommands ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/OverEditorExec")
			end
		end

		files
		{
			-- Platform Files
			"%{prj.name}/src/Platform/Windows/**.h",
			"%{prj.name}/src/Platform/Windows/**.cpp"
	 	}

	filter "system:linux"
		pic "on"
		systemversion "latest"
		defines "OE_PLATFORM_LINUX"
		staticruntime "on"

		files
		{
			-- Platform Files
			"%{prj.name}/src/Platform/Linux/**.h",
			"%{prj.name}/src/Platform/Linux/**.cpp"
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