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
						asset->m_Name = metaData["Name"].as<String>();
						asset->m_Guid = metaData["Guid"].as<uint64_t>();
						CreateAsset(metaData["Path"].as<String>(), asset);
					}
					else
					{
						OE_THROW("Unknown asset type");
					}
				}
				catch (const std::exception&)
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

	bool AssetDatabase::CreateAsset(const String& path, const Ref<Asset>& asset)
	{
		if (path[0] != '/')
			return false;

		Ref<AssetFolder> currentNode = s_Root;
		String currentPath;

		Vector<String> pathNodes;
		StringUtils::Split(path, "/", pathNodes);

		uint32_t i = (uint32_t)pathNodes.size();
		for (const String& pathNode : pathNodes)
		{
			currentPath += "/";
			currentPath += pathNode;

			if (i == 1)
			{
				(*currentNode)[pathNode] = asset;
			}
			else if (STD_MAP_HAS(currentNode->GetAssets(), pathNode))
			{
				currentNode = std::dynamic_pointer_cast<AssetFolder>((*currentNode)[pathNode]);

				if (!currentNode)
					return false;
			}
			else
			{
				auto newFolder = CreateRef<AssetFolder>();
				newFolder->m_Path = currentPath;
				newFolder->m_Name = pathNode;

				(*currentNode)[pathNode] = newFolder;
				currentNode = newFolder;
			}

			i--;
		}

		auto it = s_Registry.find(asset->GetGuid());
		if (it != s_Registry.end())
			it->second->Acquire(asset);
		else
			s_Registry[asset->GetGuid()] = asset;

		asset->m_Path = path;
		return true;
	}

	Ref<Asset> AssetDatabase::GetAssetByPath(const String& path)
	{
		if (path[0] != '/')
			return nullptr;

		Ref<AssetFolder> currentNode = s_Root;

		Vector<String> pathNodes;
		StringUtils::Split(path, "/", pathNodes);

		uint32_t i = (uint32_t)pathNodes.size();
		for (const String& pathNode : pathNodes)
		{
			if (i == 1)
			{
				auto it = currentNode->GetAssets().find(pathNode);

				if (it == currentNode->GetAssets().end())
					return nullptr;

				return it->second;
			}
			else
			{
				if (auto it = currentNode->GetAssets().find(pathNode); it != currentNode->GetAssets().end())
					currentNode = std::dynamic_pointer_cast<AssetFolder>(it->second);
				else
					return nullptr;

				if (!currentNode)
					return nullptr;
			}

			i--;
		}

		return currentNode;
	}
}
