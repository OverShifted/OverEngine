#include "SandboxLayer.h"

#include "imgui/imgui.h"

SandboxLayer::SandboxLayer()
	: Layer("SandboxLayer")
{
	OverEngine::Application& app = OverEngine::Application::Get();

	m_Camera.reset(new OverEngine::OrthographicCamera(1.0f, (float)app.GetMainWindow().GetWidth() / (float)app.GetMainWindow().GetHeight()));

	// m_Camera.reset(new OverEngine::PerspectiveCamera(60.0f, (float)app.GetMainWindow().GetWidth() / (float)app.GetMainWindow().GetHeight()));
	// m_Camera->SetPosition({ 0.0f, 0.0f, 10.0f });

	m_SquareTransform = OverEngine::CreateRef<OverEngine::Transform>();
	m_SquareTransform->SetPosition({ 2.0f, 3.0f, 0.0f });
	m_SquareTransform->SetScale({ 5.0f, 5.0f, 1.0f });

	m_TriangleTransform = OverEngine::CreateRef<OverEngine::Transform>();

	m_VertexArray.reset(OverEngine::VertexArray::Create());

	float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
	};

	OverEngine::Ref<OverEngine::VertexBuffer> vertexBuffer;
	vertexBuffer.reset(OverEngine::VertexBuffer::Create(vertices, sizeof(vertices)));

	OverEngine::BufferLayout layout = {
		{ OverEngine::ShaderDataType::Float3, "a_Position" },
		{ OverEngine::ShaderDataType::Float4, "a_Color" }
	};

	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	uint32_t indices[3] = { 0, 1, 2 };
	OverEngine::Ref<OverEngine::IndexBuffer> indexBuffer;
	indexBuffer.reset(OverEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	m_SquareVA.reset(OverEngine::VertexArray::Create());

	float squareVertices[3 * 4] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	OverEngine::Ref<OverEngine::VertexBuffer> squareVB;
	squareVB.reset(OverEngine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

	squareVB->SetLayout({
		{ OverEngine::ShaderDataType::Float3, "a_Position" }
	});

	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	OverEngine::Ref<OverEngine::IndexBuffer> squareIB;
	squareIB.reset(OverEngine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	m_SquareVA->SetIndexBuffer(squareIB);

	OverEngine::String vertexSrc = R"(
		#version 330 core
		
		layout(location = 0) in vec3 a_Position;
		layout(location = 1) in vec4 a_Color;

		// out vec3 v_Position;
		out vec4 v_Color;

		uniform mat4 u_ViewProjMatrix;
		uniform mat4 u_TransformationMatrix;

		void main()
		{
			// v_Position = a_Position;
			v_Color = a_Color;
			gl_Position = u_ViewProjMatrix * u_TransformationMatrix * vec4(a_Position, 1.0);	
		}
	)";

	OverEngine::String fragmentSrc = R"(
		#version 330 core
		
		layout(location = 0) out vec4 color;

		in vec3 v_Position;
		in vec4 v_Color;

		void main()
		{
			// color = vec4(v_Position * 0.5 + 0.5, 1.0);
			color = v_Color;
		}
	)";

	m_Shader.reset(OverEngine::Shader::Create(vertexSrc, fragmentSrc));

	OverEngine::String blueShaderVertexSrc = R"(
		#version 330 core
		
		layout(location = 0) in vec3 a_Position;

		out vec3 v_Position;

		uniform mat4 u_ViewProjMatrix;
		uniform mat4 u_TransformationMatrix;

		void main()
		{
			v_Position = a_Position;
			gl_Position = u_ViewProjMatrix * u_TransformationMatrix * vec4(a_Position, 1.0);	
		}
	)";

	OverEngine::String flatColorFragmentShaderSrc = R"(
		#version 330 core
		
		layout(location = 0) out vec4 color;

		in vec3 v_Position;

		uniform vec4 u_Color;

		void main()
		{
			color = u_Color;
		}
	)";

	m_FlatColorFragmentShader.reset(OverEngine::Shader::Create(blueShaderVertexSrc, flatColorFragmentShaderSrc));
}

