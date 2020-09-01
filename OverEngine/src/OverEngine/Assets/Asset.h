#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Renderer/Texture.h"

namespace OverEngine
{
	//////////////////////////////////////////////////////////////////
	// Asset /////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	class Resource;

	enum class AssetType
	{
		None = 0, Texture2D
	};

	class Asset
	{
	public:
		virtual AssetType GetType() const = 0;
		virtual const String& GetName() const = 0;
		virtual Resource* GetResource() const = 0;
	};

	class Texture2DAsset : public Asset
	{
	public:
		Texture2DAsset(const String& name, Resource* resource, Ref<Texture2D> asset)
			: m_Name(name), m_Resource(resource), m_Asset(asset) {}

		virtual AssetType GetType() const override { return AssetType::Texture2D; }
		virtual const String& GetName() const override { return m_Name; }
		virtual Resource* GetResource() const override { return m_Resource; }

		Ref<Texture2D>& GetAsset() { return m_Asset; }
		const Ref<Texture2D>& GetAsset() const { return m_Asset; }
	private:
		String m_Name;
		Resource* m_Resource;
		Ref<Texture2D> m_Asset;
	};
}
