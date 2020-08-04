#include "pcheader.h"
#include "EditorLayer.h"

#include "SceneLoader.h"
#include "GUI/OnEditorGUI.h"

#include "imgui/imgui.h"

/*EditorLayer::EditorLayer()
	: Layer("Editor Layer")
{
	FrameBufferProps fbProps;
	fbProps.Width = 1280;
	fbProps.Height = 720;
	m_SceneFrameBuffer = FrameBuffer::Create(fbProps);

	m_GameScene = CreateRef<Scene>();

	Entity cameraEntity = m_GameScene->CreateEntity("Main Camera");
	auto& camera = cameraEntity.AddComponent<CameraComponent>(Camera(CameraType::Orthographic, 10.0f, 16.0f / 0.9f, -1.0f, 1.0f));
	camera.Camera.SetClearColor(Color(1.0f, 0.0f, 1.0f, 1.0f));

	Ref<Texture2D> playerSprite = Texture2D::MasterFromFile("assets/textures/Checkerboard.png");

	Entity player = m_GameScene->CreateEntity("Player");
	player.AddComponent<SpriteRendererComponent>(playerSprite);
}

void EditorLayer::OnAttach()
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

void EditorLayer::OnUpdate(OverEngine::TimeStep DeltaTime)
{
	//m_SceneFrameBuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
	m_SceneFrameBuffer->Resize(512, 512);
	m_SceneFrameBuffer->Bind();

	m_GameScene->OnUpdate(DeltaTime);

	m_SceneFrameBuffer->Unbind();
}

void EditorLayer::OnImGuiRender()
{
	MainMenuBar();
	MainDockSpace();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

	ImGui::Begin("Viewport");

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	ImGui::Image((void*)(intptr_t)m_SceneFrameBuffer->GetColorAttachmentRendererID(), viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	OE_CORE_INFO("{}, {}", m_SceneFrameBuffer->GetProps().Width, m_SceneFrameBuffer->GetProps().Height);

	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorLayer::OnEvent(OverEngine::Event& event)
{
}
*/

