#include "SandboxECS.h"

#include <OverEngine/Scripting/ScriptingEngine.h>
#include <OverEngine/Core/FileSystem/FileSystem.h>
#include <imgui.h>

using namespace OverEngine;

SandboxECS::SandboxECS()
	: Layer("SandboxECS"), m_CameraMovementDirection(0.0f)
{
	OE_PROFILE_FUNCTION();

#pragma region Input
	auto actionMap = InputActionMap::Create();

	InputAction CameraMovement(InputActionType::Button, {
		/*{
			{KeyCode::A}, {KeyCode::D},
			{KeyCode::S}, {KeyCode::W}
		},*/
		{
			{KeyCode::Left}, {KeyCode::Right},
			{KeyCode::Down}, {KeyCode::Up}
		}
	});
	CameraMovement.AddCallBack([&](const auto& info) {
		m_CameraMovementDirection = { info.x, info.y };
	});
	actionMap->AddAction(CameraMovement);

	InputAction CameraRotation(InputActionType::Button, {
		{ {KeyCode::Q}, {KeyCode::E} }
	});
	CameraRotation.AddCallBack([&](const auto& info) {
		m_CameraRotationDirection = -info.x;
	});
	actionMap->AddAction(CameraRotation);

	InputAction EscapeKeyAction(InputActionType::Button, {
		{ {KeyCode::Escape, true, false} }
	});
	EscapeKeyAction.AddCallBack([&](const auto& info) {
		m_MainCameraTransform->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_MainCameraTransform->SetEulerAngles({ 0.0f, 0.0f, 0.0f });
		m_MainCameraCameraHandle->SetOrthographicSize(10.0f);
	});
	actionMap->AddAction(EscapeKeyAction);
#pragma endregion

#pragma region Textures
	m_SpriteSheet = Texture2D::Create("assets/textures/platformPack_tilesheet@2.png");
	m_SpriteSheet->SetUWrap(TextureWrap::Repeat);
	m_SpriteSheet->SetVWrap(TextureWrap::Repeat);
	m_SpriteSheet->SetFilter(TextureFilter::Nearest);

	m_Sprite = SubTexture2D::Create(m_SpriteSheet, { 128 * 10, 128 * 0, 128, 128 });
	m_ObstacleSprite = SubTexture2D::Create(m_SpriteSheet, { 128 * 0, 128 * 0, 128, 128 });
#pragma endregion

#pragma region ECS
	m_Scene = CreateRef<Scene>();

	////////////////////////////////////////////////////////////////
	// Player //////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////

	{
		m_Player = m_Scene->CreateEntity("Player");

		// SpriteRenderer
		m_Player.AddComponent<SpriteRendererComponent>(m_Sprite);

		// RigidBody2D
		{
			RigidBody2DProps props;
			props.Type = RigidBody2DType::Dynamic;
			m_Player.AddComponent<RigidBody2DComponent>(props);
		}

		// Collider2D
		{
			auto& playerColliderList = m_Player.AddComponent<Colliders2DComponent>();
			Collider2DProps props;
			props.Shape = CircleCollisionShape2D::Create(0.5f);
			props.Bounciness = 0.0f;
			props.Friction = 100.0f;
			props.Density = 1.0f;
			playerColliderList.Colliders.push_back(Collider2D::Create(props));
		}

		m_Player.AddComponent<LuaScriptsComponent>().Scripts["player"] = FileSystem::ReadFile("assets/scripts/player.lua");
	}

	////////////////////////////////////////////////////////////////
	// Obstacle ////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////

	{
		Entity obstacle = m_Scene->CreateEntity("Obstacle");

		// SpriteRenderer
		auto& spriteRenderer = obstacle.AddComponent<SpriteRendererComponent>(m_ObstacleSprite);
		spriteRenderer.Tiling.x = 4.0f;
		spriteRenderer.ForceTile = true;

		auto& obstacleTransform = obstacle.GetComponent<TransformComponent>();
		obstacleTransform.SetLocalPosition({ 0.0f, -2.0f, 0.0f });
		obstacleTransform.SetLocalScale({ 4.0f, 1.0f, 1.0f });
		obstacleTransform.SetLocalEulerAngles({ 0.0f, 0.0f, 45.0f });

		// RigidBody2D
		{
			RigidBody2DProps props;
			props.Type = RigidBody2DType::Static;
			obstacle.AddComponent<RigidBody2DComponent>(props);
		}

		// Collider2D
		{
			auto& colliderList = obstacle.AddComponent<Colliders2DComponent>();
			Collider2DProps props;
			props.Shape = BoxCollisionShape2D::Create({ 4.0f, 1.0f });
			props.Bounciness = 0.3f;
			props.Friction = 1.0f;
			props.Density = 200.0f;
			colliderList.Colliders.push_back(Collider2D::Create(props));
		}
	}

	////////////////////////////////////////////////////////////////
	// Obstacle2 ///////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////

	{
		Entity obstacle = m_Scene->CreateEntity("Obstacle2");

		// SpriteRenderer
		auto& spriteRenderer = obstacle.AddComponent<SpriteRendererComponent>(m_ObstacleSprite);
		spriteRenderer.Tiling.x = 4000.0f;
		spriteRenderer.Tiling.y = 1.0f;
		spriteRenderer.ForceTile = true;

		auto& obstacleTransform = obstacle.GetComponent<TransformComponent>();
		obstacleTransform.SetLocalScale({ 4000.0f, 1.0f, 1.0f });
		obstacleTransform.SetLocalPosition({ 0.0f, -8.0f, 0.0f });

		// RigidBody2D
		{
			RigidBody2DProps obstacleBodyProps;
			obstacleBodyProps.Type = RigidBody2DType::Static;
			obstacle.AddComponent<RigidBody2DComponent>(obstacleBodyProps);
		}

		// Collider2D
		{
			auto& colliderList = obstacle.AddComponent<Colliders2DComponent>();
			Collider2DProps props;
			props.Shape = BoxCollisionShape2D::Create({ 4000.0f, 1.0f });
			props.Bounciness = 0.3f;
			props.Friction = 1.0f;
			props.Density = 200.0f;
			colliderList.Colliders.push_back(Collider2D::Create(props));
		}
	}

	////////////////////////////////////////////////////////////////
	// Main Camera /////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////

	m_MainCamera = m_Scene->CreateEntity("MainCamera");
	auto& app = Application::Get();
	m_MainCameraCameraHandle = &m_MainCamera.AddComponent<CameraComponent>().Camera;
	m_MainCameraCameraHandle->SetOrthographic(10.0f, -1.0f, 1.0f);
	m_MainCameraTransform = &m_MainCamera.GetComponent<TransformComponent>();

	m_MainCameraTransform->SetLocalPosition({ 0.0f, -1.0f, 0.0f });
	m_MainCameraCameraHandle->SetOrthographicSize(25);

	#pragma endregion

	class Player : public ScriptableEntity
	{
	public:

		Player(ParticleSystem2D* particleSystem)
			: m_ParticleSystem(particleSystem)
		{
		}

		virtual void OnUpdate(TimeStep ts) override
		{
			static float mult = 10 * ts;

			bool moving = false;
			Vector2 vel(0.0f);

			if (Input::IsKeyPressed(KeyCode::A))
			{
				vel.x -= mult;
				moving = true;
			}
			if (Input::IsKeyPressed(KeyCode::D))
			{
				vel.x += mult;
				moving = true;
			}
			if (Input::IsKeyPressed(KeyCode::W))
			{
				vel.y += mult;
				moving = true;
			}
			if (Input::IsKeyPressed(KeyCode::S))
			{
				vel.y -= mult;
				moving = true;
			}

			auto& rb = GetComponent<RigidBody2DComponent>().RigidBody;
			rb->ApplyLinearImpulseToCenter(vel);

			m_ParticleProps.Position = GetComponent<TransformComponent>().GetPosition();
			m_ParticleProps.Position.z = -0.2f;

			m_ParticleProps.Position.x += Random::Range(-0.2f, 0.2f);
			m_ParticleProps.Position.y += Random::Range(-0.2f, 0.2f);

			m_ParticleProps.BirthSize = Vector2(0.2f);

			m_ParticleProps.RenderingProps.Sprite = GetComponent<SpriteRendererComponent>().Sprite;
			m_ParticleProps.RenderingProps.BirthColor = Color(0.8f, 0.8f, 0.8f, 1.0f);

			float len = glm::fastLength(rb->GetLinearVelocity());
			// OE_CORE_INFO(len);
			for (int i = 0; i < len * ts; i += 1)
			{
				m_ParticleSystem->Emit(m_ParticleProps);
			}
		}

		virtual void OnCollisionEnter(const SceneCollision2D& collision) override
		{
			// OE_CORE_INFO("OnCollisionEnter with {}", collision.Other.Entity.GetComponent<NameComponent>().Name);
			// GetComponent<SpriteRendererComponent>().Tint = { 1.0f, 0.2f, 1.0f, 1.0f };
			// GetComponent<NameComponent>()._Reflection.dump(&GetComponent<NameComponent>(), 1);
			// SpriteRendererComponent::_Reflection.Dump(&GetComponent<SpriteRendererComponent>(), 1);
			// YAML::Emitter e;

			// RigidBody2DComponent::_Reflection.Serialize(&GetComponent<RigidBody2DComponent>(), e);

			// OE_CORE_INFO(e.c_str());
		}

		virtual void OnCollisionExit(const SceneCollision2D& collision) override
		{
			// GetComponent<SpriteRendererComponent>().Tint = { 1.0f, 1.0f, 1.0f, 1.0f };
		}

	private:
		ParticleSystem2D* m_ParticleSystem;

		Particle2DProps m_ParticleProps = Particle2DProps();
	};

	class CameraController : public ScriptableEntity
	{
	public:

        CameraController(const Entity& player)
			: m_PlayerEntity(player)
		{
		}

		virtual void OnLateUpdate(TimeStep ts) override
		{
			auto& tc = GetComponent<TransformComponent>();
			const Vector3& playerPosition = m_PlayerEntity.GetComponent<TransformComponent>().GetLocalPosition();
			const Vector3& currentPosition = tc.GetLocalPosition();
			tc.SetPosition(glm::mix(currentPosition, playerPosition, 10.0f * ts));
		}

	private:
		Entity m_PlayerEntity;
	};

	m_Player.AddComponent<NativeScriptsComponent>().AddScript<Player>(&m_ParticleSystem);
	m_MainCamera.AddComponent<NativeScriptsComponent>().AddScript<CameraController>(m_Player);

	m_Scene->OnScenePlay();
	ImGui::GetStyle().Alpha = 0.8f;
}

