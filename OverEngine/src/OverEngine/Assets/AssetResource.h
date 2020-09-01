#pragma once

#include "Asset.h"
#include <OverEngine/Core/GUIDGenerator.h>

#include <json.hpp>

namespace OverEngine
{
	enum class AssetResourceType
	{
		None = 0, Texture2D
	};

	class Resource
	{
	public:
		Resource() = default;
		Resource(const String& name, const String& virtualPath, AssetResourceType type);

		// Loads resource and assets based on a .oea file
		// To be used in OverEditor
		Resource(const String& virtualPath, const String& assetsDirectoryRoot);

		void AddAsset(const Ref<Asset>& asset);
		void RemoveAsset(const Ref<Asset>& asset);

		inline const String& GetName() const { return m_Name; }
		inline const String& GetVirtualPath() const { return m_VirtualPath; }
	private:
		String m_Name;
		String m_VirtualPath;
		Guid m_Guid;

		AssetResourceType m_Type = AssetResourceType::None;
		Vector<Ref<Asset>> m_Assets;
	};

	struct ResourceCollectionTreeNode
	{
		ResourceCollectionTreeNode() = default;
		~ResourceCollectionTreeNode() {}

		String Name;
		bool IsDirectory;

		Vector<ResourceCollectionTreeNode> Children;
		OverEngine::Resource Resource;
	};

	class ResourceCollection
	{
	public:
		ResourceCollection();

		void InitFromJson(nlohmann::json json, const String& assetsDirectoryRoot);

		void AddResource(const Resource& resource);
		void AddResource(const Resource& resource, const String& path);


		Resource& GetResource(const String& path);

		// Returns the matching node with the path
		// use 'assets://' to get root nodes
		ResourceCollectionTreeNode& GetNode(const String& path);

		bool NodeExists(const String& path);
	private:
		ResourceCollectionTreeNode m_RootResourceTreeNode;
	};
}
