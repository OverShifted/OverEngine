#include "pcheader.h"
#include "FolderAsset.h"

#include "OverEngine/Core/String.h"

namespace OverEngine
{
	FolderAsset::FolderAsset(YAML::Node node, const String& assetsDirectoryRoot)
	{
		m_Type = AssetType::Folder;
		m_Name = node["Name"].as<String>();
		m_Path = node["Path"].as<String>();
		m_Guid = node["Guid"].as<uint64_t>();
	}

	FolderAsset::FolderAsset(const String& path, const uint64_t& guid)
	{
		m_Type = AssetType::Folder;

		Vector<String> splitted;
		SplitString(path, '/', splitted);
		m_Name = splitted[splitted.size() - 1];

		m_Path = path;
		m_Guid = guid;
	}

	FolderAsset::FolderAsset(const String& name, const String& path, const uint64_t& guid)
	{
		m_Type = AssetType::Folder;
		m_Name = name;
		m_Path = path;
		m_Guid = guid;
	}
}
