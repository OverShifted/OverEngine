#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Assets/Asset.h"

namespace OverEngine
{
	class AssetManager
	{
	public:
		static Ref<Asset> Load(const String& filePath, Asset::Type type);
		static Ref<Asset> Load(const String& filePath, const String& Name, Asset::Type type);
	private:
		static Vector<Ref<Asset>> m_Assets;
	};
}