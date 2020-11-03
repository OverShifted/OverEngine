#include "pcheader.h"
#include "Texture2DAsset.h"

#include "OverEngine/Core/Random.h"
#include "OverEngine/Core/Serialization/Serializer.h"

namespace OverEngine
{
	Texture2DAsset::Texture2DAsset(YAML::Node node, const String& assetsDirectoryRoot)
	{
		m_Type = AssetType::Texture2D;
		m_Name = node["Name"].as<String>();
		m_Path = node["Path"].as<String>();
		m_Guid = node["Guid"].as<uint64_t>();

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
				std::get<MasterTextureData>(tex->m_Data).Asset = this;
				m_Textures[assetNode["Texture2D"].as<uint64_t>()] = tex;
			}
		}
	}

	const uint64_t& Texture2DAsset::GetTextureGuid(const Ref<Texture2D>& texture)
	{
		for (const auto& t : m_Textures)
			if (t.second == texture)
				return t.first;

		OE_CORE_ASSERT(false, "Texture doesn't belongs to this asset");
		return NULL_REF(uint64_t);
	}

	const uint64_t& Texture2DAsset::GetTextureGuid(Texture2D* texture)
	{
		for (const auto& t : m_Textures)
			if (t.second.get() == texture)
				return t.first;

		OE_CORE_ASSERT(false, "Texture doesn't belongs to this asset");
		return NULL_REF(uint64_t);
	}
}
