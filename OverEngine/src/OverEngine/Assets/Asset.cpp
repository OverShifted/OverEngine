#include "pcheader.h"
#include "Asset.h"

#include "AssetCollection.h"

// #include "Texture2DAsset.h"
// #include "SceneAsset.h"
#include "FolderAsset.h"

#include "OverEngine/Core/Serialization/Serializer.h"
#include "OverEngine/Core/FileSystem/FileSystem.h"
#include <filesystem>

namespace OverEngine
{
	Ref<Asset> Asset::Load(const String& path, bool isPhysicalPath, const String& assetsDirectoryRoot, AssetCollection* collection)
	{
	#if 0
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
	#else
		return nullptr;
	#endif
	}

	const std::string_view Asset::GetName() const
	{
		auto lastSlash = m_Path.find_last_of("/\\");
		lastSlash = lastSlash == String::npos ? 0 : lastSlash + 1;
		return std::string_view(m_Path.c_str() + lastSlash, m_Path.size() - lastSlash);
	}

	void Asset::Rename(const String& newName)
	{

	}

	void Asset::Move(const String& newPath)
	{
		
	}
}
