#pragma once

#include "Asset.h"
#include <yaml-cpp/yaml.h>

namespace OverEngine
{
	class Asset;

	class FolderAsset : public Asset
	{
		OE_ASSET_CLASS(Folder)

	public:
		FolderAsset() = default;
		FolderAsset(YAML::Node node, const String& assetsDirectoryRoot);
		FolderAsset(const String& name, const String& path, const uint64_t& guid);

		auto& GetAssets() { return m_Assets; }
		const auto& GetAssets() const { return m_Assets; }

		Ref<Asset> GetChild(const String& name);
	private:
		Map<String, Ref<Asset>> m_Assets;
	};
}
