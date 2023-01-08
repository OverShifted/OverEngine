file(GLOB_RECURSE OE_SOURCE_FILES "src/OverEngine/*.cpp")
file(GLOB_RECURSE OE_HEADER_FILES "src/OverEngine/*.h")

set(OE_CROSS_PLATFORM_FILES
	"src/OverEngine.h"
	"src/pcheader.cpp"
	"src/pcheader.h"

	${OE_SOURCE_FILES}
	${OE_HEADER_FILES}

	"vendor/stb/stb_image.h"
	"vendor/stb/stb_image.cpp"

	"vendor/stb/stb_sprintf.h"
	"vendor/stb/stb_sprintf.cpp"

	"vendor/tinyfiledialogs/tinyfiledialogs.h"
	"vendor/tinyfiledialogs/tinyfiledialogsBuild.cpp"

	"vendor/wrenpp/Wren++.h"
	"vendor/wrenpp/Wren++.cpp"
)

file(GLOB_RECURSE OE_OPENGL_SOURCE_FILES "src/Platform/OpenGL/*.cpp")
file(GLOB_RECURSE OE_OPENGL_HEADER_FILES "src/Platform/OpenGL/*.h")

file(GLOB_RECURSE OE_WINDOWS_SOURCE_FILES "src/Platform/Windows/*.cpp")
file(GLOB_RECURSE OE_WINDOWS_HEADER_FILES "src/Platform/Windows/*.h")

file(GLOB_RECURSE OE_LINUX_SOURCE_FILES "src/Platform/Linux/*.cpp")
file(GLOB_RECURSE OE_LINUX_HEADER_FILES "src/Platform/Linux/*.h")

if(WIN32)
	set(OE_PLATFORM_FILES ${OE_WINDOWS_SOURCE_FILES} ${OE_WINDOWS_HEADER_FILES})
elseif("${CMAKE_SYSTEM}" MATCHES "Linux")
	set(OE_PLATFORM_FILES ${OE_LINUX_SOURCE_FILES} ${OE_LINUX_HEADER_FILES})
endif()

file(GLOB_RECURSE OE_WREN_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/src/Wren" "src/Wren/*.wren")
set(OE_WREN_INC_FILES)
foreach(WREN_FILE IN LISTS OE_WREN_FILES)
	add_custom_command(
		OUTPUT "${WREN_FILE}.inc"
		MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/src/Wren/${WREN_FILE}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/Wren"
		COMMAND python "${PROJECT_SOURCE_DIR}/utils/wren_to_c_string.py" "${WREN_FILE}.inc" ${WREN_FILE}
	)

	list(APPEND OE_WREN_INC_FILES "${WREN_FILE}.inc")
endforeach()

add_library(OverEngine STATIC ${OE_CROSS_PLATFORM_FILES} ${OE_PLATFORM_FILES}
	${OE_OPENGL_SOURCE_FILES}
	${OE_OPENGL_HEADER_FILES}

	${OE_WREN_INC_FILES}
)

target_precompile_headers(OverEngine PUBLIC "src/pcheader.h")

target_include_directories(OverEngine PRIVATE
	"res"
	"vendor/GLFW/include"
	"vendor/glad/include"
)

target_include_directories(OverEngine PUBLIC
	"src"
	"vendor"
	"vendor/spdlog/include"
	"vendor/imgui"
	"vendor/glm"
	"vendor/stb"
	"vendor/entt/include"
	"vendor/box2d/include"
	"vendor/json/include"
	"vendor/fmt/include"
	"vendor/yaml-cpp/include"
	"vendor/wren/src/include"
)

target_compile_definitions(
	OverEngine

	PRIVATE
	"_CRT_SECURE_NO_WARNINGS"
	"GLFW_INCLUDE_NONE"

	PUBLIC
	$<$<CONFIG:Debug>:OE_DEBUG>
	$<$<CONFIG:Release>:OE_RELEASE>

	$<$<CONFIG:RelWithDebInfo>:OE_DEBUG>
	$<$<CONFIG:MinSizeRel>:OE_RELEASE>
)

target_link_libraries(OverEngine PRIVATE glfw glad)
target_link_libraries(OverEngine PUBLIC imgui box2d yaml-cpp wren)

