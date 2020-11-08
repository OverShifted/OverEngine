file(GLOB_RECURSE sandbox_source_files "src/*.cpp")
file(GLOB_RECURSE sandbox_header_files "src/*.h")

add_executable(Sandbox
	${sandbox_source_files}
	${sandbox_header_files}
)

set_property(TARGET Sandbox PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Sandbox")

target_link_libraries(Sandbox PRIVATE OverEngine)
