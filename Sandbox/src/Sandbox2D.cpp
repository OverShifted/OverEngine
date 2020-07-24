#include "Sandbox2D.h"

#include "imgui/imgui.h"

using namespace OverEngine;

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraMovementDirection(0.0f)
{
	OE_PROFILE_FUNCTION();
#pragma region CameraSetup
	Application& app = Application::Get();

	float aspectRatio = (float)app.GetMainWindow().GetWidth() / (float)app.GetMainWindow().GetHeight();
	m_Camera = OrthographicCamera(1.0f, aspectRatio);
#pragma endregion

#pragma region Input
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
#pragma endregion

#pragma region Textures
	m_CheckerBoardTexture = Texture2D::MasterFromFile("assets/textures/Checkerboard.png");
	m_CheckerBoardTexture->SetSWrapping(TextureWrapping::ClampToBorder);
	m_CheckerBoardTexture->SetTWrapping(TextureWrapping::ClampToBorder);
	m_CheckerBoardTexture->SetBorderColor({ 0.0f, 1.0f, 1.0f, 1.0f });
	m_CheckerBoardTexture->SetFilter(TextureFiltering::Nearest);

	m_OELogoTexture = Texture2D::MasterFromFile("assets/textures/OELogo.png");

	m_SpriteSheet = Texture2D::MasterFromFile("assets/textures/SpriteSheetTest.png");
	m_SpriteSheet->SetSWrapping(TextureWrapping::ClampToBorder);
	m_SpriteSheet->SetTWrapping(TextureWrapping::ClampToBorder);
	m_SpriteSheet->SetBorderColor({ 0.0f, 1.0f, 1.0f, 1.0f });

	m_Sprite = Texture2D::SubTextureFromExistingOne(m_SpriteSheet, { 128 * 10, 0, 128, 128 });
#pragma endregion
}

static Vector<float> s_FPSSamples(200);

void Sandbox2D::OnUpdate(TimeStep DeltaTime)
{
	OE_PROFILE_FUNCTION();

	// Update
	Vector3 offset(m_CameraMovementDirection, 0.0f);
	offset = offset * (m_CameraSpeed * DeltaTime * m_Camera.GetOrthographicSize());
	Mat4x4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(m_Camera.GetRotation().z), Vector3(0, 0, 1));
	m_Camera.SetPosition(Vector4(m_Camera.GetPosition(), 1.0f) + (rotationMatrix * Vector4(offset, 1.0f)));

	m_Camera.SetRotation({ 0.0f, 0.0f, m_Camera.GetRotation().z + m_CameraRotationDirection * DeltaTime * 80.0f });

	for (uint32_t i = 0; i < (uint32_t)((int)s_FPSSamples.size() - 1); i++)
	{
		s_FPSSamples[i] = s_FPSSamples[i + 1];
	}

	s_FPSSamples[(int)s_FPSSamples.size() - 1] = 1 / Time::GetDeltaTime();
	if (s_FPSSamples[(int)s_FPSSamples.size() - 1] > 10000)
		s_FPSSamples[(int)s_FPSSamples.size() - 1] = 0;

	// Render
	RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	RenderCommand::Clear();

	Renderer2D::BeginScene(m_Camera);

	Renderer2D::DrawQuad({ -0.0f, -0.0f, -0.1f }, 0.0f, { 2.3f, 2.3f }, { 1.0f, 1.0f, 1.0f, 1.0f });

	Renderer2D::DrawQuad({ -0.54f, -0.54f }, 0.0f, { 1.0f, 1.0f }, { 0.00f, 0.64f, 0.93f, 1.0f });

	// Renderer2D::DrawQuad({ 0.54f,  0.54f }, 0.0f, { 1.0f, 1.0f }, { 0.49f, 0.72f, 0.00f, 1.0f });
	Renderer2D::DrawQuad({ 0.54f,  0.54f }, 0.0f, { 1.0f, 1.0f }, m_CheckerBoardTexture);

	// Renderer2D::DrawQuad({ -0.54f,  0.54f }, 0.0f, { 1.0f, 1.0f }, { 0.94f, 0.31f, 0.13f, 1.0f });
	Renderer2D::DrawQuad({ -0.54f,  0.54f }, 0.0f, { 1.0f, 1.0f }, m_Sprite);

	Renderer2D::DrawQuad({  0.54f, -0.54f }, 0.0f, { 1.0f, 1.0f }, { 1.00f, 0.72f, 0.00f, 1.0f });

	Renderer2D::EndScene();
}


