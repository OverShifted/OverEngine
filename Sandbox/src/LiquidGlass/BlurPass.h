#pragma once

#include <OverEngine.h>

using namespace OverEngine;

class BlurPass {
	struct Vertex {
		Vector3 a_Position;
		Vector2 a_TexCoord;
	};
	
	public:
	BlurPass() {
		m_Shader = Shader::Create("assets/shaders/Blur.glsl");
		m_Shader->Bind();
		m_Shader->UploadUniformInt("u_in", 0);

		m_VB = VertexBuffer::Create();
		m_VB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
		});
		Vertex vertices[4] = {
			Vertex { { -1., -1., 0.0 }, { 0.0, 0.0 } },
			Vertex { { 1.0, -1., 0.0 }, { 1.0, 0.0 } },
			Vertex { { 1.0, 1.0, 0.0 }, { 1.0, 1.0 } },
			Vertex { { -1., 1.0, 0.0 }, { 0.0, 1.0 } }
		};
		m_VB->Allocate(4 * sizeof(Vertex), vertices);


		uint32_t indices[6] = {0, 1, 2, 2, 3, 0 };
		m_IB = IndexBuffer::Create(indices, 6);

		m_VA = VertexArray::Create();
		m_VA->AddVertexBuffer(m_VB);
		m_VA->SetIndexBuffer(m_IB);
	}

	void Run(const Ref<FrameBuffer>& in, const Ref<FrameBuffer>& intermediate, const Ref<FrameBuffer>& out, uint32_t iterations = 7) {
		m_Shader->Bind();
		m_VA->Bind();

		if (iterations == 0) {
			Step(in, out, { 0.0, 0.0 });
			return;
		}

		for (uint32_t i = 0; i < iterations; i++) {
			const Ref<FrameBuffer>& inputFb = i ? out : in;
			const auto& props = inputFb->GetProps();
			m_Shader->UploadUniformFloat2("u_resolution", { props.Width, props.Height });
			Step(inputFb, intermediate, { 1.0, 0.0 });
			Step(intermediate, out, { 0.0, 1.0 });
		}
	}

	void OnResize(uint32_t width, uint32_t height) {
		m_Shader->Bind();
		m_Shader->UploadUniformFloat2("u_resolution", { width, height });
	}

	Ref<Shader>& GetShader() {
		return m_Shader;
	}

private:
	void Step(const Ref<FrameBuffer>& in, const Ref<FrameBuffer>& out, Vector2 direction) {
		out->Bind();
		RenderCommand::SetClearColor(Color{ 0.1, 0.1, 0.1, 1.0 });
		RenderCommand::Clear(ClearFlags_ClearColor | ClearFlags_ClearDepth);
		in->BindColorAttachment(0);
		m_Shader->UploadUniformFloat2("u_direction", direction);
		RenderCommand::DrawIndexed(m_VA);
	}

	Ref<Shader> m_Shader;
	Ref<VertexArray> m_VA;
	Ref<VertexBuffer> m_VB;
	Ref<IndexBuffer> m_IB;
};
