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
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const Camera& camera);
		static void EndScene();
		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const Mat4x4& transform = Mat4x4(1.0f));

		inline static ShaderLibrary& GetShaderLibrary() { return s_ShaderLibrary; }
	private:
		struct SceneData
		{
			Mat4x4 ViewProjectionMatrix;
		};
		static Scope<SceneData> s_SceneData;

		static ShaderLibrary s_ShaderLibrary;
	};
}