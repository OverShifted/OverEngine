#include "pcheader.h"
#include "VirtualFileSystem.h"

#include "FileSystem.h"

namespace OverEngine
{
	UnorderedMap<String, Vector<String>> VirtualFileSystem::m_MountPoints;

	void VirtualFileSystem::Mount(const String& virtualPath, const String& physicalPath)
	{
		m_MountPoints[virtualPath].push_back(physicalPath);
	}

	void VirtualFileSystem::Unmount(const String& virtualPath)
	{
		m_MountPoints[virtualPath].clear();
	}

	bool VirtualFileSystem::ResolvePhysicalPath(const String& path, String& outPhysicalPath)
	{
		if (path[0] != '/')
		{
			outPhysicalPath = path;
			return FileSystem::FileExists(path);
		}

		std::vector<String> dirs = SplitString(path, '/');
		const String& virtualDir = dirs.front();

		if (m_MountPoints.find(virtualDir) == m_MountPoints.end() || m_MountPoints[virtualDir].empty())
			return false;

		String remainder = path.substr(virtualDir.size() + 1, path.size() - virtualDir.size());
		for (const String& physicalPath : m_MountPoints[virtualDir])
		{
			String path = physicalPath + remainder;
			if (FileSystem::FileExists(path))
			{
				outPhysicalPath = path;
				return true;
			}
		}
		return false;
	}
}