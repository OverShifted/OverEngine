#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/AssetManagement/Asset.h"

namespace OverEngine
{
	class AssetDatabase
	{
	public:
		static void Init();
		static bool Init(const String& assetDirectoryPath);

		static void Clear();
		static void Refresh();

		inline static Ref<AssetFolder> GetRoot() { return s_Root; }
		static bool CreateFolder(const String& parentFolderPath, const String& newFolderName, bool createMissingParents = true) { OE_CORE_ASSERT(false, "Not Implemented"); return false; }
		static bool CreateAsset(const String& path, const Ref<Asset>& asset);

		static bool Delete(const String& path) { OE_CORE_ASSERT(false, "Not Implemented"); return false; }

		static Ref<Asset> GetAssetByPath(const String& path);

		template <typename T>
		static Ref<T> RegisterAndGet(const uint64_t& guid)
		{
			auto it = s_Registry.find(guid);
			if (it != s_Registry.end())
				return std::dynamic_pointer_cast<T>(it->second);

			auto asset = T::Create(guid);
			s_Registry[guid] = asset;
			return asset;
		}

	private:
		static UnorderedMap<uint64_t, Ref<Asset>> s_Registry;
		static Ref<AssetFolder> s_Root;

		friend class Asset;
	};
}
