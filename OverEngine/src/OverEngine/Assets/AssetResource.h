#pragma once

#include "Asset.h"

namespace OverEngine
{
	enum class AssetResourceType
	{
		None = 0, Texture2D
	};

	class AssetResource
	{
		AssetResource() = default;
		AssetResource(const String& name, const String& virtualPath, AssetResourceType type = AssetResourceType::None);

		// LoadFromAssetManifestFile
		// To be used in OverEditor
		AssetResource(const String& filePath);

		void AddAsset(const Ref<Asset>& asset);
		void RemoveAsset(const Ref<Asset>& asset);
	private:
		String m_Name;
		String m_VirtualPath;

		AssetResourceType m_Type = AssetResourceType::None;
		Vector<Ref<Asset>> m_Assets;
	};

	struct AssetResourceTreeNode
	{
		AssetResourceTreeNode() = default;
		~AssetResourceTreeNode() {}

		String Name;
		bool IsDirectory;

		union
		{
			Vector<AssetResourceTreeNode> Children;
			AssetResource Resource;
		};
	};

	class AssetResourceCollection
	{
	public:
		AssetResourceCollection() = default;

		void AddResource(const AssetResource& resource);
		void AddResource(const AssetResource& resource, const String& path);

		AssetResource& GetResource(const String& path);
	private:
		Vector<AssetResourceTreeNode> m_RootResourceTreeNodes;
	};
}