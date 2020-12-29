#include "pcheader.h"
#include "AssetCollection.h"

#include "OverEngine/Core/FileSystem/FileSystem.h"
#include "OverEngine/Core/Random.h"
#include "OverEngine/Core/Extentions.h"
#include "OverEngine/Core/String.h"
#include <filesystem>

namespace OverEngine
{
	AssetCollection::AssetCollection()
		: m_RootAsset(CreateRef<FolderAsset>("Assets", "/", Random::UInt64()))
	{
		m_RootAsset->m_Collection = this;
	}

	void AssetCollection::InitFromAssetsDirectory(const String& assetsDirectoryPath, const uint64_t& assetsDirectoryGuid)
	{
		m_RootAsset->SetGuid(assetsDirectoryGuid);

		for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsDirectoryPath))
		{
			auto path = entry.path().string();
			auto extention = FileSystem::ExtractFileExtentionFromPath(path);

			if (extention == OE_META_ASSET_FILE_EXTENSION)
			{
				if (auto asset = Asset::Load(path, true, assetsDirectoryPath, nullptr))
					AddAsset(asset, true);
			}
		}
	}

	void AssetCollection::AddAsset(const Ref<Asset> asset, bool makeFolders)
	{
		asset->m_Collection = this;
		m_AllAssets.insert(asset);

		if (!makeFolders)
		{
			asset->GetParent()->GetFolderAsset()->GetAssets()[asset->GetName()] = asset;
			return;
		}

		auto nodesNames = SplitString(FileSystem::FixPath(asset->GetPath()), '/');

		uint32_t currentPathNodeIndex = 0;
		Ref<FolderAsset> currentAsset = m_RootAsset;

		for (const String& nodeName : *nodesNames)
		{
			if (currentAsset->GetAssets().count(nodeName))
			{
				auto& asset = currentAsset->GetAssets()[nodeName];

				if (currentPathNodeIndex == nodesNames->size() - 1)
				{
					if (asset->IsFolder())
					{
						asset->m_Guid = asset->GetGuid();
						return;
					}

					OE_THROW("Asset '{}' already exists!", nodeName);
					return;
				}
				
				if (asset->IsFolder())
				{
					currentAsset = TYPE_PAWN(asset, Ref<FolderAsset>);
				}
				else
				{
					OE_THROW("Invalid asset path '{}' (at least one node '{}' is not a folder)", asset->GetPath(), currentAsset->GetName());
					return;
				}
			}
			else
			{
				if (currentPathNodeIndex == nodesNames->size() - 1)
				{
					currentAsset->GetAssets()[nodeName] = asset;
					return;
				}

				String newFolderAssetPath = currentAsset->GetPath();
				if (newFolderAssetPath[newFolderAssetPath.size() - 1] != '/')
					newFolderAssetPath += '/';
				newFolderAssetPath += nodeName;

				auto newFolderAsset = CreateRef<FolderAsset>(nodeName, newFolderAssetPath, Random::UInt64());
				newFolderAsset->m_Collection = this;

				currentAsset->GetAssets()[nodeName] = newFolderAsset;
				m_AllAssets.insert(newFolderAsset);
				currentAsset = newFolderAsset;
			}

			currentPathNodeIndex++;
		}
	}

	Ref<Asset> AssetCollection::GetAsset(const String& path)
	{
		if (path[0] != '/')
		{
			OE_CORE_ASSERT(false, "Invalid asset path '{}' (must be started with '/')", path);
			return nullptr;
		}

		if (path.size() == 1) // path == "/"
			return m_RootAsset;

		auto nodesNames = SplitString(FileSystem::FixPath(path), '/');

		uint32_t currentPathNodeIndex = 0;
		Ref<FolderAsset> currentAsset = m_RootAsset;

		for (const String& nodeName : *nodesNames)
		{
			if (currentAsset->GetAssets().count(nodeName))
			{
				auto& asset = currentAsset->GetAssets()[nodeName];

				if (currentPathNodeIndex == nodesNames->size() - 1)
					return asset;
				
				if (asset->IsFolder())
				{
					currentAsset = TYPE_PAWN(asset, Ref<FolderAsset>);
				}
				else
				{
					OE_THROW("Invalid asset path '{}' (at least one node '{}' is not a folder)", path, currentAsset->GetName());
					return nullptr;
				}
			}
			else
			{
				OE_THROW("Invalid asset path '{}' (node '{}' not founded)", path, nodeName);
				return nullptr;
			}

			currentPathNodeIndex++;
		}

		return currentAsset;
	}

	Ref<Asset> AssetCollection::GetAsset(const uint64_t& guid)
	{
		for (const auto& asset : m_AllAssets)
		{
			if (asset->GetGuid() == guid)
				return asset;
		}

		OE_THROW("Cant find asset with {0:#x} GUID!", guid);
		return nullptr;
	}

	bool AssetCollection::AssetExists(const String& path)
	{
		if (path[0] != '/')
		{
			OE_CORE_ASSERT(false, "Invalid asset path '{}' (must be started with '/')", path);
			return false;
		}

		if (path.size() == 1) // path == "/"
			return true;

		auto nodesNames = SplitString(FileSystem::FixPath(path), '/');

		uint32_t currentPathNodeIndex = 0;
		Ref<FolderAsset> currentAsset = m_RootAsset;

		for (const String& nodeName : *nodesNames)
		{
			if (currentAsset->GetAssets().count(nodeName))
			{
				auto& asset = currentAsset->GetAssets()[nodeName];

				if (currentPathNodeIndex == nodesNames->size() - 1)
					return true;
				
				if (asset->IsFolder())
					currentAsset = TYPE_PAWN(asset, Ref<FolderAsset>);
				else
					return false;

				break;
			}
			else
			{
				return false;
			}

			currentPathNodeIndex++;
		}

		return false;
	}
}