void Sandbox2D::OnImGuiRender()
{
	OE_PROFILE_FUNCTION();

	ImGui::Begin("Camera");
	ImGui::DragFloat2("Position", glm::value_ptr(const_cast<Vector3&>(m_Camera.GetPosition())), m_Camera.GetOrthographicSize() / 20);
	// ImGui::DragFloat("Rotation", glm::value_ptr(const_cast<Vector3&>(m_Camera.GetRotation())));
	// ImGui::DragFloat("Size", &(const_cast<float&>(m_Camera.GetOrthographicSize())));
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Texture Debugger");

	uint32_t cellSize = 8;
	for (int x = 0; x < ImGui::GetContentRegionAvail().x / cellSize + 1; x++)
	{
		for (int y = 0; y < ImGui::GetContentRegionAvail().y / cellSize + 1; y++)
		{
			float colorTint = (x % 2 == 0 && y % 2 != 0) || (x % 2 != 0 && y % 2 == 0) ? 0.8f : 1.0f;
			ImGui::GetWindowDrawList()->AddRectFilled(
				ImVec2({ ImGui::GetCursorScreenPos().x + x * cellSize, ImGui::GetCursorScreenPos().y + y * cellSize }),
				ImVec2({ ImGui::GetCursorScreenPos().x + (x + 1) * cellSize, ImGui::GetCursorScreenPos().y + (y + 1) * cellSize }),
				ImGui::ColorConvertFloat4ToU32(ImVec4(colorTint, colorTint, colorTint, 1.0f))
			);

		}
	}
	
	ImGui::Image((void*)1, ImGui::GetContentRegionAvail());
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::Begin("Renderer2D Statistics");
	
	char fps[32];
	sprintf(fps, "%f", s_FPSSamples[(int)s_FPSSamples.size() - 1]);
	ImGui::PlotLines("FPS", s_FPSSamples.data(), (int)s_FPSSamples.size(), 0, fps, FLT_MAX, FLT_MAX, ImVec2{ 0, 80 });

	ImGui::Text("DrawCalls : %i", Renderer2D::GetStatistics().DrawCalls);
	ImGui::Text("QuadCount : %i", Renderer2D::GetStatistics().QuadCount);
	ImGui::Text("IndexCount : %i", Renderer2D::GetStatistics().GetIndexCount());
	ImGui::Text("VertexCount : %i", Renderer2D::GetStatistics().GetVertexCount());

	ImGui::End();
}

void Sandbox2D::OnEvent(Event& event)
{
	OE_PROFILE_FUNCTION();

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>(OE_BIND_EVENT_FN(Sandbox2D::OnWindowResizeEvent));
	dispatcher.Dispatch<MouseScrolledEvent>(OE_BIND_EVENT_FN(Sandbox2D::OnMouseScrolledEvent));
}

bool Sandbox2D::OnWindowResizeEvent(WindowResizeEvent& event)
{
	OE_PROFILE_FUNCTION();

	m_Camera.SetAspectRatio((float)event.GetWidth() / (float)event.GetHeight());
	return false;
}

bool Sandbox2D::OnMouseScrolledEvent(MouseScrolledEvent& event)
{
	OE_PROFILE_FUNCTION();

	float newSize = m_Camera.GetOrthographicSize() - (float)event.GetYOffset() / 4.0f;
	if (newSize > 0)
		m_Camera.SetOrthographicSize(newSize);
	return false;
}