#include "pcheader.h"
#include "FolderAsset.h"

#include "OverEngine/Core/String.h"

namespace OverEngine
{
	FolderAsset::FolderAsset(YAML::Node node, const String& assetsDirectoryRoot)
	{
		m_Path = node["Path"].as<String>();
		m_Guid = node["Guid"].as<uint64_t>();
	}

	FolderAsset::FolderAsset(const String& name, const String& path, const uint64_t& guid)
	{
		m_Path = path;
		m_Guid = guid;
	}

	Ref<Asset> FolderAsset::GetChild(const String& name)
	{
		return m_Assets[name];
	}
}
