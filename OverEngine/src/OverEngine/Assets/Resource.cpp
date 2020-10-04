#include "pcheader.h"
#include "Resource.h"

#include "OverEngine/Renderer/Texture.h"
#include "OverEngine/Scene/Scene.h"

#include <OverEngine/Core/FileSystem/FileSystem.h>
#include <filesystem>

namespace OverEngine
{
	Resource::Resource(const String& name, const String& virtualPath, ResourceType type, ResourceCollection* collection)
		: m_Type(type), m_Name(name), m_VirtualPath(virtualPath), m_Guid(GUIDGenerator::GenerateVersion4()), m_Collection(collection)
	{
	}

	Resource::Resource(const String& path, const String& assetsDirectoryRoot, bool isPhysicalPath, ResourceCollection* collection)
		: m_Collection(collection)
	{
		nlohmann::json resourceJson;
		std::ifstream resourceFile;

		if (isPhysicalPath)
			resourceFile.open(path);
		else
			resourceFile.open(assetsDirectoryRoot + "/" + path.substr(9, path.size()));

		resourceFile >> resourceJson;
		resourceFile.close();

		m_Name = resourceJson["Name"];
		m_VirtualPath = resourceJson["FilePath"];
		m_Guid = Guid(resourceJson["Guid"]);

		const String& typeName = resourceJson["TypeName"].get<String>();
		if (typeName == "Directory")
			m_Type = ResourceType::Directory;
		else if (typeName == "Texture2D")
			m_Type = ResourceType::Texture2D;
		else if (typeName == "Scene")
			m_Type = ResourceType::Scene;
		else
			m_Type = ResourceType::None;

		for (const auto& assetJson : resourceJson["Assets"])
		{
			if (assetJson["TypeName"] == "MasterTexture")
			{
				OE_CORE_INFO("Generating MasterTexture Asset");
				auto fileToLoadPath = assetsDirectoryRoot + "/" + m_VirtualPath.substr(9, m_VirtualPath.size());
				auto texture = Texture2D::MasterFromFile(fileToLoadPath);
				auto asset = CreateRef<Texture2DAsset>(assetJson["Name"], this, texture);
				m_Assets.push_back(asset);
			}
			else if (assetJson["TypeName"] == "Scene")
			{
				OE_CORE_INFO("Generating Scene Asset");
				auto fileToLoadPath = assetsDirectoryRoot + "/" + m_VirtualPath.substr(9, m_VirtualPath.size());
				auto asset = CreateRef<SceneAsset>(assetJson["Name"], this, LoadSceneFromFile(fileToLoadPath));
				m_Assets.push_back(asset);
			}
		}
	}

	Ref<Resource> Resource::GetParent() const
	{
		if (m_VirtualPath == "assets://")
			return nullptr;

		OE_CORE_ASSERT(m_Collection, "Resource is not owned by a collection");

		auto lastSlash = m_VirtualPath.find_last_of("/\\");
		auto parentPath = m_VirtualPath.substr(0, lastSlash);
		if (parentPath == "assets:/")
			parentPath += "/";

		return m_Collection->GetResource(parentPath);
	}

	void Resource::AddAsset(const Ref<Asset>& asset)
	{
		OE_CORE_ASSERT(!IsDirectory(), "directory Resources dont have assets!");
		m_Assets.push_back(asset);
	}

	void Resource::RemoveAsset(const Ref<Asset>& asset)
	{
		OE_CORE_ASSERT(!IsDirectory(), "directory Resources dont have assets!");
		auto it = std::find(m_Assets.begin(), m_Assets.end(), asset);
		if (it != m_Assets.end())
			m_Assets.erase(it);
		else
			OE_CORE_ASSERT(false, "Asset is not owned by this AssetResource!");
	}

	ResourceCollection::ResourceCollection()
		: m_RootResource(CreateRef<Resource>("Assets", "assets://", ResourceType::Directory))
	{
	}

