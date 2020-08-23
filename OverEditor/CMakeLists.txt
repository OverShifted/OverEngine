add_executable(OverEditor 
    "src/ComponentEditor.h"

    "src/Editor.h"
    "src/Editor.cpp"
    
    "src/EditorLayer.h"
    "src/EditorLayer.cpp"
    "src/EditorApp.cpp"

    "src/SceneLoaderAndSaver.h"
    "src/SceneLoaderAndSaver.cpp"
)

include_directories(
    "src"
	"../OverEngine/src"
	"../OverEngine/res"
	"../OverEngine/vendor"
	"../OverEngine/vendor/spdlog/include"
	# "../OverEngine/vendor/imgui"
	"../OverEngine/vendor/glm"
	"../OverEngine/vendor/entt/include"
	"../OverEngine/vendor/box2d/include"
	"../OverEngine/vendor/json/include"
)

add_compile_definitions(
	"_CRT_SECURE_NO_WARNINGS"
	"OE_BUILD_STATIC"
)

target_link_libraries(OverEditor PUBLIC OverEngine)