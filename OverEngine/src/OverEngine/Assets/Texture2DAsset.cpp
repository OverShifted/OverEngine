#include "pcheader.h"
#include "Texture2DAsset.h"

#include "OverEngine/Core/Serialization/Serializer.h"

namespace OverEngine
{
	Texture2DAsset::Texture2DAsset(YAML::Node node, const String& assetsDirectoryRoot)
	{
		m_Type = AssetType::Texture2D;
		m_Name = node["Name"].as<String>();
		m_Path = node["Path"].as<String>();
		m_Guid = node["Guid"].as<Guid>();

		if (!Serializer::GlobalEnumExists("TextureType"))
		{
			Serializer::DefineGlobalEnum("TextureType", {
				{ 0, "Master" },
				{ 1, "Subtexture" }
			});
		}

		for (auto assetNode : node["Textures"])
		{
			if (Serializer::GetGlobalEnumValue("TextureType", assetNode["Type"].as<String>()) == (int)TextureType::Master)
			{
				auto tex = Texture2D::CreateMaster(assetsDirectoryRoot + m_Path);
				tex->m_MasterTextureData.Asset = this;
				m_Textures.push_back({ Guid("fcbf7858-4013-4b9a-8d17-0d16ce4c25a4"), tex });
			}
		}
	}

	const Guid& Texture2DAsset::GetTextureGuid(const Ref<Texture2D>& texture)
	{
		for (const auto& t : m_Textures)
			if (t.second == texture)
				return t.first;

		OE_CORE_ASSERT(false, "Texture doesn't belongs to this asset");
		return NULL_REF(Guid);
	}

	const Guid& Texture2DAsset::GetTextureGuid(Texture2D* texture)
	{
		for (const auto& t : m_Textures)
			if (t.second.get() == texture)
				return t.first;

		OE_CORE_ASSERT(false, "Texture doesn't belongs to this asset");
		return NULL_REF(Guid);
	}
}
