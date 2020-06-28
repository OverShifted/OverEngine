#include "SandboxLayer.h"

#include "imgui/imgui.h"

SandboxLayer::SandboxLayer()
	: Layer("SandboxLayer"), m_CameraMovementDirection(0.0f)
{
	// Camera
	OverEngine::Application& app = OverEngine::Application::Get();

	float aspectRatio = (float)app.GetMainWindow().GetWidth() / (float)app.GetMainWindow().GetHeight();
	m_Camera = OverEngine::OrthographicCamera(1.0f, aspectRatio);

	// Vertex Arrays, Vertex Buffers, Index Buffers
	m_VertexArray = OverEngine::VertexArray::Create();

	float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
	};

	auto vertexBuffer = OverEngine::VertexBuffer::Create(vertices, sizeof(vertices));

	OverEngine::BufferLayout layout = {
		{ OverEngine::ShaderDataType::Float3, "a_Position" },
		{ OverEngine::ShaderDataType::Float4, "a_Color" }
	};

	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	uint32_t indices[3] = { 0, 1, 2 };
	auto indexBuffer = OverEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	m_SquareVA = OverEngine::VertexArray::Create();

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	auto squareVB = OverEngine::VertexBuffer::Create(squareVertices, sizeof(squareVertices));

	squareVB->SetLayout({
		{ OverEngine::ShaderDataType::Float3, "a_Position" },
		{ OverEngine::ShaderDataType::Float2, "a_TexCoord" }
	});

	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	auto squareIB = OverEngine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_SquareVA->SetIndexBuffer(squareIB);

	// Shaders
	m_Shader = OverEngine::Shader::Create("assets/shaders/VertexColor.glsl");

	auto textureShader = OverEngine::Renderer::GetShaderLibrary().Load("assets/shaders/Texture.glsl");
	textureShader->UploadUniformInt("u_Textuer", 0);

	OverEngine::Renderer::GetShaderLibrary().Load("assets/shaders/FlatColor.glsl");

	m_OELogoTexture = OverEngine::Texture2D::Create("assets/textures/OELogo.png");
	m_CheckerBoardTexture = OverEngine::Texture2D::Create("assets/textures/Checkerboard.png", OverEngine::Texture::Filtering::Linear, OverEngine::Texture::Filtering::Nearest);

	// Input
	auto actionMap = OverEngine::InputActionMap::Create();

	OverEngine::InputAction CameraMovement(OverEngine::InputActionType::Button, {
		{
			{OverEngine::KeyCode::A}, {OverEngine::KeyCode::D},
			{OverEngine::KeyCode::S}, {OverEngine::KeyCode::W}
		},
		{
			{OverEngine::KeyCode::Left}, {OverEngine::KeyCode::Right},
			{OverEngine::KeyCode::Down}, {OverEngine::KeyCode::Up}
		}
	});
	CameraMovement.AddCallBack([&](OverEngine::InputAction::TriggerInfo& info) {
		m_CameraMovementDirection = info.ReadValue<OverEngine::Math::Vector2>();
	});
	actionMap->AddAction(CameraMovement);

	OverEngine::InputAction CameraRotation(OverEngine::InputActionType::Button, {
		{ {OverEngine::KeyCode::Q}, {OverEngine::KeyCode::E} }
	});
	CameraRotation.AddCallBack([&](OverEngine::InputAction::TriggerInfo& info) {
		m_CameraRotationDirection = info.x;
	});
	actionMap->AddAction(CameraRotation);

	OverEngine::InputAction EscapeKeyAction(OverEngine::InputActionType::Button, {
		{ {OverEngine::KeyCode::Escape, true, false} }
	});
	EscapeKeyAction.AddCallBack([&](OverEngine::InputAction::TriggerInfo& info) {
		m_Camera.SetPosition({ 0.0f, 0.0f, 0.0f });
		m_Camera.SetRotation({ 0.0f, 0.0f, 0.0f });
		m_Camera.SetOrthographicSize(1.0f);
	});
	actionMap->AddAction(EscapeKeyAction);
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
	// Update
	OverEngine::Math::Vector3 Offset(m_CameraMovementDirection, 0.0f);
	m_Camera.SetPosition(m_Camera.GetPosition() + Offset * (m_CameraSpeed * DeltaTime * m_Camera.GetOrthographicSize()));
	m_Camera.SetRotation({ 0.0f, 0.0f, m_Camera.GetRotation().z + m_CameraRotationDirection * DeltaTime * 80.0f });

	// Render
	OverEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	OverEngine::RenderCommand::Clear();

	OverEngine::Renderer::BeginScene(m_Camera);

	auto flatColorShader = OverEngine::Renderer::GetShaderLibrary().Get("FlatColor");
	flatColorShader->Bind();
	flatColorShader->UploadUniformFloat4("u_Color", OverEngine::Math::Color(0.2, 0.3, 0.8, 1.0));

	for (int x = 0; x <= 20; x++)
	{
		for (int y = 0; y <= 20; y++)
		{
			OverEngine::Transform t;
			t.SetPosition({ x * 0.11f + 0.3f, y * 0.11f, 0.0f });
			t.SetRotation(OverEngine::Math::QuaternionEuler({ 0.0f, 0.0f, 0.0f }));
			t.SetScale(OverEngine::Math::Vector3(0.1f));
			
			OverEngine::Renderer::Submit(flatColorShader, m_SquareVA, t.GetTransformationMatrix());
		}
	}

	auto textureShader = OverEngine::Renderer::GetShaderLibrary().Get("Texture");

	m_CheckerBoardTexture->Bind();
	OverEngine::Renderer::Submit(textureShader, m_SquareVA, m_SquareTransform.GetTransformationMatrix());

	m_OELogoTexture->Bind();
	OverEngine::Renderer::Submit(textureShader, m_SquareVA, m_SquareTransform.GetTransformationMatrix());

	// Triangle
	// OverEngine::Renderer::Submit(m_Shader, m_VertexArray, m_TriangleTransform.GetTransformationMatrix());

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
	m_Camera.SetAspectRatio((float)event.GetWidth() / (float)event.GetHeight());
	return false;
}

bool SandboxLayer::OnMouseScrolledEvent(OverEngine::MouseScrolledEvent& event)
{
	float newSize = m_Camera.GetOrthographicSize() - (float)event.GetYOffset() / 4.0f;
	if (newSize > 0)
		m_Camera.SetOrthographicSize(newSize);
	return false;
}