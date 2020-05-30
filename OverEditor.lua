project "OverEditor"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime (StaticRuntime)

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	--pchheader "pcheader.h"
	--pchsource "OverEngine/src/pcheader.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",

		-- Resources
		--"%{prj.name}/resources/**.h",
 	}

	includedirs
	{
		"%{prj.name}/src",
		"OverEngine/src",
		"OverEngine/vendor/spdlog/include",
		"OverEngine/vendor" -- For Imgui
		--"%{prj.name}/resources",
		--"%{prj.name}/vendor/spdlog/include",
		--"%{IncludeDir.GLFW}",
		--"%{IncludeDir.Glad}",
		--"%{IncludeDir.ImGui}",
		--"%{IncludeDir.glm}"
	}

	-- links
	-- {
	-- 	"GLFW",
	-- 	"Glad", ---------------------------------
	-- 	"ImGui",
	-- 	"opengl32.lib"
	-- }

	defines
	{
		--"OE_PROJECT_BUILD_SHARED",
		--"OE_BUILD_SHARED",
		"OE_BUILD_STATIC",
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "system:windows"
		systemversion "latest"
		defines "OE_PLATFORM_WINDOWS"

		-- postbuildcommands
		-- {
			--("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/OverPlayerExec")--, NO DLL
			--("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/OverEditorExec")
		-- }

	filter "configurations:Debug"
		defines "OE_DEBUG"
		--buildoptions "/MDd"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "OE_RELEASE"
		--buildoptions "/MD"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "OE_DIST"
		runtime "Release"
		--buildoptions "/MD"
		optimize "on"
