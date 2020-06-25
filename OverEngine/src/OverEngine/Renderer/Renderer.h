#pragma once

#include "RenderCommand.h"

#include "Shader.h"
#include "Camera.h"

namespace OverEngine
{
	class Renderer
	{
	public:
		static void Init();

		static void BeginScene(const Camera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const Math::Mat4x4& transform = Math::Mat4x4(1.0f));
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* s_SceneData;
	};
}