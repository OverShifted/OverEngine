#include "pcheader.h"
#include "SceneAsset.h"
#include "OverEngine/Scene/SceneSerializer.h"

namespace OverEngine
{
	SceneAsset::SceneAsset(YAML::Node node, const String& assetsDirectoryRoot)
	{
		m_Type = AssetType::Scene;
		m_Name = node["Name"].as<String>();
		m_Path = node["Path"].as<String>();
		m_Guid = node["Guid"].as<uint64_t>();

		m_Scene = CreateRef<Scene>();
		SceneSerializer(m_Scene).Deserialize(assetsDirectoryRoot + m_Path);
	}
}