void SandboxLayer::OnAttach()
{
	std::stringstream ss = std::stringstream();
	ss << "Vendor: " << OverEngine::Application::Get().GetMainWindow().GetRendererContext()->GetInfoVendor();
	vendorInfo = ss.str();

	ss = std::stringstream();
	ss << "Renderer: " << OverEngine::Application::Get().GetMainWindow().GetRendererContext()->GetInfoRenderer();
	rendererInfo = ss.str();

	ss = std::stringstream();
	ss << "Version: " << OverEngine::Application::Get().GetMainWindow().GetRendererContext()->GetInfoVersion();
	versionInfo = ss.str();
}

void SandboxLayer::OnUpdate(OverEngine::TimeStep DeltaTime)
{
	OverEngine::Math::Vector3 offset(0.0f);
	float rotationOffset = 0.0f;

	if (OverEngine::Input::IsKeyPressed(OverEngine::KeyCode::W))
		offset.y += m_CameraSpeed * DeltaTime;
	if (OverEngine::Input::IsKeyPressed(OverEngine::KeyCode::S))
		offset.y -= m_CameraSpeed * DeltaTime;
	if (OverEngine::Input::IsKeyPressed(OverEngine::KeyCode::D))
		offset.x += m_CameraSpeed * DeltaTime;
	if (OverEngine::Input::IsKeyPressed(OverEngine::KeyCode::A))
		offset.x -= m_CameraSpeed * DeltaTime;

	if (OverEngine::Input::IsKeyPressed(OverEngine::KeyCode::Q))
		rotationOffset -= 100 * DeltaTime;
	if (OverEngine::Input::IsKeyPressed(OverEngine::KeyCode::E))
		rotationOffset += 100 * DeltaTime;

	if (OverEngine::Input::IsKeyPressed(OverEngine::KeyCode::Escape))
	{
		m_Camera->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_Camera->SetRotation({ 0.0f, 0.0f, 0.0f });
		m_Camera->SetOrthographicSize(1.0f);
	}

	m_Camera->SetPosition(m_Camera->GetPosition() + offset);
	m_Camera->SetRotation({ 0.0f, 0.0f, m_Camera->GetRotation().z + rotationOffset });

	OverEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	OverEngine::RenderCommand::Clear();

	OverEngine::Renderer::BeginScene(*m_Camera);

	for (int x = 0; x <= 20; x++)
	{
		for (int y = 0; y <= 20; y++)
		{
			OverEngine::Transform t;
			t.SetPosition({ x * 0.11f + 0.3f, y * 0.11f, 0.0f });
			t.SetRotation(OverEngine::Math::QuaternionEuler({ 0.0f, 0.0f, 0.0f }));
			t.SetScale(OverEngine::Math::Vector3(0.1f));

			if (x % 2 == 0)
				m_FlatColorFragmentShader->UploadUniformFloat4("u_Color", OverEngine::Math::Color{ 1.0f, 0.0f, 0.0f, 1.0f });
			else
				m_FlatColorFragmentShader->UploadUniformFloat4("u_Color", OverEngine::Math::Color{ 0.0f, 1.0f, 0.0f, 1.0f });
			OverEngine::Renderer::Submit(m_FlatColorFragmentShader, m_SquareVA, t);
		}
	}
	OverEngine::Renderer::Submit(m_Shader, m_VertexArray, *m_TriangleTransform);

	OverEngine::Renderer::EndScene();
}

void SandboxLayer::OnImGuiRender() 
{
	static bool show = false;
	if (show)
		ImGui::ShowDemoWindow(&show);

	ImGui::Begin("Renderer");

	ImGui::Text(vendorInfo.c_str());
	ImGui::Text(rendererInfo.c_str());
	ImGui::Text(versionInfo.c_str());

	ImGui::End();
}

void SandboxLayer::OnEvent(OverEngine::Event& event) 
{
	OverEngine::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<OverEngine::WindowResizeEvent>(OE_BIND_EVENT_FN(SandboxLayer::OnWindowResizeEvent));
	dispatcher.Dispatch<OverEngine::MouseScrolledEvent>(OE_BIND_EVENT_FN(SandboxLayer::OnMouseScrolledEvent));
}

bool SandboxLayer::OnWindowResizeEvent(OverEngine::WindowResizeEvent& event)
{
	m_Camera->SetAspectRatio((float)event.GetWidth() / (float)event.GetHeight());
	return false;
}

bool SandboxLayer::OnMouseScrolledEvent(OverEngine::MouseScrolledEvent& event)
{
	float newSize = m_Camera->GetOrthographicSize() - (float)event.GetYOffset() / 4.0f;
	if (newSize > 0)
		m_Camera->SetOrthographicSize(newSize);
	return false;
}