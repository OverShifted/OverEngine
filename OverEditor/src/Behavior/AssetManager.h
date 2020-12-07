#pragma once

#include <OverEngine.h>

namespace OverEditor
{
	using namespace OverEngine;

	class AssetManager
	{
	public:
		static Ref<Asset> ImportAndLoad(const String& physicalPath, const String& assetsDirectoryRoot, AssetCollection* collection);

		static AssetType FindAssetTypeFromExtension(const String& extension);
	};
}
