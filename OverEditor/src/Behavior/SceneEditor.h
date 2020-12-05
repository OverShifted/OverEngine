#pragma once

#include <OverEngine/Assets/SceneAsset.h>

#include <entt.hpp>
#include <experimental/vector>

namespace OverEditor
{
	using namespace OverEngine;

	struct SceneEditor
	{
		SceneEditor() = default;

		// Primary scene asset which is open in the editor.
		// Can be nullptr.
		Ref<SceneAsset> PrimaryScene = nullptr;

		// The scene that is running
		Ref<Scene> SecondaryScene = nullptr;

		Vector<entt::entity> Selection;

		enum RuntimeFlags_ : uint8_t
		{
			RuntimeFlags_None                    = 0,
			RuntimeFlags_Simulating              = BIT(0),
			RuntimeFlags_SimulationPaused        = BIT(1),
			RuntimeFlags_SimulationStepNextFrame = BIT(2)
		};

		uint8_t RuntimeFlags = RuntimeFlags_None;

		Ref<Scene> GetActiveScene()
		{
			return RuntimeFlags & RuntimeFlags_Simulating ? SecondaryScene : PrimaryScene->GetScene();
		}

		bool AnySceneOpen()
		{
			return (bool)PrimaryScene;
		}

		void BeginSimulation()
		{
			SecondaryScene = CreateRef<Scene>(*PrimaryScene->GetScene());
			SecondaryScene->InitializePhysics();
		}

		void EndSimulation()
		{
			SecondaryScene = nullptr;

			std::experimental::erase_if(Selection, [this](const entt::entity& entity) {
				return !PrimaryScene->GetScene()->Exists(entity);
			});
		}
	};
}
