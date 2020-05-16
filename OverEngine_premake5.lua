project "OverEngine"
    location "OverEngine"

    if (DynamicLink) then
        kind "SharedLib"
    else
        kind "StaticLib"
    end

    language "C++"
    cppdialect "C++17"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "pcheader.h"
    pchsource "OverEngine/src/pcheader.cpp"

    files
    {
        -- Source Files
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",

        -- Resources
        "%{prj.name}/resources/**.h",
     }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/resources",
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
            "opengl32.lib"
        }
    end
    
    if (IncludeTinyFileDialogs) then
        includedirs "%{IncludeDir.TFD}"
        links "TinyFileDialogs"
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