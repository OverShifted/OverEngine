#include "SceneLoaderAndSaver.h"

#include <fstream>
#include <json.hpp>

Ref<Scene> CreateSceneOnDisk(const String& path)
{
	nlohmann::json json;

	if (path.size() == 0)
		return nullptr;

	json["Settings"]["Physics2DSettings"]["Gravity"]["x"] = 0.0f;
	json["Settings"]["Physics2DSettings"]["Gravity"]["y"] = -9.8f;

	json["Entities"] = {};

	std::ofstream sceneFile(path);

	sceneFile << json.dump(1, '\t');
	sceneFile.flush();
	sceneFile.close();

	return LoadSceneFromFile(path);
}

Ref<Scene> LoadSceneFromFile(const String& path)
{
	// read a JSON file
	std::ifstream sceneFile(path);
	nlohmann::json sceneJson;
	sceneFile >> sceneJson;

	auto& Physics2DGravityJson = sceneJson["Settings"]["Physics2DSettings"]["Gravity"];

	SceneSettings settings;
	settings.physics2DSettings.gravity.x = Physics2DGravityJson["x"];
	settings.physics2DSettings.gravity.y = Physics2DGravityJson["y"];
	Ref<Scene> scene = CreateRef<Scene>(settings);

	for (auto& entityJson : sceneJson["Entities"])
	{
		Entity entity = scene->CreateEntity(entityJson["Name"]);

		// Transform
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& transformJson = entityJson["Transform"];

		transform.SetPosition({
			transformJson["Position"]["x"],
			transformJson["Position"]["y"],
			transformJson["Position"]["z"]
		});

		transform.SetEulerAngles({
			transformJson["Rotation"]["x"],
			transformJson["Rotation"]["y"],
			transformJson["Rotation"]["z"]
		});

		transform.SetScale({
			transformJson["Scale"]["x"],
			transformJson["Scale"]["y"],
			transformJson["Scale"]["z"]
		});

		auto& componentsJson = entityJson["Components"];

		for (auto& componentJson : componentsJson)
		{
			if (componentJson["TypeName"] == "_CameraComponent")
			{
				if (componentJson["m_Type"] == "Orthographic")
				{
					Camera cam = Camera(
						CameraType::Orthographic, componentJson["m_OrthographicSize"],
						1, componentJson["m_ZNear"], componentJson["m_ZFar"]
					);

					cam.SetIsClearingColor(componentJson["m_IsClearingColor"]);
					cam.SetClearColor({
						componentJson["m_ClearColor"]["r"],
						componentJson["m_ClearColor"]["g"],
						componentJson["m_ClearColor"]["b"],
						componentJson["m_ClearColor"]["a"]
					});
					cam.SetIsClearingDepth(componentJson["m_IsClearingDepth"]);
					entity.AddComponent<CameraComponent>(cam);
				}
				// TODO: Perspective cameras
			}
			else if (componentJson["TypeName"] == "_SpriteRendererComponent")
			{
				entity.AddComponent<SpriteRendererComponent>(nullptr, Color{
					componentJson["m_Tint"]["r"],
					componentJson["m_Tint"]["g"],
					componentJson["m_Tint"]["b"],
					componentJson["m_Tint"]["a"]
				});
			}
		}
	}

	return scene;
}

void SaveSceneToFile(const String& path, Ref<Scene> scene)
{
	nlohmann::json sceneJson;

	// Physics2D Settings
	const auto& Physics2DGravity = scene->GetPhysicsWorld2D().GetGravity();

	sceneJson["Settings"]["Physics2DSettings"]["Gravity"] = {
		{ "x", Physics2DGravity.x },
		{ "y", Physics2DGravity.y }
	};

	Vector<nlohmann::json> entitiesJson;

	uint32_t i = 0;
	for (auto& entity : scene->GetEntities())
	{
		const auto& componentList = entity.GetEntitiesComponentsTypeIDList();

		entitiesJson.push_back({ nlohmann::json() });
		auto& entityJson = entitiesJson[i];

		// Name
		entityJson["Name"] = entity.GetComponent<NameComponent>().Name;

		// Family
		entityJson["Family"]["Parent"]   = nullptr; // TODO: do something
		entityJson["Family"]["Children"] = {};		// TODO: do something

		// Transform
		if (entity.HasComponent<TransformComponent>())
		{
			auto& transform = entity.GetComponent<TransformComponent>();

			entityJson["Transform"]["Position"] = {
				{ "x", transform.GetPosition().x },
				{ "y", transform.GetPosition().y },
				{ "z", transform.GetPosition().z }
			};

			entityJson["Transform"]["Rotation"] = {
				{ "x", transform.GetEulerAngles().x },
				{ "y", transform.GetEulerAngles().y },
				{ "z", transform.GetEulerAngles().z }
			};

			entityJson["Transform"]["Scale"] = {
				{ "x", transform.GetScale().x },
				{ "y", transform.GetScale().y },
				{ "z", transform.GetScale().z }
			};
		}
		else
		{
			entityJson["Transform"] = nullptr;
		}

		entityJson["Components"] = Vector<nlohmann::json>();
		for (const auto& componentTypeID : componentList)
		{
			nlohmann::json componentJson;
			bool componentParsed = false;

			if (componentTypeID == GetComponentTypeID<CameraComponent>())
			{
				auto& camera = entity.GetComponent<CameraComponent>();

				componentJson["TypeName"] = "_CameraComponent";

				if (camera.Camera.IsOrthographic())
				{
					componentJson["m_Type"] = "Orthographic";
					componentJson["m_OrthographicSize"] = camera.Camera.GetOrthographicSize();

					componentJson["m_ZNear"] = camera.Camera.GetZNear();
					componentJson["m_ZFar"] = camera.Camera.GetZFar();

					componentJson["m_IsClearingColor"] = camera.Camera.GetIsClearingColor();
					componentJson["m_ClearColor"] = {
						{ "r", camera.Camera.GetClearColor().r },
						{ "g", camera.Camera.GetClearColor().g },
						{ "b", camera.Camera.GetClearColor().b },
						{ "a", camera.Camera.GetClearColor().a }
					};
					componentJson["m_IsClearingDepth"] = camera.Camera.GetIsClearingDepth();
				}
				// TODO: Perspective cameras

				componentParsed = true;
			}
			else if (componentTypeID == GetComponentTypeID<SpriteRendererComponent>())
			{
				auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();

				componentJson["TypeName"] = "_SpriteRendererComponent";

				componentJson["m_Tint"] = {
					{ "r", spriteRenderer.Tint.r },
					{ "g", spriteRenderer.Tint.g },
					{ "b", spriteRenderer.Tint.b },
					{ "a", spriteRenderer.Tint.a }
				};

				componentParsed = true;
			}

			if (componentParsed)
				entityJson["Components"].push_back(componentJson);
		}

		i++;
	}

	sceneJson["Entities"] = entitiesJson;

	std::ofstream sceneFile(path);
	sceneFile << sceneJson.dump(1, '\t');
	sceneFile.flush();
	sceneFile.close();
}