#if 0
#include "SandboxLayer.h"

#include "imgui/imgui.h"

using namespace OverEngine;

SandboxLayer::SandboxLayer()
	: Layer("SandboxLayer"), m_CameraMovementDirection(0.0f)
{
	// Camera
	Application& app = Application::Get();

	float aspectRatio = (float)app.GetMainWindow().GetWidth() / (float)app.GetMainWindow().GetHeight();
	m_Camera = OrthographicCamera(1.0f, aspectRatio);

	// Vertex Arrays, Vertex Buffers, Index Buffers
	m_VertexArray = VertexArray::Create();

	float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
	};

	auto vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));

	BufferLayout layout = {
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float4, "a_Color" }
	};

	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	uint32_t indices[3] = { 0, 1, 2 };
	auto indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	m_SquareVA = VertexArray::Create();

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	auto squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));

	squareVB->SetLayout({
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float2, "a_TexCoord" }
	});

	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	auto squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_SquareVA->SetIndexBuffer(squareIB);

	// Shaders
	m_Shader = Shader::Create("assets/shaders/VertexColor.glsl");

	auto textureShader = Renderer::GetShaderLibrary().Load("assets/shaders/Texture.glsl");
	textureShader->UploadUniformInt("u_Textuer", 0);

	Renderer::GetShaderLibrary().Load("assets/shaders/FlatColor.glsl");

	m_OELogoTexture = Texture2D::Create("assets/textures/OELogo.png");
	m_CheckerBoardTexture = Texture2D::Create("assets/textures/Checkerboard.png", TextureFiltering::Linear, TextureFiltering::Nearest);

	// Input
	auto actionMap = InputActionMap::Create();

	InputAction CameraMovement(InputActionType::Button, {
		{
			{KeyCode::A}, {KeyCode::D},
			{KeyCode::S}, {KeyCode::W}
		},
		{
			{KeyCode::Left}, {KeyCode::Right},
			{KeyCode::Down}, {KeyCode::Up}
		}
	});
	CameraMovement.AddCallBack([&](InputAction::TriggerInfo& info) {
		m_CameraMovementDirection = info.ReadValue<Vector2>();
	});
	actionMap->AddAction(CameraMovement);

	InputAction CameraRotation(InputActionType::Button, {
		{ {KeyCode::Q}, {KeyCode::E} }
	});
	CameraRotation.AddCallBack([&](InputAction::TriggerInfo& info) {
		m_CameraRotationDirection = info.x;
	});
	actionMap->AddAction(CameraRotation);

	InputAction EscapeKeyAction(InputActionType::Button, {
		{ {KeyCode::Escape, true, false} }
	});
	EscapeKeyAction.AddCallBack([&](InputAction::TriggerInfo& info) {
		m_Camera.SetPosition({ 0.0f, 0.0f, 0.0f });
		m_Camera.SetRotation({ 0.0f, 0.0f, 0.0f });
		m_Camera.SetOrthographicSize(1.0f);
	});
	actionMap->AddAction(EscapeKeyAction);
}

void SandboxLayer::OnAttach()
{
	std::stringstream ss = std::stringstream();
	ss << "Vendor: " << Application::Get().GetMainWindow().GetRendererContext()->GetInfoVendor();
	vendorInfo = ss.str();

	ss = std::stringstream();
	ss << "Renderer: " << Application::Get().GetMainWindow().GetRendererContext()->GetInfoRenderer();
	rendererInfo = ss.str();

	ss = std::stringstream();
	ss << "Version: " << Application::Get().GetMainWindow().GetRendererContext()->GetInfoVersion();
	versionInfo = ss.str();
}

void SandboxLayer::OnUpdate(TimeStep DeltaTime)
{
	// Update
	Vector3 offset(m_CameraMovementDirection, 0.0f);
	offset = offset * (m_CameraSpeed * DeltaTime * m_Camera.GetOrthographicSize());
	Mat4x4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(m_Camera.GetRotation().z), Vector3(0, 0, 1));
	m_Camera.SetPosition(Vector4(m_Camera.GetPosition(), 1.0f) + (rotationMatrix * Vector4(offset, 1.0f)));

	m_Camera.SetRotation({ 0.0f, 0.0f, m_Camera.GetRotation().z + m_CameraRotationDirection * DeltaTime * 80.0f });

	// Render
	RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	RenderCommand::Clear();

	Renderer::BeginScene(m_Camera);

	auto flatColorShader = Renderer::GetShaderLibrary().Get("FlatColor");
	flatColorShader->Bind();
	flatColorShader->UploadUniformFloat4("u_Color", Color(0.2, 0.3, 0.8, 1.0));

	for (int x = 0; x <= 20; x++)
	{
		for (int y = 0; y <= 20; y++)
		{
			Transform t;
			t.SetPosition({ x * 0.11f + 0.3f, y * 0.11f, 0.0f });
			t.SetRotation(QuaternionEuler({ 0.0f, 0.0f, 0.0f }));
			t.SetScale(Vector3(0.1f));

			Renderer::Submit(flatColorShader, m_SquareVA, t.GetTransformationMatrix());
		}
	}

	auto textureShader = Renderer::GetShaderLibrary().Get("Texture");

	m_CheckerBoardTexture->Bind();
	Renderer::Submit(textureShader, m_SquareVA, m_SquareTransform.GetTransformationMatrix());

	m_OELogoTexture->Bind();
	Renderer::Submit(textureShader, m_SquareVA, m_SquareTransform.GetTransformationMatrix());

	// Triangle
	// Renderer::Submit(m_Shader, m_VertexArray, m_TriangleTransform.GetTransformationMatrix());

	Renderer::EndScene();
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

void SandboxLayer::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>(OE_BIND_EVENT_FN(SandboxLayer::OnWindowResizeEvent));
	dispatcher.Dispatch<MouseScrolledEvent>(OE_BIND_EVENT_FN(SandboxLayer::OnMouseScrolledEvent));
}

bool SandboxLayer::OnWindowResizeEvent(WindowResizeEvent& event)
{
	m_Camera.SetAspectRatio((float)event.GetWidth() / (float)event.GetHeight());
	return false;
}

bool SandboxLayer::OnMouseScrolledEvent(MouseScrolledEvent& event)
{
	float newSize = m_Camera.GetOrthographicSize() - (float)event.GetYOffset() / 4.0f;
	if (newSize > 0)
		m_Camera.SetOrthographicSize(newSize);
	return false;
}
#endif
