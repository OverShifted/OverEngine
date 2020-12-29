file(GLOB header_files "lua/src/*.h")
file(GLOB source_files "lua/src/*.c")
list(REMOVE_ITEM source_files "lua/src/luac.c")

file(GLOB sol_files "sol/include/*.hpp")

add_library(lua-overengine
	${source_files}
	${header_files}
	"lua/src/lua.hpp"
	${sol_files}
)

if(WIN32)
	target_compile_definitions(lua-overengine PRIVATE
		"LUA_USE_WINDOWS"
	)
elseif("${CMAKE_SYSTEM}" MATCHES "Linux")
	target_compile_definitions(lua-overengine PRIVATE
		"LUA_USE_LINUX"
	)
endif()
