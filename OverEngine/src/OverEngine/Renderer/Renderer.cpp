#include "pcheader.h"
#include "Renderer.h"

namespace OverEngine
{
	namespace Renderer
	{
		Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

		void Renderer::BeginScene(const Ref<Camera>& camera)
		{
			s_SceneData->ViewProjectionMatrix = camera->GetViewProjectionMatrix();
		}

		void Renderer::EndScene()
		{
		}

		void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray)
		{
			shader->Bind();
			shader->UploadUniform("u_ViewProjMatrix", s_SceneData->ViewProjectionMatrix);
			vertexArray->Bind();
			RenderCommand::DrawIndexed(vertexArray);
		}
	}
}