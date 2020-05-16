project "FontFormatter"
	location "OverEngine/vendor/imgui/misc/fonts"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetname "ff"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files "OverEngine/vendor/imgui/misc/fonts/binary_to_compressed_c.cpp"

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"