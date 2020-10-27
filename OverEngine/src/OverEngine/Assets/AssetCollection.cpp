#include "pcheader.h"
#include "AssetCollection.h"

#include "OverEngine/Core/FileSystem/FileSystem.h"
#include "OverEngine/Core/GUIDGenerator.h"
#include "OverEngine/Core/Extentions.h"
#include "OverEngine/Core/String.h"
#include <filesystem>

namespace OverEngine
{
	AssetCollection::AssetCollection()
		: m_RootAsset(CreateRef<FolderAsset>("Assets", "/", GUIDGenerator::GenerateVersion4()))
	{
		m_RootAsset->m_Collection = this;
	}

	void AssetCollection::InitFromAssetsDirectory(const String& assetsDirectoryPath, const Guid& assetsDirectoryGuid)
	{
		m_RootAsset->SetGuid(assetsDirectoryGuid);

		for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsDirectoryPath))
		{
			auto path = entry.path().string();
			auto extention = FileSystem::ExtractFileExtentionFromPath(path);

			if (extention == OE_META_ASSET_FILE_EXTENSION)
			{
				if (auto asset = Asset::Load(path, true, assetsDirectoryPath, this))
					AddAsset(asset, true);
			}
		}
	}

	void AssetCollection::AddAsset(const Ref<Asset> asset, bool loading)
	{
		AddAsset(asset, asset->GetPath(), loading);
	}

	void AssetCollection::AddAsset(const Ref<Asset> asset, const String& path, bool loading)
	{
		auto lastSlash = path.find_last_of("/");

		auto nodesNames = SplitString(path, '/');

		Ref<FolderAsset> currentAsset = m_RootAsset;
		bool missing = false;

		uint32_t currentPathNodeIndex = 0;
		for (const String& nodeName : *nodesNames)
		{
			if (currentPathNodeIndex == nodesNames->size() - 1)
				break;
			currentPathNodeIndex++;

			if (!missing)
			{
				bool founded = false;
				for (const auto& child : currentAsset->GetAssets())
				{
					if (child->GetName() == nodeName)
					{
						if (child->IsFolder())
							currentAsset = TYPE_PAWN(child, Ref<FolderAsset>);
						founded = true;
						break;
					}
				}

				if (!founded)
				{
					auto newAsset = CreateRef<FolderAsset>(currentAsset->GetPath() + "/" + nodeName, GUIDGenerator::GenerateVersion4());
					currentAsset->GetAssets().push_back(newAsset);
					currentAsset = newAsset;
					currentAsset->m_Collection = this;
					missing = true;
				}
			}
			else
			{
				auto newAsset = CreateRef<FolderAsset>(currentAsset->GetPath() + "/" + nodeName, GUIDGenerator::GenerateVersion4());
				currentAsset->GetAssets().push_back(newAsset);
				currentAsset = newAsset;
				currentAsset->m_Collection = this;
			}
		}

		auto parentAsset = GetAsset(path.substr(0, lastSlash + 1));
		auto& parentAssetChildren = TYPE_PAWN(parentAsset, Ref<FolderAsset>)->GetAssets();

		for (auto& a : parentAssetChildren)
		{
			if (a->GetName() == asset->GetName())
			{
				if (!loading)
				{
					OE_CORE_ASSERT(false, "Asset already exists!");
					return;
				}

				//a->m_Type = asset->m_Type;
				a->m_Path = asset->m_Path;
				a->m_Guid = asset->m_Guid;
				return;
			}
		}

		asset->m_Collection = this;
		parentAssetChildren.push_back(asset);
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

		auto nodesNames = SplitString(FileSystem::FixFileSystemPath(path), '/');

		uint32_t currentPathNodeIndex = 0;
		Ref<FolderAsset> currentAsset = m_RootAsset;
		for (const String& nodeName : *nodesNames)
		{
			bool founded = false;
			for (const auto& child : currentAsset->GetAssets())
			{
				if (child->GetName() == nodeName)
				{
					if (currentPathNodeIndex == nodesNames->size() - 1)
					{
						return child;
					}
					else if (child->IsFolder())
					{
						currentAsset = TYPE_PAWN(child, Ref<FolderAsset>);
						founded = true;
					}
					else
					{
						OE_CORE_ASSERT(false, "Invalid asset path '{}' (at least one node '{}' is not a folder)", path, currentAsset->GetName());
						return nullptr;
					}
					break;
				}
			}

			if (!founded)
			{
				OE_CORE_ASSERT(false, "Invalid asset path '{}' (node '{}' not founded)", path, nodeName);
				return nullptr;
			}

			currentPathNodeIndex++;
		}

		return nullptr;
	}

	static Ref<Asset> FindAssetByGuidRecursive(Ref<Asset> asset, const Guid& guid)
	{
		if (asset->GetGuid() == guid)
			return asset;

		if (!asset->IsFolder())
			return nullptr;

		for (const auto& child : TYPE_PAWN(asset, Ref<FolderAsset>)->GetAssets())
			if (auto result = FindAssetByGuidRecursive(child, guid))
				return result;

		return nullptr;
	};

	inline Ref<Asset> AssetCollection::GetAsset(const Guid& guid)
	{
		return FindAssetByGuidRecursive(m_RootAsset, guid);
	}

	bool AssetCollection::AssetExists(const String& path)
	{
		if (path[0] != '/')
		{
			OE_CORE_ASSERT(false, "Invalid asset path '{}' (must be started with '/')", path);
			return false;
		}

		auto nodesNames = SplitString(path, '/');

		uint32_t currentPathNodeIndex = 0;
		Ref<FolderAsset> currentAsset = m_RootAsset;
		for (const String& nodeName : *nodesNames)
		{
			for (const auto& child : currentAsset->GetAssets())
			{
				if (child->GetName() == nodeName)
				{
					if (currentPathNodeIndex == nodesNames->size() - 1)
						return true;
					else if (child->IsFolder())
						currentAsset = TYPE_PAWN(child, Ref<FolderAsset>);
					else
						return false;
					break;
				}
			}

			currentPathNodeIndex++;
		}

		return true;
	}
}
