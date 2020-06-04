#pragma once

#include "RenderCommand.h"

namespace OverEngine
{
	namespace Renderer
	{
		class Renderer
		{
		public:
			static void BeginScene();
			static void EndScene();

			static void Submit(const Ref<VertexArray>& vertexArray);
		};
	}
}