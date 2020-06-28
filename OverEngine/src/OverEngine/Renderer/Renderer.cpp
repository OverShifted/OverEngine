#include "pcheader.h"
#include "Renderer.h"

namespace OverEngine
{
	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;
	ShaderLibrary Renderer::s_ShaderLibrary;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const Camera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const Math::Mat4x4& transform /*= Math::Mat4x4(1.0f)*/)
	{
		shader->Bind();
		shader->UploadUniformMat4("u_ViewProjMatrix", s_SceneData->ViewProjectionMatrix);
		shader->UploadUniformMat4("u_Transform", transform);
		vertexArray->Bind();
		RenderCommand::DrawIndexed(*vertexArray);
	}
}