#pragma once

#include "Asset.h"
#include "OverEngine/Renderer/Texture.h"

#include "OverEngine/Core/Serialization/YamlConverters.h"
#include <yaml-cpp/yaml.h>

namespace OverEngine
{
	class Texture2DAsset : public Asset
	{
	public:
		// Load asset from YAML definition
		Texture2DAsset(YAML::Node node, const String& assetsDirectoryRoot);

		const Guid& GetTextureGuid(const Ref<Texture2D>& texture);
		const Guid& GetTextureGuid(Texture2D* texture);

		const auto& GetTextures() const { return m_Textures; }
	private:
		VecPair<Guid, Ref<Texture2D>> m_Textures;
	};
}
