#include "pcheader.h"
#include "Asset.h"

#include "OverEngine/Core/AssetManagement/AssetDatabase.h"

namespace OverEngine
{
	void Asset::SetGuid(const uint64_t& guid)
	{
		Ref<Asset> me = AssetDatabase::s_Registry[m_Guid];
		AssetDatabase::s_Registry.erase(m_Guid);
		AssetDatabase::s_Registry[guid] = me;
		m_Guid = guid;
	}

	bool AssetFolder::RenameChild(const String& currentName, const String& newName)
	{
		Ref<Asset> asset = m_Assets[currentName];

		if (!asset)
			return false;

		m_Assets.erase(currentName);
		m_Assets[newName] = asset;

		return false;
	}
}
