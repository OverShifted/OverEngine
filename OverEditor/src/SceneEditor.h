#pragma once

namespace OverEditor
{
	using namespace OverEngine;

	struct SceneEditor
	{
		Ref<Scene> Context;
		String ContextResourcePath;
		Vector<Entity> SelectionContext;
	};
}
