project "OverEditor"
	location "OverEditor"

	if (EditorDynamicLink) then
		kind "SharedLib"
	else
		kind "StaticLib"
	end
	
	language "C++"
	cppdialect "C++17"

	targetname "LibOverEditor"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
 	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"OverEngine/src",
		"OverEngine/vendor/spdlog/include",
		"OverEngine/vendor" -- For Imgui
	}
	
	defines
	{
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

	filter "system:linux"
		pic "on"
		systemversion "latest"
		defines "OE_PLATFORM_LINUX"
		staticruntime "on"
		
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