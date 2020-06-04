#include "pcheader.h"
#include "Renderer.h"

namespace OverEngine
{
	namespace Renderer
	{
		void Renderer::BeginScene()
		{
		}

		void Renderer::EndScene()
		{
		}

		void Renderer::Submit(const Ref<VertexArray>& vertexArray)
		{
			vertexArray->Bind();
			RenderCommand::DrawIndexed(vertexArray);
		}
	}
}