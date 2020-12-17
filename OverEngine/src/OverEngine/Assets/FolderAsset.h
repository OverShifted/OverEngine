#pragma once

#include "Asset.h"
#include <yaml-cpp/yaml.h>

namespace OverEngine
{
	class FolderAsset : public Asset
	{
	public:
		FolderAsset() = default;
		FolderAsset(YAML::Node node, const String& assetsDirectoryRoot);

		FolderAsset(const String& path, const uint64_t& guid);
		FolderAsset(const String& name, const String& path, const uint64_t& guid);

		auto& GetAssets() { return m_Assets; }
		const auto& GetAssets() const { return m_Assets; }
	private:
		// Wanna be ordered
		Map<String, Ref<Asset>> m_Assets;
	};
}
