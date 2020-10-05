project "glad"
    kind "StaticLib"
    language "C"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "include/glad/gl.h",
        "include/KHR/khrplatform.h",
        "src/gl.c"
    }

    includedirs
    {
        "include"
    }
    
    filter "system:linux"
        pic "on"
        systemversion "latest"
        staticruntime "on"

    filter "system:windows"
        systemversion "latest"
        staticruntime (StaticRuntime)

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:DebugOptimized"
		runtime "Debug"
		symbols "on"
		optimize "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"