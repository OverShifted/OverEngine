project "OverEditor"
	location "OverEditor"

	if (OverEditorExecHideConsole) then
		kind "WindowedApp"
	else
		kind "ConsoleApp"
	end

	language "C++"
	cppdialect "C++17"
	staticruntime (StaticRuntime)

	targetname "OverEditor"
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
		"OverEditor/src",
		"OverEngine/vendor/spdlog/include",
		"OverEngine/vendor",
		"%{IncludeDir.glm}"
	}

	links "OverEngine"

	if (DynamicLink) then
		defines "OE_BUILD_SHARED"
	else
		links
		{
			"GLFW",
			"Glad",
			"ImGui",
		}
		defines "OE_BUILD_STATIC"
	end

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
