#include "SandboxECS.h"

#include <OverEngine/Scene/SceneSerializer.h>
#include <wren.h>

#include <OverEngine/ImGui/ExtraImGui.h>
#include <imgui.h>

using namespace OverEngine;

SandboxECS::SandboxECS()
	: Layer("SandboxECS"), m_VM(CreateRef<Wren>())
{
	OE_PROFILE_FUNCTION();

	m_VM->LoadModule("src/wren/scripts");

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
			auto& playerCollider = m_Player.AddComponent<Collider2DComponent>();
			Collider2DProps props;
			props.Shape = CircleCollisionShape2D::Create(0.5f);
			props.Bounciness = 0.2f;
			props.Friction = 100.0f;
			props.Density = 1.0f;
			playerCollider.Collider = Collider2D::Create(props);
		}
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
			auto& collider = obstacle.AddComponent<Collider2DComponent>();
			Collider2DProps props;
			props.Shape = BoxCollisionShape2D::Create({ 1.0f, 1.0f });
			props.Bounciness = 0.3f;
			props.Friction = 1.0f;
			props.Density = 200.0f;
			collider.Collider = Collider2D::Create(props);
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
			auto& collider = obstacle.AddComponent<Collider2DComponent>();
			Collider2DProps props;
			props.Shape = BoxCollisionShape2D::Create({ 1.0f, 1.0f });
			props.Bounciness = 0.3f;
			props.Friction = 1.0f;
			props.Density = 200.0f;
			collider.Collider = Collider2D::Create(props);
		}
	}

	////////////////////////////////////////////////////////////////
	// Main Camera /////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////

	m_MainCamera = m_Scene->CreateEntity("MainCamera");

	auto& cam = m_MainCamera.AddComponent<CameraComponent>().Camera;
	cam.SetOrthographic(25.0f, -1.0f, 1.0f);

	auto& camTransform = m_MainCamera.GetComponent<TransformComponent>();
	camTransform.SetLocalPosition({ 0.0f, -1.0f, 0.0f });

#pragma endregion

	class Player : public ScriptableEntity
	{
	public:

		Player(ParticleSystem2D* particleSystem)
			: m_ParticleSystem(particleSystem)
		{
		}

		virtual void OnCreate() override
		{
			m_ParticleProps.BirthSize = Vector2(0.2f);

			m_ParticleProps.RenderingProps.Sprite = GetComponent<SpriteRendererComponent>().Sprite;
			m_ParticleProps.RenderingProps.BirthColor = Color(0.8f, 0.8f, 0.8f, 1.0f);
		}

		virtual void OnUpdate(TimeStep ts) override
		{
			m_ParticleProps.Position = GetComponent<TransformComponent>().GetPosition();
			m_ParticleProps.Position.x += Random::Range(-0.2f, 0.2f);
			m_ParticleProps.Position.y += Random::Range(-0.2f, 0.2f);
			m_ParticleProps.Position.z = -0.2f; // Move particles behind player

			OE_SIMPLE_FOR_LOOP(2) m_ParticleSystem->Emit(m_ParticleProps);
		}

	private:
		ParticleSystem2D* m_ParticleSystem;
		Particle2DProps m_ParticleProps = Particle2DProps();
	};

	m_Player.AddComponent<NativeScriptsComponent>().AddScript<Player>(&m_ParticleSystem);
	m_Player.AddComponent<ScriptComponent>(m_VM->GetScriptClass("src/wren/scripts", "Player")->Construct(m_Player));

	{
		auto cameraScript = m_VM->GetScriptClass("src/wren/scripts", "CameraController")->Construct(m_MainCamera);
		cameraScript->Call("player=(_)", m_Player);
		m_MainCamera.AddComponent<ScriptComponent>(cameraScript);
	}

	m_Scene->OnScenePlay();
	ImGui::GetStyle().Alpha = 0.9f;
}

void SandboxECS::OnUpdate(TimeStep deltaTime)
{
	OE_PROFILE_FUNCTION();

	auto& camTransform = m_MainCamera.GetComponent<TransformComponent>();

	Window& win = Application::Get().GetWindow();
	m_Scene->SetViewportSize(win.GetWidth(), win.GetHeight());

	m_Scene->OnUpdate(deltaTime);
	m_ParticleSystem.UpdateAndRender(deltaTime, glm::inverse(camTransform.GetLocalToWorld()), m_MainCamera.GetComponent<CameraComponent>().Camera);

	m_VM->Call(m_VM->GetScheduler(), m_VM->CallHandle("poll()"));
}

void SandboxECS::OnImGuiRender()
{
	OE_PROFILE_FUNCTION();

	if (ImGui::Button("Reload Renderer2D Shader"))
		Renderer2D::GetShader()->Reload();

	if (ImGui::Button("Serialize"))
		SceneSerializer(m_Scene).Serialize("SandboxScene.oes");

	auto& script = m_MainCamera.GetComponent<ScriptComponent>().Script;
	script->Call("onImGuiRender()");

	ImGui::Begin("CameraController script");
	script->Inspect();
	ImGui::End();

	ImGui::Begin("Wren console");
	static String wrenInputValue = "";
	ImGui::PushItemWidth(-35);
	ImGui::InputText("##WrenInput", &wrenInputValue);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button(">"))
	{
		m_VM->ExecuteString(wrenInputValue.c_str());
	}
	ImGui::End();
}

void SandboxECS::OnEvent(Event& event)
{
	OE_PROFILE_FUNCTION();

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseScrolledEvent>(BIND_FN(SandboxECS::OnMouseScrolledEvent));
	dispatcher.Dispatch<KeyPressedEvent>(BIND_FN(SandboxECS::OnKeyPressedEvent));
}

bool SandboxECS::OnMouseScrolledEvent(MouseScrolledEvent& event)
{
	OE_PROFILE_FUNCTION();

	auto& cam = m_MainCamera.GetComponent<CameraComponent>().Camera;

	float newSize = cam.GetOrthographicSize() - (float)event.GetYOffset() / 4.0f;
	if (newSize > 0)
		cam.SetOrthographicSize(newSize);

	return false;
}

bool SandboxECS::OnKeyPressedEvent(KeyPressedEvent& event)
{
	if (event.GetKeyCode() == KeyCode::Escape)
		m_MainCamera.GetComponent<CameraComponent>().Camera.SetOrthographicSize(10.0f);

	return false;
}