	void ResourceCollection::InitFromAssetsDirectory(const String& assetsDirectoryPath, const Guid& assetsDirectoryGuid)
	{
		m_RootResource->SetGuid(assetsDirectoryGuid);

		for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsDirectoryPath))
		{
			auto path = entry.path().string();
			auto name = FileSystem::ExtractFileNameFromPath(path);
			auto extention = FileSystem::ExtractFileExtentionFromName(name);

			if (extention == "oerd")
			{
				AddResource(CreateRef<Resource>(path, assetsDirectoryPath, true, this), true);
			}
		}
	}

	void ResourceCollection::AddResource(const Ref<Resource> resource, bool loading)
	{
		AddResource(resource, resource->GetVirtualPath(), loading);
	}

	void ResourceCollection::AddResource(const Ref<Resource> resource, const String& path, bool loading)
	{
		auto lastSlash = path.find_last_of("/\\");

		auto nodesNames = SplitString(FileSystem::FixFileSystemPath(path.substr(9, path.size())), '/');

		Ref<Resource> currentResource = m_RootResource;
		bool missing = false;

		uint32_t currentPathNodeIndex = 0;
		for (const auto& nodeName : *nodesNames)
		{
			if (currentPathNodeIndex == nodesNames->size() - 1)
				break;
			currentPathNodeIndex++;

			if (!missing)
			{
				bool founded = false;
				for (const auto& child : currentResource->GetChildren())
				{
					if (child->GetName() == nodeName)
					{
						currentResource = child;
						founded = true;
						break;
					}
				}

				if (!founded)
				{
					currentResource->GetChildren().push_back(CreateRef<Resource>(nodeName, currentResource->GetVirtualPath() + "/" + nodeName, ResourceType::Directory, this));
					auto& currentResourceChildren = currentResource->GetChildren();
					currentResource = currentResourceChildren[currentResourceChildren.size() - 1];
					missing = true;
				}
			}
			else
			{
				currentResource->GetChildren().push_back(CreateRef<Resource>(nodeName, currentResource->GetVirtualPath() + "/" + nodeName, ResourceType::Directory));
				auto& currentResourceChildren = currentResource->GetChildren();
				currentResource = currentResourceChildren[currentResourceChildren.size() - 1];
			}
		}

		auto& parentResourceChildren = GetResource(path.substr(0, lastSlash + 1))->GetChildren();

		for (auto& res : parentResourceChildren)
		{
			if (res->GetName() == resource->GetName())
			{
				if (!loading)
				{
					OE_CORE_ASSERT(false, "Resource already exists!");
					return;
				}

				res->m_Type = resource->m_Type;
				res->m_VirtualPath = resource->m_VirtualPath;
				res->m_Guid = resource->m_Guid;
				return;
			}
		}

		resource->m_Collection = this;
		parentResourceChildren.push_back(resource);
	}

	Ref<Resource> ResourceCollection::GetResource(const String& path)
	{
		if (std::string_view(path.c_str(), 9) != "assets://")
		{
			OE_CORE_ASSERT(false, "Invalid resource path '{}' (must be started with 'assets://')", path);
			return nullptr;
		}

		if (path.size() == 9) // path == "assets://"
		{
			return m_RootResource;
		}

		auto nodesNames = SplitString(FileSystem::FixFileSystemPath(path.substr(9, path.size())), '/');

		Ref<Resource> currentResource = m_RootResource;
		for (const auto& nodeName : *nodesNames)
		{
			if (!currentResource->IsDirectory())
			{
				OE_CORE_ASSERT(false, "Invalid resource path '{}' (at least one node '{}' is not a directory)", path, currentResource->GetName())
					return nullptr;
			}

			bool founded = false;
			for (const auto& child : currentResource->GetChildren())
			{
				if (child->GetName() == nodeName)
				{
					currentResource = child;
					founded = true;
					break;
				}
			}

			if (!founded)
			{
				OE_CORE_ASSERT(false, "Invalid resource path '{}' (node '{}' not founded)", path, nodeName);
				return nullptr;
			}
		}

		return currentResource;
	}

	static Ref<Resource> FindResByGuidRecursive(Ref<Resource> res, const Guid& guid)
	{
		if (res->GetGuid() == guid)
			return res;

		if (!res->IsDirectory())
			return nullptr;

		for (const auto& r : res->GetChildren())
			if (auto result = FindResByGuidRecursive(r, guid))
				return result;

		return nullptr;
	};

	inline Ref<Resource> ResourceCollection::GetResource(const Guid& guid)
	{
		return FindResByGuidRecursive(m_RootResource, guid);
	}

	bool ResourceCollection::ResourceExists(const String& path)
	{
		if (std::string_view(path.c_str(), 9) != "assets://")
		{
			OE_CORE_ASSERT(false, "Invalid resource path '{}' (must be started with 'assets://')", path);
			return false;
		}

		auto nodesNames = SplitString(FileSystem::FixFileSystemPath(path.substr(9, path.size())), '/');

		Ref<Resource> currentResource = m_RootResource;
		for (const auto& nodeName : *nodesNames)
		{
			if (!currentResource->IsDirectory())
				return false;

			bool founded = false;
			for (const auto& child : currentResource->GetChildren())
			{
				if (child->GetName() == nodeName)
				{
					currentResource = child;
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
