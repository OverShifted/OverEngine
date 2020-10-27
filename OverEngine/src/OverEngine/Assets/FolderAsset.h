#pragma once

#include "Asset.h"

namespace OverEngine
{
	class FolderAsset : public Asset
	{
	public:
		FolderAsset() = default;
		FolderAsset(const String& path, const Guid& guid);
		FolderAsset(const String& name, const String& path, const Guid& guid);

		auto& GetAssets() { return m_Assets; }
		const auto& GetAssets() const { return m_Assets; }
	private:
		Vector<Ref<Asset>> m_Assets;
	};
}