static Vector<float> s_FPSSamples(200);
static int maxFPS = 0;
static bool VSync = true;
static char fpsText[32];

static int emitPerFrame = 1;

void SandboxECS::OnUpdate(TimeStep deltaTime)
{
	OE_PROFILE_FUNCTION();

	m_MainCameraCameraHandle = &m_MainCamera.GetComponent<CameraComponent>().Camera;
	m_MainCameraTransform = &m_MainCamera.GetComponent<TransformComponent>();

	// Update
	Vector3 offset(m_CameraMovementDirection, 0.0f);
	offset = offset * (m_CameraSpeed * deltaTime * m_MainCameraCameraHandle->GetOrthographicSize());
	Mat4x4 rotationMatrix = glm::rotate(IDENTITY_MAT4X4, glm::radians(m_MainCameraTransform->GetEulerAngles().z), { 0, 0, 1 });
	m_MainCameraTransform->SetPosition(Vector4(m_MainCameraTransform->GetPosition(), 0.0f) + (rotationMatrix * Vector4(offset, 1.0f)));

	m_MainCameraTransform->SetEulerAngles({ 0.0f, 0.0f, m_MainCameraTransform->GetEulerAngles().z + m_CameraRotationDirection * deltaTime * 80.0f });

	for (uint32_t i = 0; i < (uint32_t)((int)s_FPSSamples.size() - 1); i++)
	{
		s_FPSSamples[i] = s_FPSSamples[i + 1u];
	}

	s_FPSSamples[(int)s_FPSSamples.size() - 1] = 1 / deltaTime;
	if (s_FPSSamples[(int)s_FPSSamples.size() - 1] > 10000)
		s_FPSSamples[(int)s_FPSSamples.size() - 1] = 0;

	if ((int)s_FPSSamples[(int)s_FPSSamples.size() - 1] > maxFPS)
		maxFPS = (int)s_FPSSamples[(int)s_FPSSamples.size() - 1];

	sprintf_s(fpsText, 32, "%i", (int)s_FPSSamples[(int)s_FPSSamples.size() - 1]);

	Window& win = Application::Get().GetWindow();
	m_Scene->SetViewportSize(win.GetWidth(), win.GetHeight());

//	for (int i = 0; i < emitPerFrame; i++)
//		m_ParticleSystem.Emit({});
	m_Scene->OnUpdate(deltaTime);
    m_ParticleSystem.UpdateAndRender(deltaTime, glm::inverse(m_MainCameraTransform->GetLocalToWorld()), *m_MainCameraCameraHandle);
}

