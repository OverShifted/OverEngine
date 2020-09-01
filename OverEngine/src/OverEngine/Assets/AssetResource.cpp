#include "pcheader.h"
#include "AssetResource.h"

#include <OverEngine/Core/FileSystem/FileSystem.h>

namespace OverEngine
{
	Resource::Resource(const String& name, const String& virtualPath, AssetResourceType type)
		: m_Name(name), m_VirtualPath(virtualPath), m_Type(type)
	{
	}

	Resource::Resource(const String& virtualPath, const String& assetsDirectoryRoot)
	{
		nlohmann::json assetFileJson;
		std::ifstream assetFile(assetsDirectoryRoot + "/" + virtualPath.substr(9, virtualPath.size()));
		assetFile >> assetFileJson;
		assetFile.close();

		m_Name = assetFileJson["Name"];
		m_VirtualPath = assetFileJson["FilePath"];
		m_Guid = Guid(assetFileJson["Guid"]);

		for (const auto& assetJson : assetFileJson["Assets"])
		{
			if (assetJson["TypeName"] == "MasterTexture")
			{
				OE_CORE_INFO("Generating MasterTexture Asset");
				const auto& filePathJson = assetJson["FilePath"].get<String>();
				auto fileToLoadPath = assetsDirectoryRoot + "/" + filePathJson.substr(9, filePathJson.size());
				auto texture = Texture2D::MasterFromFile(fileToLoadPath);
				auto asset = CreateRef<Texture2DAsset>(assetJson["Name"], this, texture);
				m_Assets.push_back(asset);
			}
		}
	}

	void Resource::AddAsset(const Ref<Asset>& asset)
	{
		m_Assets.push_back(asset);
	}

	void Resource::RemoveAsset(const Ref<Asset>& asset)
	{
		auto it = std::find(m_Assets.begin(), m_Assets.end(), asset);
		if (it != m_Assets.end())
			m_Assets.erase(it);
		else
			OE_CORE_ASSERT(false, "Asset is not owned by this AssetResource!");
	}


	ResourceCollection::ResourceCollection()
	{
		m_RootResourceTreeNode.IsDirectory = true;
		//m_RootResourceTreeNode.
	}

	void ResourceCollection::InitFromJson(nlohmann::json json, const String& assetsDirectoryRoot)
	{
		for (const auto& asset : json)
		{
			const auto& path = asset["Path"].get<String>();
			AddResource(Resource(path, assetsDirectoryRoot));
		}
	}

	void ResourceCollection::AddResource(const Resource& resource)
	{
		AddResource(resource, resource.GetVirtualPath());
	}

	void ResourceCollection::AddResource(const Resource& resource, const String& path)
	{
		auto lastSlash = path.find_last_of("/\\");

		ResourceCollectionTreeNode node;

		node.IsDirectory = false;
		node.Name = resource.GetName();
		node.Resource = resource;

		auto nodesNames = SplitString(FileSystem::FixFileSystemPath(path.substr(9, path.size())), '/');

		ResourceCollectionTreeNode* currentNode = &m_RootResourceTreeNode;
		bool missing = false;
		for (const auto& nodeName : *nodesNames)
		{
			if (!missing)
			{
				bool founded = false;
				for (auto& child : currentNode->Children)
				{
					if (child.Name == nodeName)
					{
						currentNode = &child;
						founded = true;
						break;
					}
				}

				if (!founded)
				{
					ResourceCollectionTreeNode nodeToAdd;
					nodeToAdd.IsDirectory = true;
					nodeToAdd.Name = nodeName;
					currentNode->Children.push_back(nodeToAdd);
					currentNode = &currentNode->Children[currentNode->Children.size() - 1];
					missing = true;
				}
			}
			else
			{
				ResourceCollectionTreeNode nodeToAdd;
				nodeToAdd.IsDirectory = true;
				nodeToAdd.Name = nodeName;
				currentNode->Children.push_back(nodeToAdd);
				currentNode = &currentNode->Children[currentNode->Children.size() - 1];
			}
		}

		GetNode(path.substr(0, lastSlash)).Children.push_back(node);
	}

	Resource& ResourceCollection::GetResource(const String& path)
	{
		auto& node = GetNode(path);

		if (node.IsDirectory)
		{
			OE_CORE_ASSERT(false, "Entry '{}' is a resource directory instead of a resource", path);
			return NULL_REF(Resource);
		}

		return node.Resource;
	}

	ResourceCollectionTreeNode& ResourceCollection::GetNode(const String& path)
	{
		if (std::string_view(path.c_str(), 9) != "assets://")
		{
			OE_CORE_ASSERT(false, "Invalid resource path '{}' (must be started with 'assets://')", path);
			return NULL_REF(ResourceCollectionTreeNode);
		}

		if (path.size() == 9) // path == "assets://"
		{
			return m_RootResourceTreeNode;
		}

		auto nodesNames = SplitString(FileSystem::FixFileSystemPath(path.substr(9, path.size())), '/');

		ResourceCollectionTreeNode* currentNode = &m_RootResourceTreeNode;
		for (const auto& nodeName : *nodesNames)
		{
			if (!currentNode->IsDirectory)
			{
				OE_CORE_ASSERT(false, "Invalid resource path '{}' (at least one node '{}' is not a directory)", path, currentNode->Name)
				return NULL_REF(ResourceCollectionTreeNode);
			}

			bool founded = false;
			for (auto& child : currentNode->Children)
			{
				if (child.Name == nodeName)
				{
					currentNode = &child;
					founded = true;
					break;
				}
			}

			if (!founded)
			{
				OE_CORE_ASSERT(false, "Invalid resource path '{}' (node '{}' not founded)", path, nodeName);
				return NULL_REF(ResourceCollectionTreeNode);
			}
		}

		return *currentNode;
	}

	bool ResourceCollection::NodeExists(const String& path)
	{
		if (std::string_view(path.c_str(), 9) != "assets://")
		{
			OE_CORE_ASSERT(false, "Invalid resource path '{}' (must be started with 'assets://')", path);
			return false;
		}

		auto nodesNames = SplitString(FileSystem::FixFileSystemPath(path.substr(9, path.size())), '/');

		ResourceCollectionTreeNode* currentNode = &m_RootResourceTreeNode;
		for (const auto& nodeName : *nodesNames)
		{
			if (!currentNode->IsDirectory)
				return false;

			bool founded = false;
			for (auto& child : currentNode->Children)
			{
				if (child.Name == nodeName)
				{
					currentNode = &child;
					founded = true;
					break;
				}
			}

			if (!founded)
				return false;
		}

		return true;
	}
}
