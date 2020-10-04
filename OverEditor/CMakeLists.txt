file(GLOB_RECURSE overeditor_source_files "src/*.cpp")
file(GLOB_RECURSE overeditor_header_files "src/*.h")

add_executable(OverEditor 
    ${overeditor_source_files}
    ${overeditor_header_files}
)

target_include_directories(OverEditor PRIVATE
    "src"
)

set_property(TARGET OverEditor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/OverEditor")

target_link_libraries(OverEditor PRIVATE OverEngine)