void SandboxECS::OnImGuiRender()
{
	OE_PROFILE_FUNCTION();

	ImGui::Begin("Camera");
	Vector3 pos = m_MainCameraTransform->GetPosition();
	ImGui::PushItemWidth(-1);
	if (ImGui::DragFloat3("##CameraPosition", glm::value_ptr(pos), m_MainCameraCameraHandle->GetOrthographicSize() / 20))
		m_MainCameraTransform->SetPosition(pos);
	ImGui::End();

	#if 0
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

	ImGui::Begin("Texture Debugger1");

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

	ImGui::Begin("Texture Debugger2");

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

	ImGui::Image((void*)2, ImGui::GetContentRegionAvail());
	ImGui::End();

	ImGui::PopStyleVar();
	#endif

	ImGui::Begin("Renderer2D Statistics");

	ImGui::PushItemWidth(-1);
	ImGui::PlotLines("##FPS", s_FPSSamples.data(), (int)s_FPSSamples.size(), 0, fpsText, FLT_MAX, FLT_MAX, ImVec2{ 0, 80 });

	ImGui::Text("DrawCalls : %i   ", Renderer2D::GetStatistics().DrawCalls); ImGui::SameLine();
	ImGui::Text("QuadCount : %i   ", Renderer2D::GetStatistics().QuadCount); ImGui::SameLine();
	ImGui::Text("IndexCount : %i   ", Renderer2D::GetStatistics().GetIndexCount()); ImGui::SameLine();
	ImGui::Text("VertexCount : %i   ", Renderer2D::GetStatistics().GetVertexCount()); ImGui::SameLine();
	ImGui::Text("MaxFPS : %i   ", maxFPS); ImGui::SameLine();

	if (ImGui::Button("Reload Renderer2D Shader"))
		Renderer2D::GetShader()->Reload();

	ImGui::SameLine();

	if (ImGui::Checkbox("V-Sync", &VSync))
		Application::Get().GetWindow().SetVSync(VSync);

	ImGui::DragInt("Emit Per Frame", &emitPerFrame);

	ImGui::End();
}

void SandboxECS::OnEvent(Event& event)
{
	OE_PROFILE_FUNCTION();

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseScrolledEvent>(BIND_FN(SandboxECS::OnMouseScrolledEvent));
}

bool SandboxECS::OnMouseScrolledEvent(MouseScrolledEvent& event)
{
	OE_PROFILE_FUNCTION();

	float newSize = m_MainCameraCameraHandle->GetOrthographicSize() - (float)event.GetYOffset() / 4.0f;
	if (newSize > 0)
		m_MainCameraCameraHandle->SetOrthographicSize(newSize);
	return false;
}
