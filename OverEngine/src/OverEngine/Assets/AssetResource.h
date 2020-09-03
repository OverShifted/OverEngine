#pragma once

#include "Asset.h"
#include <OverEngine/Core/GUIDGenerator.h>

#include <json.hpp>

namespace OverEngine
{
	enum class ResourceType
	{
		None = 0, Directory, Texture2D
	};

	class Resource
	{
	public:
		Resource(const String& name, const String& virtualPath, ResourceType type);

		// Loads resource and assets based on a .oea file
		// To be used in OverEditor
		Resource(const String& path, const String& assetsDirectoryRoot, bool isPhysicalPath);

		~Resource() {}

		void AddAsset(const Ref<Asset>& asset);
		void RemoveAsset(const Ref<Asset>& asset);

		inline const String& GetName() const { return m_Name; }
		inline const String& GetVirtualPath() const { return m_VirtualPath; }

		inline bool IsDirectory() const { return m_Type == ResourceType::Directory; }

		inline Vector<Ref<Resource>>& GetChildren()
		{
			OE_CORE_ASSERT(IsDirectory(), "non-directory Resources dont have children!");
			return m_Children;
		}

		inline const Vector<Ref<Resource>>& GetChildren() const
		{
			OE_CORE_ASSERT(IsDirectory(), "non-directory Resources dont have children!");
			return m_Children;
		}

		inline const Guid& GetGuid() const { return m_Guid; }
		inline void SetGuid(const Guid& guid) { m_Guid = guid; }
	private:
		ResourceType m_Type = ResourceType::None;
		String m_Name;
		String m_VirtualPath;
		Guid m_Guid;

		// Only for non-directory Resources
		Vector<Ref<Asset>> m_Assets;
		// Only for directory Resources
		Vector<Ref<Resource>> m_Children;
	};

	class ResourceCollection
	{
	public:
		ResourceCollection();

		void InitFromAssetsDirectory(const String& assetsDirectoryPath, const Guid& assetsDirectoryGuid);

		void AddResource(const Ref<Resource> resource);
		void AddResource(const Ref<Resource> resource, const String& path);

		// Returns the matching Resource with the path
		// use 'assets://' to get the root Resource
		Ref<Resource> GetResource(const String& path);

		bool NodeExists(const String& path);
	private:
		Ref<Resource> m_RootResource;
	};
}