EditorLayer::EditorLayer()
	: Layer("EditorLayer"), m_CameraMovementDirection(0.0f)
{
	OE_PROFILE_FUNCTION();

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
		m_CameraRotationDirection = -info.x;
	});
	actionMap->AddAction(CameraRotation);

	InputAction EscapeKeyAction(InputActionType::Button, {
		{ {KeyCode::Escape, true, false} }
	});
	EscapeKeyAction.AddCallBack([&](InputAction::TriggerInfo& info) {
		m_MainCameraTransform->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_MainCameraTransform->SetRotation(QuaternionEuler({ 0.0f, 0.0f, 0.0f }));
		m_MainCameraCameraHandle->SetOrthographicSize(10.0f);
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

	m_SpriteSheet = Texture2D::MasterFromFile("assets/textures/platformPack_tilesheet@2.png");
	m_SpriteSheet->SetSWrapping(TextureWrapping::ClampToBorder);
	m_SpriteSheet->SetTWrapping(TextureWrapping::ClampToBorder);
	m_SpriteSheet->SetBorderColor({ 0.0f, 1.0f, 1.0f, 1.0f });
	m_SpriteSheet->SetFilter(TextureFiltering::Nearest);

	m_Sprite = Texture2D::SubTextureFromExistingOne(m_SpriteSheet, { 128 * 4, 128 * 2, 128, 128 });
	m_ObstacleSprite = Texture2D::SubTextureFromExistingOne(m_SpriteSheet, { 128 * 0, 128 * 0, 128, 128 });
	#pragma endregion

	#pragma region ECS
	m_Scene = CreateRef<Scene>();

	////////////////////////////////////////////////////////////////
	// Player //////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////

	m_Player = m_Scene->CreateEntity("Player");

	// SpriteRenderer
	m_Player.AddComponent<SpriteRendererComponent>(m_Sprite);

	// PhysicsBody2D
	PhysicsBodyProps props;
	props.Type = PhysicsBodyType::Dynamic;
	m_Player.AddComponent<PhysicsBodyComponent>(props);

	// PhysicsCollider2D
	auto& playerColliderList = m_Player.AddComponent<PhysicsColliders2DComponent>();
	auto playerCollider = CreateRef<PhysicsCollider2D>();
	playerCollider->GetShape()->SetAsBox({ 1.0f, 1.0f });
	playerCollider->GetMaterial().Bounciness = 0.9f;
	playerColliderList.AddCollider(playerCollider);

	////////////////////////////////////////////////////////////////
	// Obstacle ////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////

	Entity obstacle = m_Scene->CreateEntity("Obstacle");

	// SpriteRenderer
	auto& spriteRenderer = obstacle.AddComponent<SpriteRendererComponent>(m_ObstacleSprite);
	spriteRenderer.TilingFactorX = 4.0f;
	spriteRenderer.OverrideSWrapping = TextureWrapping::Repeat;
	spriteRenderer.OverrideTWrapping = TextureWrapping::Repeat;

	// PhysicsBody2D
	PhysicsBodyProps obstacleBodyProps;
	obstacleBodyProps.Type = PhysicsBodyType::Static;
	obstacle.AddComponent<PhysicsBodyComponent>(obstacleBodyProps);

	auto& obstacleTransform = obstacle.GetComponent<TransformComponent>();
	obstacleTransform.SetPosition({ 0.0f, -2.0f, 0.0f });
	obstacleTransform.SetScale({ 4.0f, 1.0f, 1.0f });
	obstacleTransform.SetRotation(QuaternionEuler({ 0.0f, 0.0f, 90.0f }));

	// PhysicsCollider2D
	auto& colliderList = obstacle.AddComponent<PhysicsColliders2DComponent>();
	auto collider = CreateRef<PhysicsCollider2D>();
	collider->GetShape()->SetAsBox({ 4.0f, 1.0f });
	colliderList.AddCollider(collider);

	////////////////////////////////////////////////////////////////
	// Obstacle2 ///////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////

	Entity obstacle2 = m_Scene->CreateEntity("Obstacle2");

	// SpriteRenderer
	auto& spriteRenderer2 = obstacle2.AddComponent<SpriteRendererComponent>(m_ObstacleSprite);
	spriteRenderer2.TilingFactorX = 40.0f;
	spriteRenderer2.OverrideSWrapping = TextureWrapping::Repeat;
	spriteRenderer2.OverrideTWrapping = TextureWrapping::Repeat;

	// PhysicsBody2D
	PhysicsBodyProps obstacle2BodyProps;
	obstacle2BodyProps.Type = PhysicsBodyType::Static;
	obstacle2.AddComponent<PhysicsBodyComponent>(obstacle2BodyProps);


	auto& obstacle2Transform = obstacle2.GetComponent<TransformComponent>();
	obstacle2Transform.SetScale({ 40.0f, 1.0f, 1.0f });
	obstacle2Transform.SetPosition({ 0.0f, -8.0f, 0.0f });
	//obstacle2Transform.SetRotation(QuaternionEuler({ 0.0f, 0.0f, 90.0f }));

	// PhysicsCollider2D
	auto& colliderList2 = obstacle2.AddComponent<PhysicsColliders2DComponent>();
	auto collider2 = CreateRef<PhysicsCollider2D>();
	collider2->GetShape()->SetAsBox({ 40.0f, 1.0f });
	colliderList2.AddCollider(collider2);

	////////////////////////////////////////////////////////////////
	// Main Camera /////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////

	m_MainCamera = m_Scene->CreateEntity("MainCamera");
	m_MainCameraCameraHandle = &m_MainCamera.AddComponent<CameraComponent>(Camera(CameraType::Orthographic, 10.0f, 16.0f / 9.0f, -1.0f, 1.0f)).Camera;
	m_MainCameraTransform = &m_MainCamera.GetComponent<TransformComponent>();
	#pragma endregion

	FrameBufferProps fbProps;
	fbProps.Width = 7680;
	fbProps.Height = 6320;
	m_SceneFrameBuffer = FrameBuffer::Create(fbProps);
	
	LoadSceneFromFile("");
}

static Vector<float> s_FPSSamples(200);
static bool VSync = true;
static char fpsText[32];

void EditorLayer::OnUpdate(TimeStep DeltaTime)
{
	OE_PROFILE_FUNCTION();

	m_MainCameraCameraHandle = &m_MainCamera.GetComponent<CameraComponent>().Camera;
	m_MainCameraTransform = &m_MainCamera.GetComponent<TransformComponent>();

	// UpdateO
	Vector3 offset(m_CameraMovementDirection, 0.0f);
	offset = offset * (m_CameraSpeed * DeltaTime * m_MainCameraCameraHandle->GetOrthographicSize());
	Mat4x4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(QuaternionEulerAngles(m_MainCameraTransform->GetRotation()).z), Vector3(0, 0, 1));
	m_MainCameraTransform->SetPosition(Vector4(m_MainCameraTransform->GetPosition(), 0.0f) + (rotationMatrix * Vector4(offset, 1.0f)));

	m_MainCameraTransform->SetRotation(QuaternionEuler({ 0.0f, 0.0f, QuaternionEulerAngles(m_MainCameraTransform->GetRotation()).z + m_CameraRotationDirection * DeltaTime * 80.0f }));

	for (uint32_t i = 0; i < (uint32_t)((int)s_FPSSamples.size() - 1); i++)
	{
		s_FPSSamples[i] = s_FPSSamples[i + (uint32_t)1];
	}

	s_FPSSamples[(int)s_FPSSamples.size() - 1] = 1 / DeltaTime;
	if (s_FPSSamples[(int)s_FPSSamples.size() - 1] > 10000)
		s_FPSSamples[(int)s_FPSSamples.size() - 1] = 0;

	sprintf_s(fpsText, "%i", (int)s_FPSSamples[(int)s_FPSSamples.size() - 1]);


	if (FrameBufferProps props = m_SceneFrameBuffer->GetProps();
		m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
		(props.Width != m_ViewportSize.x || props.Height != m_ViewportSize.y))
	{
		m_SceneFrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_MainCameraCameraHandle->SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);
	}

	RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	RenderCommand::Clear();

	m_SceneFrameBuffer->Bind();
	m_Scene->OnUpdate(DeltaTime);
	m_SceneFrameBuffer->Unbind();
}

