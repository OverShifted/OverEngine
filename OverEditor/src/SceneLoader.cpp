#include "SceneLoader.h"

#include <fstream>
#include <json.hpp>

Ref<Scene> LoadSceneFromFile(const String& path)
{
	// read a JSON file
	std::ifstream sceneFile("assets/scenes/Scene.oes");
	nlohmann::json sceneJson;
	sceneFile >> sceneJson;

	Ref<Scene> scene = CreateRef<Scene>();

	for (auto& entityJson : sceneJson["Entities"])
	{
		Entity entity = scene->CreateEntity(entityJson["Name"]);

		// Transform
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& transformJson = entityJson["Transform"];
		transform.SetPosition({ transformJson["Position"]["x"], transformJson["Position"]["y"], transformJson["Position"]["z"] });
		transform.SetRotation({ transformJson["Rotation"]["x"], transformJson["Rotation"]["y"], transformJson["Rotation"]["z"], transformJson["Rotation"]["w"] });
		transform.SetScale({ transformJson["Scale"]["x"], transformJson["Scale"]["y"], transformJson["Scale"]["z"] });

		auto& componentsJson = entityJson["Components"];

		for (auto& componentJson : componentsJson)
		{
			if (componentJson["TypeName"] == "_CameraComponent")
			{
				if (componentJson["m_Type"] == "Orthographic")
				{
					Camera cam = Camera(CameraType::Orthographic, componentJson["m_OrthographicSize"], 1, componentJson["m_ZNear"], componentJson["m_ZFar"]);

					cam.SetIsClearingColor(componentJson["m_IsClearingColor"]);
					cam.SetClearColor({ componentJson["m_ClearColor"]["r"], componentJson["m_ClearColor"]["g"], componentJson["m_ClearColor"]["b"], componentJson["m_ClearColor"]["a"] });
					cam.SetIsClearingDepth(componentJson["m_IsClearingDepth"]);
					entity.AddComponent<CameraComponent>(cam);
				}
				// TODO : Perspective cameras
			}
			else if (componentJson["TypeName"] == "_SpriteRendererComponent")
			{

			}
		}
	}

	return scene;
}