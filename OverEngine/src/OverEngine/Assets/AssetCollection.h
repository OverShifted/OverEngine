#pragma once

#include "Asset.h"
#include "FolderAsset.h"

namespace OverEngine
{
	class AssetCollection
	{
	public:
		AssetCollection();

		void InitFromAssetsDirectory(const String& assetsDirectoryPath, const uint64_t& assetsDirectoryGuid);

		void AddAsset(const Ref<Asset> asset, bool makeFolders = false);

		// Returns the matching Asset with the path
		// use 'assets://' to get the root Asset
		Ref<Asset> GetAsset(const String& path);

		// Returns the matching Asset with the path
		Ref<Asset> GetAsset(const uint64_t& guid);

		bool AssetExists(const String& path);
	private:
		Ref<FolderAsset> m_RootAsset;
		std::unordered_set<Ref<Asset>> m_AllAssets;
	};
}
