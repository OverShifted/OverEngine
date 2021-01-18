#include "pcheader.h"
#include "AssetCollection.h"

#include "OverEngine/Core/Serialization/YamlConverters.h"
#include "OverEngine/Core/FileSystem/FileSystem.h"
#include "OverEngine/Core/Random.h"
#include "OverEngine/Core/Extentions.h"
#include "OverEngine/Core/String.h"

#include <filesystem>

namespace OverEngine
{
	AssetCollection::AssetCollection()
		: m_RootAsset(CreateRef<FolderAsset>("Assets", "/", Random::UInt64()))
	{
		m_RootAsset->m_Collection = this;
	}

	void AssetCollection::InitFromAssetsDirectory(const String& assetsDirectoryPath, const uint64_t& assetsDirectoryGuid)
	{
		m_AssetsDirectoryPath = assetsDirectoryPath;

		m_RootAsset->SetGuid(assetsDirectoryGuid);

		for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsDirectoryPath))
		{
			auto path = entry.path().string();
			auto extention = FileSystem::ExtractFileExtentionFromPath(path);

			if (extention == OE_META_ASSET_FILE_EXTENSION)
			{
				if (auto asset = Asset::Load(path, true, assetsDirectoryPath, nullptr))
					AddAsset(asset, true);
			}
		}
	}

	#define __CHECK_META_YAML_PARAM_VALIDATION(param, type) isValid &&= node[param].IsDefined() && node[param].Is##type(); if (!isValid) break

	static bool IsYamlNodeValidMetaDocument(const YAML::Node& node)
	{
		if (!YamlCppHelper::IsNodeString(node["Name"]))           return false;
		if (!YamlCppHelper::IsNodeString(node["Path"]))           return false;
		if (!                            node["Guid"].IsScalar()) return false;
		if (!YamlCppHelper::IsNodeString(node["Hash"]))           return false;
		if (!YamlCppHelper::IsNodeString(node["Type"]))           return false;

		return true;
	}

	void AssetCollection::Refresh()
	{
		String path;
		String extention;

		auto ImportAndLoadAndLog = [this, &path](const uint64_t& guid = 0)
		{
			auto asset = ImportAndLoad(path, guid);

			if (!asset)
			{
				OE_CORE_WARN("AssetCollection::Refresh() : Asset {} could not be loaded!", FileSystem::FixPath(path));
				return;
			}

			OE_CORE_INFO("New asset loaded! '{}'", asset->GetPath());
		};

		for (const auto& entry : std::filesystem::recursive_directory_iterator(m_AssetsDirectoryPath))
		{
			path = entry.path().string();
			extention = FileSystem::ExtractFileExtentionFromPath(path);

			if (!extention.empty() && extention != OE_META_ASSET_FILE_EXTENSION)
			{
				String metaFilePath = path + '.' + OE_META_ASSET_FILE_EXTENSION;

				// Check if a .meta file associated with the asset located at the 'path' already exists
				if (!std::filesystem::exists(metaFilePath))
				{
					// Import the asset
					ImportAndLoadAndLog();
				}
				else
				{
					// Check if .meta file is valid

					YAML::Node node;

					try
					{
						node = YAML::LoadFile(metaFilePath);
					}

					// Can't open file or parse it's content
					catch (const std::runtime_error&)
					{
						// So first lets delete the .meta file
						std::filesystem::remove(metaFilePath);

						// And then import the asset
						// Cant pass any guid
						ImportAndLoadAndLog();

						try
						{
							node = YAML::LoadFile(metaFilePath);
						}
						catch (const std::runtime_error&)
						{
							OE_CORE_ASSERT(false, "Cant't, open YAML file after re-import! This is a BUG in the engine.");
							return;
						}
					}

					// Make sure 'node' is a valid OverEngine meta file document
					if (!IsYamlNodeValidMetaDocument(node))
					{
						uint64_t guid = 0;

						try
						{
							guid = node["Guid"].as<uint64_t>();
						}
						catch (const std::runtime_error&) {}

						ImportAndLoadAndLog(guid);
						
						try
						{
							node = YAML::LoadFile(metaFilePath);
						}
						catch (const std::runtime_error&)
						{
							OE_CORE_ASSERT(false, "Cant't, open YAML file after re-import! This is a BUG in the engine.");
							return;
						}
					}

					// Handle asset reloading
					if (auto lastHashNode = node["Hash"]; node && lastHashNode)
					{
						String lastHash = lastHashNode.as<String>();
						String currentHash = FileSystem::HashFile(path);

						if (lastHash != currentHash)
						{
							// Asset has been changed => reload it
							GetAsset(node["Path"].as<String>())->Reload();
						}
					}
				}
			}
		}
	}

	static AssetType FindAssetTypeFromExtension(const String& extension)
	{
		if (extension == "png" || extension == "jpg")
			return AssetType::Texture2D;

		if (extension == OE_SCENE_FILE_EXTENSION)
			return AssetType::Scene;

		return AssetType::None;
	}

	Ref<Asset> AssetCollection::ImportAndLoad(const String& physicalPath, uint64_t guid)
	{
		auto assetPath = FileSystem::FixPath(physicalPath.substr(m_AssetsDirectoryPath.size(), physicalPath.size()));

		#if 0
		if (collection->AssetExists(assetPath))
			return nullptr;
		#endif

		if (guid == 0)
			guid = Random::UInt64();

		if (std::filesystem::is_directory(physicalPath))
		{
			YAML::Emitter emitter;
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Name" << YAML::Value << FileSystem::ExtractFileNameFromPath(physicalPath);
			emitter << YAML::Key << "Path" << YAML::Value << assetPath;
			emitter << YAML::Key << "Guid" << YAML::Value << YAML::Hex << guid;
			emitter << YAML::Key << "Type" << YAML::Value << "Folder";
			emitter << YAML::EndMap;

			String metaFilePath = physicalPath + "." + OE_META_ASSET_FILE_EXTENSION;

			std::ofstream metaFile(metaFilePath);
			metaFile << emitter.c_str();
			metaFile.flush();
			metaFile.close();

			return Asset::Load(metaFilePath, true, m_AssetsDirectoryPath, this);
		}

		auto extension = FileSystem::ExtractFileExtentionFromPath(physicalPath);
		auto type = FindAssetTypeFromExtension(extension);

		switch (type)
		{
		case AssetType::Texture2D:
		{
			YAML::Emitter emitter;

			auto name = FileSystem::ExtractFileNameFromPath(physicalPath);
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Name" << YAML::Value << name;
			emitter << YAML::Key << "Path" << YAML::Value << assetPath;
			emitter << YAML::Key << "Guid" << YAML::Value << YAML::Hex << guid;
			emitter << YAML::Key << "Hash" << YAML::Value << FileSystem::HashFile(physicalPath);
			emitter << YAML::Key << "Type" << YAML::Value << "Texture2D";

			emitter << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;

			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Texture2D" << YAML::Value << YAML::Hex << Random::UInt64();
			emitter << YAML::Key << "Type" << YAML::Value << "Master";
			emitter << YAML::Key << "Name" << YAML::Value << name + "MasterTexture";
			emitter << YAML::EndMap;

			emitter << YAML::EndSeq;

			emitter << YAML::EndMap;

			String metaFilePath = physicalPath + "." + OE_META_ASSET_FILE_EXTENSION;

			std::ofstream metaFile(metaFilePath);
			metaFile << emitter.c_str();
			metaFile.flush();
			metaFile.close();

			return Asset::Load(metaFilePath, true, m_AssetsDirectoryPath, this);
		}

		case AssetType::Scene:
		{
			YAML::Emitter emitter;

			auto name = FileSystem::ExtractFileNameFromPath(physicalPath);
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Name" << YAML::Value << name;
			emitter << YAML::Key << "Path" << YAML::Value << assetPath;
			emitter << YAML::Key << "Guid" << YAML::Value << YAML::Hex << guid;
			emitter << YAML::Key << "Hash" << YAML::Value << FileSystem::HashFile(physicalPath);
			emitter << YAML::Key << "Type" << YAML::Value << "Scene";
			emitter << YAML::EndMap;

			String metaFilePath = physicalPath + "." + OE_META_ASSET_FILE_EXTENSION;
			std::ofstream metaFile(metaFilePath);
			metaFile << emitter.c_str();
			metaFile.flush();
			metaFile.close();

			return Asset::Load(metaFilePath, true, m_AssetsDirectoryPath, this);
		}

		default: break;
		}

		OE_CORE_WARN("Asset {} could not be loaded!", assetPath);
		return nullptr;
	}

	void AssetCollection::AddAsset(const Ref<Asset> asset, bool makeFolders)
	{
		asset->m_Collection = this;
		m_AllAssets.insert(asset);

		if (!makeFolders)
		{
			asset->GetParent()->GetFolderAsset()->GetAssets()[asset->GetName()] = asset;
			return;
		}

		Vector<String> nodesNames;
		SplitString(FileSystem::FixPath(asset->GetPath()), '/', nodesNames);

		uint32_t currentPathNodeIndex = 0;
		Ref<FolderAsset> currentAsset = m_RootAsset;

		for (const String& nodeName : nodesNames)
		{
			if (currentAsset->GetAssets().count(nodeName))
			{
				auto& asset = currentAsset->GetAssets()[nodeName];

				if (currentPathNodeIndex == nodesNames.size() - 1)
				{
					if (asset->IsFolder())
					{
						asset->m_Guid = asset->GetGuid();
						return;
					}

					OE_CORE_ASSERT(false, "Asset '{}' already exists!", nodeName);
					return;
				}
				
				if (asset->IsFolder())
				{
					currentAsset = TYPE_PAWN(asset, Ref<FolderAsset>);
				}
				else
				{
					OE_CORE_ASSERT(false, "Invalid asset path '{}' (at least one node '{}' is not a folder)", asset->GetPath(), currentAsset->GetName());
					return;
				}
			}
			else
			{
				if (currentPathNodeIndex == nodesNames.size() - 1)
				{
					currentAsset->GetAssets()[nodeName] = asset;
					return;
				}

				String newFolderAssetPath = currentAsset->GetPath();
				if (newFolderAssetPath[newFolderAssetPath.size() - 1] != '/')
					newFolderAssetPath += '/';
				newFolderAssetPath += nodeName;

				auto newFolderAsset = CreateRef<FolderAsset>(nodeName, newFolderAssetPath, Random::UInt64());
				newFolderAsset->m_Collection = this;

				currentAsset->GetAssets()[nodeName] = newFolderAsset;
				m_AllAssets.insert(newFolderAsset);
				currentAsset = newFolderAsset;
			}

			currentPathNodeIndex++;
		}
	}

	void AssetCollection::RemoveAsset(const String& path)
	{
		if (!AssetExists(path))
			return;

		auto asset = GetAsset(path);
		auto& parentAssets = asset->GetParent()->GetFolderAsset()->GetAssets();
		parentAssets.erase(asset->GetName());
	}

	Ref<Asset> AssetCollection::GetAsset(const String& path)
	{
		if (path[0] != '/')
		{
			OE_CORE_ASSERT(false, "Invalid asset path '{}' (must be started with '/')", path);
			return nullptr;
		}

		if (path.size() == 1) // path == "/"
			return m_RootAsset;

		Vector<String> nodesNames;
		SplitString(FileSystem::FixPath(path), '/', nodesNames);

		uint32_t currentPathNodeIndex = 0;
		Ref<FolderAsset> currentAsset = m_RootAsset;

		for (const String& nodeName : nodesNames)
		{
			if (currentAsset->GetAssets().count(nodeName))
			{
				auto& asset = currentAsset->GetAssets()[nodeName];

				if (currentPathNodeIndex == nodesNames.size() - 1)
					return asset;
				
				if (asset->IsFolder())
				{
					currentAsset = TYPE_PAWN(asset, Ref<FolderAsset>);
				}
				else
				{
					OE_CORE_ASSERT(false, "Invalid asset path '{}' (at least one node '{}' is not a folder)", path, currentAsset->GetName());
					return nullptr;
				}
			}
			else
			{
				OE_CORE_ASSERT(false, "Invalid asset path '{}' (node '{}' not founded)", path, nodeName);
				return nullptr;
			}

			currentPathNodeIndex++;
		}

		return currentAsset;
	}

	Ref<Asset> AssetCollection::GetAsset(const uint64_t& guid)
	{
		for (const auto& asset : m_AllAssets)
		{
			if (asset->GetGuid() == guid)
				return asset;
		}

		#if 0
		OE_CORE_ASSERT(false, "Cant find asset with {0:#x} GUID!", guid);
		#endif

		return nullptr;
	}

	bool AssetCollection::AssetExists(const String& path)
	{
		if (path[0] != '/')
		{
			OE_CORE_ASSERT(false, "Invalid asset path '{}' (must be started with '/')", path);
			return false;
		}

		if (path.size() == 1) // path == "/"
			return true;

		Vector<String> nodesNames;
		SplitString(FileSystem::FixPath(path), '/', nodesNames);

		uint32_t currentPathNodeIndex = 0;
		Ref<FolderAsset> currentAsset = m_RootAsset;

		for (const String& nodeName : nodesNames)
		{
			if (currentAsset->GetAssets().count(nodeName))
			{
				auto& asset = currentAsset->GetAssets()[nodeName];

				if (currentPathNodeIndex == nodesNames.size() - 1)
					return true;
				
				if (asset->IsFolder())
					currentAsset = TYPE_PAWN(asset, Ref<FolderAsset>);
				else
					return false;

				break;
			}
			else
			{
				return false;
			}

			currentPathNodeIndex++;
		}

		return false;
	}
}
