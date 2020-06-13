#pragma once

#include "RenderCommand.h"
#include "Shader.h"
#include "Camera.h"

namespace OverEngine
{
	namespace Renderer
	{
		class Renderer
		{
		public:
			static void BeginScene(const Ref<Camera>& camera);
			static void EndScene();

			static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray);
		private:
			struct SceneData
			{
				glm::mat4 ViewProjectionMatrix;
			};

			static SceneData* s_SceneData;
		};
	}
}