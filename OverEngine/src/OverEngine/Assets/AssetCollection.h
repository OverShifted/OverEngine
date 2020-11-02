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

		void AddAsset(const Ref<Asset> resource, bool loading = false);
		void AddAsset(const Ref<Asset> resource, const String& path, bool loading = false);

		// Returns the matching Asset with the path
		// use 'assets://' to get the root Asset
		Ref<Asset> GetAsset(const String& path);

		// Returns the matching Asset with the path
		inline Ref<Asset> GetAsset(const uint64_t& guid);

		bool AssetExists(const String& path);
	private:
		Ref<FolderAsset> m_RootAsset;
	};
}
