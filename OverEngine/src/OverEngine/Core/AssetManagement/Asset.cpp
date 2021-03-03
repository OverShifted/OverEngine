#include "pcheader.h"
#include "Asset.h"

#include "OverEngine/Core/AssetManagement/AssetDatabase.h"

namespace OverEngine
{
	const String& Asset::GetName() const
	{
		static String rootAssetName = "/";
		if (m_Path.PathNodes.empty()) return rootAssetName;

		return m_Path.PathNodes[m_Path.PathNodes.size() - 1];
	}

	void Asset::SetGuid(const uint64_t& guid)
	{
		Ref<Asset> me = AssetDatabase::s_Registry[m_Guid];
		AssetDatabase::s_Registry.erase(m_Guid);
		AssetDatabase::s_Registry[guid] = me;
		m_Guid = guid;
	}

	bool AssetFolder::RenameChild(const String& currentName, const String& newName)
	{
		AssetMappingKey k;
		k.Name = currentName;

		Ref<Asset> asset = nullptr;

		k.Type = AssetMappingKey::AssetMappingKeyType_Folder;
		if (STD_MAP_HAS(m_Assets, k))
			asset = m_Assets[k];

		if (!asset)
		{
			k.Type = AssetMappingKey::AssetMappingKeyType_NonFolder;
			if (STD_MAP_HAS(m_Assets, k))
				asset = m_Assets[k];
		}

		if (!asset)
			return false;

		m_Assets.erase(k);
		k.Name = newName;
		m_Assets[k] = asset;

		return false;
	}

	Ref<Asset> AssetFolder::operator[] (const String& name)
	{
		AssetMappingKey k;
		k.Name = name;
		
		Ref<Asset> asset = nullptr;

		k.Type = AssetMappingKey::AssetMappingKeyType_Folder;
		if (STD_MAP_HAS(m_Assets, k))
			asset = m_Assets[k];

		if (!asset)
		{
			k.Type = AssetMappingKey::AssetMappingKeyType_NonFolder;
			if (STD_MAP_HAS(m_Assets, k))
				asset = m_Assets[k];
		}

		return asset;
	}
}
