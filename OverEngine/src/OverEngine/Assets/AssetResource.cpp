#include "pcheader.h"
#include "AssetResource.h"

#include <json.hpp>

namespace OverEngine
{
	AssetResource::AssetResource(const String& filePath)
	{
		nlohmann::json assetFileJson;
		std::ifstream assetFile(filePath);
		assetFile >> assetFileJson;

		m_Name = assetFileJson["Name"];
		m_VirtualPath = assetFileJson["FilePath"];

		for (const auto& asset : assetFileJson["Assets"])
		{
			if (asset["TypeName"] == "MasterTexture")
			{
				// auto texture = Texture2D::MasterFromFile(asset["FilePath"]);
				// m_Assets.push_back(CreateRef<Texture2DAsset>(texture, asset["FilePath"]));
			}
		}
	}

	AssetResource::AssetResource(const String& name, const String& virtualPath, AssetResourceType type)
		: m_Name(name), m_VirtualPath(virtualPath), m_Type(type)
	{
	}

	void AssetResource::AddAsset(const Ref<Asset>& asset)
	{
		m_Assets.push_back(asset);
	}

	void AssetResource::RemoveAsset(const Ref<Asset>& asset)
	{
		auto it = std::find(m_Assets.begin(), m_Assets.end(), asset);
		if (it != m_Assets.end())
			m_Assets.erase(it);
		else
			OE_CORE_ASSERT(false, "Asset is not owned by this AssetResource!");
	}
}