void EditorLayer::OnImGuiRender()
{
	OnMainMenubarGUI();
	OnMainDockSpaceGUI();

	OnToolbarGUI();
	m_ViewportSize = OnViewportGUI(m_SceneFrameBuffer, m_MainCameraCameraHandle);
	OnSceneGraphGUI(m_Scene);
	OnAssetsGUI();
	OnConsoleGUI();
}

void EditorLayer::OnEvent(Event& event)
{
	OE_PROFILE_FUNCTION();

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>(OE_BIND_EVENT_FN(EditorLayer::OnWindowResizeEvent));
	dispatcher.Dispatch<MouseScrolledEvent>(OE_BIND_EVENT_FN(EditorLayer::OnMouseScrolledEvent));
}

bool EditorLayer::OnWindowResizeEvent(WindowResizeEvent& event)
{
	OE_PROFILE_FUNCTION();

	m_MainCameraCameraHandle->SetAspectRatio((float)event.GetWidth() / (float)event.GetHeight());
	return false;
}

bool EditorLayer::OnMouseScrolledEvent(MouseScrolledEvent& event)
{
	OE_PROFILE_FUNCTION();

	float newSize = m_MainCameraCameraHandle->GetOrthographicSize() - (float)event.GetYOffset() / 4.0f;
	if (newSize > 0)
		m_MainCameraCameraHandle->SetOrthographicSize(newSize);
	return false;
}