#include "pcheader.h"
#include "AssetDatabase.h"

#include <OverEngine/Core/Extensions.h>
#include <OverEngine/Scene/SceneSerializer.h>

#include <OverEngine/Renderer/Texture.h>

#include <yaml-cpp/yaml.h>
#include <filesystem>

namespace OverEngine
{
	UnorderedMap<uint64_t, Ref<Asset>> AssetDatabase::s_Registry;
	Ref<AssetFolder> AssetDatabase::s_Root = nullptr;

	void AssetDatabase::Init()
	{
		s_Root = CreateRef<AssetFolder>();
	}

	bool AssetDatabase::Init(const String& assetDirectoryPath)
	{
		s_Root = CreateRef<AssetFolder>();
		String correctedAssetDirectoryPath = std::filesystem::path(assetDirectoryPath).string();

		for (auto entry : std::filesystem::recursive_directory_iterator(correctedAssetDirectoryPath))
		{
			String stringPath = entry.path().string();
			String extensionWithDot = entry.path().extension().string();

			if (extensionWithDot.empty())
				continue;

			std::string_view extension = std::string_view(extensionWithDot.c_str() + 1, extensionWithDot.size() - 1);

			if (extension == Extensions::AssetMetadataFileExtension)
			{
				try
				{
					YAML::Node metaData = YAML::LoadFile(stringPath);
					String typeStr = metaData["Type"].as<String>();
					uint64_t guid = metaData["Guid"].as<uint64_t>();

					Ref<Asset> asset = nullptr;

					if (typeStr == Scene::GetStaticClassName())
					{
						Ref<Scene> scene = CreateRef<Scene>();

						try
						{
							SceneSerializer(scene).Deserialize(stringPath.substr(0, stringPath.size() - 1 - strlen(Extensions::AssetMetadataFileExtension)));
						}
						catch (const std::exception& e)
						{
							OE_CORE_ERROR(fmt::format("Scene asset could not be de-serialized successfuly. Error message: '{}'", e.what()));
						}

						asset = scene;
					}
					else if (typeStr == Texture2D::GetStaticClassName())
					{
						Ref<Texture2D> texture = nullptr;

						try
						{
							texture = Texture2D::Create(stringPath.substr(0, stringPath.size() - 1 - strlen(Extensions::AssetMetadataFileExtension)));
						}
						catch (const std::exception& e)
						{
							OE_CORE_ERROR(fmt::format("Texture asset could not be loaded successfuly. Error message: '{}'", e.what()));
						}

						asset = texture;
					}

					if (asset)
					{
						asset->m_Guid = guid;
						String pathToPut = stringPath.substr(correctedAssetDirectoryPath.size(), stringPath.size() - correctedAssetDirectoryPath.size() - 1 - strlen(Extensions::AssetMetadataFileExtension));
						Put(AssetPath(pathToPut), asset);
					}
					else
					{
						OE_THROW("Unknown asset type");
					}
				}
				catch (const std::exception& e)
				{
					OE_CORE_ERROR(fmt::format("Bad metadata {}", stringPath));
				}
			}
		}

		return true;
	}

	void AssetDatabase::Clear()
	{
		s_Registry.clear();
		s_Root = nullptr;
	}

	void AssetDatabase::Refresh()
	{
	}

	bool AssetDatabase::Put(const AssetPath& path, const Ref<Asset>& asset)
	{
		if (!path.Absolute)
			return false;

		Ref<AssetFolder> currentNode = s_Root;
		AssetPath currentPath;
		currentPath.Absolute = true;

		std::size_t i = path.PathNodes.size();
		for (const String& pathNode : path.PathNodes)
		{
			currentPath.PathNodes.push_back(pathNode);

			if (i == 1)
			{
				AssetFolder::AssetMappingKey k;
				k.Name = pathNode;

				if (asset->GetClassName() == AssetFolder::GetStaticClassName())
					k.Type = AssetFolder::AssetMappingKey::AssetMappingKeyType_Folder;

				k.Type = AssetFolder::AssetMappingKey::AssetMappingKeyType_Folder;
				if (STD_MAP_HAS(currentNode->GetAssets(), k))
					return false;

				k.Type = AssetFolder::AssetMappingKey::AssetMappingKeyType_NonFolder;
				if (STD_MAP_HAS(currentNode->GetAssets(), k))
					return false;

				if (asset->GetClassName() == AssetFolder::GetStaticClassName())
					k.Type = AssetFolder::AssetMappingKey::AssetMappingKeyType_Folder;

				currentNode->GetAssets()[k] = asset;
			}
			else if (auto child = (*currentNode)[pathNode])
			{
				currentNode = std::dynamic_pointer_cast<AssetFolder>(child);

				if (!currentNode)
					return false;
			}
			else
			{
				auto newFolder = CreateRef<AssetFolder>();
				newFolder->m_Path = currentPath;

				AssetFolder::AssetMappingKey k;
				k.Name = pathNode;
				k.Type = AssetFolder::AssetMappingKey::AssetMappingKeyType_Folder;

				currentNode->GetAssets()[k] = newFolder;
				currentNode = newFolder;
			}

			i--;
		}

		s_Registry[asset->GetGuid()] = asset;
		asset->m_Path = path;

		return true;
	}

	Ref<Asset> AssetDatabase::GetAssetByPath(const AssetPath& path)
	{
		if (!path.Absolute)
			return nullptr;

		Ref<AssetFolder> currentNode = s_Root;

		std::size_t i = path.PathNodes.size();
		for (const String& pathNode : path.PathNodes)
		{
			if (i == 1)
			{
				AssetFolder::AssetMappingKey k;
				k.Name = pathNode;
				k.Type = AssetFolder::AssetMappingKey::AssetMappingKeyType_NonFolder;

				auto it = currentNode->GetAssets().find(k);

				if (it == currentNode->GetAssets().end())
				{
					k.Type = AssetFolder::AssetMappingKey::AssetMappingKeyType_Folder;
					it = currentNode->GetAssets().find(k);
				}

				if (it == currentNode->GetAssets().end())
					return nullptr;

				return it->second;
			}
			else
			{
				AssetFolder::AssetMappingKey k;
				k.Name = pathNode;
				k.Type = AssetFolder::AssetMappingKey::AssetMappingKeyType_Folder;

				if (auto it = currentNode->GetAssets().find(k); it != currentNode->GetAssets().end())
					currentNode = std::dynamic_pointer_cast<AssetFolder>(it->second);
				else
					return nullptr;
			}

			i--;
		}

		return currentNode;
	}
}
