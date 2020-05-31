#pragma once

#include "RenderCommand.h"

namespace OverEngine
{
	class Renderer
	{
	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(const Ref<VertexArray>& vertexArray);
	};
}