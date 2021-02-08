#pragma once

#include "OverEngine/Core/Core.h"

namespace OverEngine
{
	enum class AssetType
	{
		None = 0,
		Folder,
		Scene,
		Texture2D
	};

	#define OE_ASSET_CLASS(T)                                                                      \
		public:                                                                                    \
			static constexpr AssetType GetStaticAssetType() { return AssetType::T; }               \
			virtual AssetType GetAssetType() const { return GetStaticAssetType(); }

	/**
	 *     Example derived class from Asset
	 *
	 *     class MyAsset : public Asset
	 *     {
	 *         OE_ASSET_CLASS(MyAsset)
	 *
	 *     public:
	 *         MyReturnType MyMethod();
	 *     };
	 * 
	 *     Ref<MyAsset> myAsset;
	 */

	class AssetCollection;
	class FolderAsset;

	class Asset
	{
	public:
		// Loads and assets based on a .meta file
		// To be used in OverEditor
		static Ref<Asset> Load(const String& path, bool isPhysicalPath, const String& assetsDirectoryRoot, AssetCollection* collection = nullptr);
	public:
		Asset() = default;
		virtual ~Asset() = default;

		virtual AssetType GetAssetType() const = 0;
		inline bool IsFolder() const { return GetAssetType() == AssetType::Folder; }

		virtual bool IsPlaceholder() const = 0;

		Ref<FolderAsset> GetParent() const;

		const std::string_view GetName() const;
		const String& GetPath() const { return m_Path; }

		void Rename(const String& newName);
		void Move(const String& newPath);

		inline const uint64_t& GetGuid() const { return m_Guid; }
		inline void SetGuid(const uint64_t& guid) { m_Guid = guid; }

		virtual void Reload() {}
	protected:
		String m_Path;
		uint64_t m_Guid;

		AssetCollection* m_Collection = nullptr;

		friend class AssetCollection;
	};
}
