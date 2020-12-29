#include "AssetManager.h"

#include <OverEngine/Core/FileSystem/FileSystem.h>
#include <OverEngine/Core/Extentions.h>
#include <OverEngine/Core/Random.h>

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace OverEditor
{
	Ref<Asset> AssetManager::ImportAndLoad(const String& physicalPath, const String& assetsDirectoryRoot, AssetCollection* collection)
	{
		auto assetPath = FileSystem::FixPath(physicalPath.substr(assetsDirectoryRoot.size(), physicalPath.size()));

		#if 0
		if (collection->AssetExists(assetPath))
			return nullptr;
		#endif

		if (std::filesystem::is_directory(physicalPath))
		{
			YAML::Emitter emitter;
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Name" << YAML::Value << FileSystem::ExtractFileNameFromPath(physicalPath);
			emitter << YAML::Key << "Path" << YAML::Value << assetPath;
			emitter << YAML::Key << "Guid" << YAML::Value << YAML::Hex << Random::UInt64();
			emitter << YAML::Key << "Type" << YAML::Value << "Folder";
			emitter << YAML::EndMap;

			String metaFilePath = physicalPath + "." + OE_META_ASSET_FILE_EXTENSION;
			FileSystem::FixPath(metaFilePath);
			std::ofstream metaFile(metaFilePath);
			metaFile << emitter.c_str();
			metaFile.flush();
			metaFile.close();

			return Asset::Load(metaFilePath, true, assetsDirectoryRoot, collection);
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
			emitter << YAML::Key << "Guid" << YAML::Value << YAML::Hex << Random::UInt64();
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
			return Asset::Load(metaFilePath, true, assetsDirectoryRoot, collection);
		}
		default: break;
		}

		OE_CORE_INFO(extension);

		return nullptr;
	}

	AssetType AssetManager::FindAssetTypeFromExtension(const String& extension)
	{
		if (extension == "png" || extension == "jpg")
			return AssetType::Texture2D;

		return AssetType::None;
	}
}
