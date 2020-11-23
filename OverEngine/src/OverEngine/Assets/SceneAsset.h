#pragma once

#include "Asset.h"
#include "OverEngine/Scene/Scene.h"

#include "OverEngine/Core/Serialization/YamlConverters.h"
#include <yaml-cpp/yaml.h>

namespace OverEngine
{
	class SceneAsset : public Asset
	{
	public:
		// Load asset from YAML definition
		SceneAsset(YAML::Node node, const String& assetsDirectoryRoot);

		auto& GetScene() { return m_Scene; }
		const auto& GetScene() const { return m_Scene; }
	private:
		Ref<Scene> m_Scene = nullptr;
	};
}
