#include "pcheader.h"
#include "AssetResource.h"

#include <OverEngine/Core/FileSystem/FileSystem.h>
#include <filesystem>

namespace OverEngine
{
	Resource::Resource(const String& name, const String& virtualPath, ResourceType type)
		: m_Name(name), m_VirtualPath(virtualPath), m_Type(type)
	{
	}

	Resource::Resource(const String& path, const String& assetsDirectoryRoot, bool isPhysicalPath)
	{
		nlohmann::json assetFileJson;
		std::ifstream assetFile;

		if (isPhysicalPath)
			assetFile.open(path);
		else
			assetFile.open(assetsDirectoryRoot + "/" + path.substr(9, path.size()));

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
				AddResource(CreateRef<Resource>(path, assetsDirectoryPath, true));
			}
		}
	}

	void ResourceCollection::AddResource(const Ref<Resource> resource)
	{
		AddResource(resource, resource->GetVirtualPath());
	}

	void ResourceCollection::AddResource(const Ref<Resource> resource, const String& path)
	{
		auto lastSlash = path.find_last_of("/\\");

		auto nodesNames = SplitString(FileSystem::FixFileSystemPath(path.substr(9, path.size())), '/');

		Ref<Resource> currentResource = m_RootResource;
		bool missing = false;
		for (const auto& nodeName : *nodesNames)
		{
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
					currentResource->GetChildren().push_back(CreateRef<Resource>(nodeName, currentResource->GetVirtualPath() + "/" + nodeName, ResourceType::Directory));
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

		GetResource(path.substr(0, lastSlash))->GetChildren().push_back(resource);
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

	bool ResourceCollection::NodeExists(const String& path)
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
