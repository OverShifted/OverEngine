#include "pcheader.h"
#include "FolderAsset.h"

#include "OverEngine/Core/String.h"

namespace OverEngine
{
	FolderAsset::FolderAsset(const String& path, const Guid& guid)
	{
		m_Type = AssetType::Folder;
		auto splitted = SplitString(path, '/');
		m_Name = (*splitted)[splitted->size() - 1];
		m_Path = path;
		m_Guid = guid;
	}

	FolderAsset::FolderAsset(const String& name, const String& path, const Guid& guid)
	{
		m_Type = AssetType::Folder;
		m_Name = name;
		m_Path = path;
		m_Guid = guid;
	}
}
