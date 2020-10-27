#include "pcheader.h"
#include "SceneAsset.h"

namespace OverEngine
{
	SceneAsset::SceneAsset(YAML::Node node, const String& assetsDirectoryRoot)
	{
		m_Type = AssetType::Scene;
		m_Name = node["Name"].as<String>();
		m_Path = node["Path"].as<String>();
		m_Guid = node["Guid"].as<Guid>();

		m_Scene = Scene::LoadFile(assetsDirectoryRoot + m_Path);
	}
}
