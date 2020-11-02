#pragma once

#include "OverEngine/Core/Core.h"
#include "Scene.h"

namespace OverEngine
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		// YAML
		void Serialize(const String& filepath);
		bool Deserialize(const String& filepath);
	private:
		Ref<Scene> m_Scene;
	};
}
