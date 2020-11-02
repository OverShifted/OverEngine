#pragma once

#include "OverEngine/Core/Core.h"

namespace OverEngine
{
	enum class AssetType
	{
		None = 0, Folder, Texture2D, Scene
	};

	class AssetCollection;

	class FolderAsset;
	class Texture2DAsset;
	class SceneAsset;

	class Asset
	{
	public:
		// Loads and assets based on a .meta file
		// To be used in OverEditor
		static Ref<Asset> Load(const String& path, bool isPhysicalPath, const String& assetsDirectoryRoot, AssetCollection* collection = nullptr);
	public:
		Asset() = default;
		virtual ~Asset() = default;

		Ref<Asset> GetParent() const;

		inline const String& GetName() const { return m_Name; }
		inline const String& GetPath() const { return m_Path; }

		inline bool IsFolder() const { return m_Type == AssetType::Folder; }

		inline const uint64_t& GetGuid() const { return m_Guid; }
		inline void SetGuid(const uint64_t& guid) { m_Guid = guid; }

		inline const AssetType& GetType() const { return m_Type; }

		FolderAsset* GetFolderAsset();
		Texture2DAsset* GetTexture2DAsset();
		SceneAsset* GetSceneAsset();
	protected:
		AssetType m_Type = AssetType::None;
		String m_Name;
		String m_Path;
		uint64_t m_Guid;

		AssetCollection* m_Collection = nullptr;

		friend class AssetCollection;
	};
}
