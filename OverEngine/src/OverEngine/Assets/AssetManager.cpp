#include "pcheader.h"
#include "AssetManager.h"

namespace OverEngine
{
	Vector<Ref<Asset>> AssetManager::m_Assets;

	Ref<Asset> AssetManager::Load(const String& filePath, Asset::Type type)
	{
		switch (type)
		{
		case Asset::Type::Texture: 
			break;
		}
		return nullptr;
	}

	Ref<Asset> AssetManager::Load(const String& filePath, const String& Name, Asset::Type type)
	{
		return nullptr;
	}
}