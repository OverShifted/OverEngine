#pragma once

namespace OverEditor
{
	using namespace OverEngine;

	struct SceneEditor
	{
		Ref<Scene> Context;
		Scene* SimulationScene;
		String ContextResourcePath;
		bool Simulating = false;
		bool SimulationRunning = true;
		Vector<Entity> SelectionContext;

		Scene* GetActiveScene()
		{
			if (Simulating)
				return SimulationScene;
			return Context.get();
		}
	};
}
