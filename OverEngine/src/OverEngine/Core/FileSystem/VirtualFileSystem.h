#pragma once

namespace OverEngine
{
	class VirtualFileSystem
	{
	public:
		static void Mount(const String& virtualPath, const String& physicalPath);
		static void Unmount(const String& virtualPath);
		bool ResolvePhysicalPath(const String& path, String& outPhysicalPath);
	private:
		static UnorderedMap<String, Vector<String>> m_MountPoints;
	};
}
