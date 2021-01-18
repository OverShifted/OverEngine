#include "pcheader.h"
#include "Asset.h"

#include "AssetCollection.h"

#include "Texture2DAsset.h"
#include "SceneAsset.h"

#include "OverEngine/Core/Serialization/Serializer.h"
#include "OverEngine/Core/FileSystem/FileSystem.h"
#include <filesystem>

namespace OverEngine
{
	Ref<Asset> Asset::Load(const String& path, bool isPhysicalPath, const String& assetsDirectoryRoot, AssetCollection* collection)
	{
		YAML::Node node = YAML::LoadFile(isPhysicalPath ? path : assetsDirectoryRoot + "/" + path.substr(9, path.size()));

		if (!Serializer::GlobalEnumExists("AssetType"))
		{
			Serializer::DefineGlobalEnum("AssetType", {
				{ 0, "None" },
				{ 1, "Folder" },
				{ 2, "Texture2D" },
				{ 3, "Scene" }
			});
		}

		Ref<Asset> asset = nullptr;

		switch ((AssetType)Serializer::GetGlobalEnumValue("AssetType", node["Type"].as<String>()))
		{
		case AssetType::Folder:    asset = CreateRef<FolderAsset>   (node, assetsDirectoryRoot); break;
		case AssetType::Texture2D: asset = CreateRef<Texture2DAsset>(node, assetsDirectoryRoot); break;
		case AssetType::Scene:     asset = CreateRef<SceneAsset>    (node, assetsDirectoryRoot); break;
		
		default: OE_CORE_ASSERT(false, "Unknown asset type"); return nullptr;
		}

		if (asset && collection)
		{
			collection->RemoveAsset(asset->GetPath());
			collection->AddAsset(asset, true);
		}

		return asset;
	}

	Ref<Asset> Asset::GetParent() const
	{
		if (m_Path == "/")
			return nullptr;

		OE_CORE_ASSERT(m_Collection, "Asset is not owned by a collection");

		auto lastSlash = m_Path.find_last_of('/');
		return m_Collection->GetAsset(m_Path.substr(0, lastSlash + 1));
	}

	FolderAsset* Asset::GetFolderAsset()
	{
		if (m_Type != AssetType::Folder)
		{
			OE_CORE_ASSERT(false, "Cannot convert this asset to FolderAsset");
			return nullptr;
		}

		return (FolderAsset*)this;
	}

	Texture2DAsset* Asset::GetTexture2DAsset()
	{
		if (m_Type != AssetType::Texture2D)
		{
			OE_CORE_ASSERT(false, "Cannot convert this asset to Texture2DAsset");
			return nullptr;
		}

		return (Texture2DAsset*)this;
	}

	SceneAsset* Asset::GetSceneAsset()
	{
		if (m_Type != AssetType::Scene)
		{
			OE_CORE_ASSERT(false, "Cannot convert this asset to SceneAsset");
			return nullptr;
		}

		return (SceneAsset*)this;
	}
}
