#include "LiquidGlass.h"

#include <imgui.h>

#include "BlurPass.h"

using namespace OverEngine;

Ref<Texture2D> texture = nullptr;
Ref<Texture2D> bg = nullptr;
Ref<FrameBuffer> fb = nullptr;
// Ref<FrameBuffer> finalFb = nullptr;

Scope<BlurPass> blurPass = nullptr;
Ref<FrameBuffer> blurred = nullptr;
Ref<FrameBuffer> blurIntermediate = nullptr;

float blurRatio = 0.5;

int aa = 1;
// int finalAa = 2;
float u_powerFactor = 3.0;
float u_a = 0.7;
float u_b = 2.3;
float u_c = 5.2;
float u_d = 6.9;
float u_blurRadius = 2.0;
float u_fPower = 1.0;
float u_noise = 0.1;

int blurIters = 1;

float velocityMultiplier = 1.0;
float velocity = 2.0;
float size = 3.5;

float cameraVelocityMultiplier = 1.0;
float cameraVelocity = 2.0;

float lastDeltaTime = 0.0;

#define UPLOAD_UNIFORM(name) { \
	Renderer2D::GetShader()->Bind(); \
	Renderer2D::GetShader()->UploadUniformFloat(#name, name); \
}

LiquidGlass::LiquidGlass()
	: Layer("LiquidGlass")
{
	m_Camera.SetOrthographic(15.0f, -10.0f, 10.0f);
	m_Camera.SetViewportSize(1600, 900);

	m_ScreenCamera.SetOrthographic(1.0f, -10.0f, 10.0f);
	m_ScreenCamera.SetViewportSize(1, 1);

	UPLOAD_UNIFORM(u_powerFactor)
	UPLOAD_UNIFORM(u_a);
	UPLOAD_UNIFORM(u_b);
	UPLOAD_UNIFORM(u_c);
	UPLOAD_UNIFORM(u_d);
	UPLOAD_UNIFORM(u_fPower);
	UPLOAD_UNIFORM(u_noise);
	
	texture = Texture2D::Create("assets/textures/liquid-glass.png");
	bg = Texture2D::Create("assets/textures/1482182634171.jpg");
	
	FrameBufferProps props;
	props.Width = 1600 * aa;
	props.Height = 900 * aa;
	fb = FrameBuffer::Create(props);

	props.Width *= blurRatio;
	props.Height *= blurRatio;
	blurred = FrameBuffer::Create(props);
	blurIntermediate = FrameBuffer::Create(props);
	
	blurPass = CreateScope<BlurPass>();
	blurPass->OnResize(1600, 900);
	blurPass->GetShader()->Bind();
	blurPass->GetShader()->UploadUniformFloat("u_radius", u_blurRadius);
}

void LiquidGlass::OnUpdate(TimeStep deltaTime)
{
	lastDeltaTime = deltaTime;

	{
		bool anyKeyPressed = false;
		if (Input::IsKeyPressed(KeyCode::W)) {
			m_Position.y += deltaTime * velocityMultiplier * velocity;
			anyKeyPressed = true;
		}

		if (Input::IsKeyPressed(KeyCode::S)) {
			m_Position.y -= deltaTime * velocityMultiplier * velocity;
			anyKeyPressed = true;
		}

		if (Input::IsKeyPressed(KeyCode::D)) {
			m_Position.x += deltaTime * velocityMultiplier * velocity;
			anyKeyPressed = true;
		}

		if (Input::IsKeyPressed(KeyCode::A)) {
			m_Position.x -= deltaTime * velocityMultiplier * velocity;
			anyKeyPressed = true;
		}

		if (anyKeyPressed) {
			velocityMultiplier += 1.0 * deltaTime;
		} else {
			velocityMultiplier -= 3.0 * deltaTime;
		}

		velocityMultiplier = Math::Clamp(velocityMultiplier, 0.0f, 1.0f);
	}

	{
		bool anyKeyPressed = false;
		if (Input::IsKeyPressed(KeyCode::Up)) {
			m_CameraPosition.y -= deltaTime * cameraVelocityMultiplier * cameraVelocity;
			anyKeyPressed = true;
		}

		if (Input::IsKeyPressed(KeyCode::Down)) {
			m_CameraPosition.y += deltaTime * cameraVelocityMultiplier * cameraVelocity;
			anyKeyPressed = true;
		}

		if (Input::IsKeyPressed(KeyCode::Right)) {
			m_CameraPosition.x -= deltaTime * cameraVelocityMultiplier * cameraVelocity;
			anyKeyPressed = true;
		}

		if (Input::IsKeyPressed(KeyCode::Left)) {
			m_CameraPosition.x += deltaTime * cameraVelocityMultiplier * cameraVelocity;
			anyKeyPressed = true;
		}

		if (anyKeyPressed) {
			cameraVelocityMultiplier += 1.0 * deltaTime;
		} else {
			cameraVelocityMultiplier -= 3.0 * deltaTime;
		}

		cameraVelocityMultiplier = Math::Clamp(cameraVelocityMultiplier, 0.0f, 1.0f);
	}

	// Draw the bg scene
	{
		fb->Bind();
		RenderCommand::SetClearColor(Color{ 0.1, 0.1, 0.1, 1.0 });
		RenderCommand::Clear(ClearFlags_ClearColor | ClearFlags_ClearDepth);

		Renderer2D::BeginScene(glm::translate(IDENTITY_MAT4X4, m_CameraPosition), m_Camera);
		// Renderer2D::DrawQuad({ 0.7, 0.0, 0.0 }, glm::radians(30.0), { 1.2, 1.2 }, Color{ 1, 0.3, 0.2, 1.0 });
		// Renderer2D::DrawQuad({ -0.4, 1.0, 0.0 }, glm::radians(-30.0), { 1.2, 1.2 }, Color{ 0.3, 0.3, 1.0, 1.0 });
		// Renderer2D::DrawQuad({ -2.0, 0.0, 0.0 }, glm::radians(0.0), { 1.2, 1.2 }, TexturedQuadProps { .Sprite = texture });
		Renderer2D::DrawQuad({ 0.0, 0.0, 0.0 }, glm::radians(0.0), { 26, 20.61 }, TexturedQuadProps { .Sprite = bg });
		Renderer2D::EndScene();
	}

	blurPass->Run(fb, blurIntermediate, blurred, blurIters);

	fb->Unbind();
	Window& window = Application::Get().GetWindow();
	Renderer::OnWindowResize(window.GetWidth(), window.GetHeight());

	// Draw the final scene into a high resolution frame buffer
	{
		// finalFb->Bind();
		RenderCommand::SetClearColor(Color{ 0.1, 0.1, 0.1, 1.0 });
		RenderCommand::Clear(ClearFlags_ClearColor | ClearFlags_ClearDepth);
		
		// Render the framebuffer behind liquid glass
		{
			Renderer2D::BeginScene(IDENTITY_MAT4X4, m_ScreenCamera);
			Renderer2D::DrawQuad({0, 0, -1}, glm::radians(0.0), Vector2{ 1 }, Color{ 1 }, 2);
			fb->BindColorAttachment(5);
			Renderer2D::EndScene();
		}
		
		Renderer2D::BeginScene(glm::translate(IDENTITY_MAT4X4, m_CameraPosition), m_Camera);
		Renderer2D::DrawQuad(m_Position, glm::radians(0.0), Vector2{ size }, Color{ 1 }, 1);
		blurred->BindColorAttachment(5);
		Renderer2D::EndScene();
	}

	// finalFb->Unbind();
	// Renderer::OnWindowResize(window.GetWidth(), window.GetHeight());

	// {
	// 	RenderCommand::SetClearColor(Color{ 0.1, 0.1, 0.1, 1.0 });
	// 	RenderCommand::Clear(ClearFlags_ClearColor | ClearFlags_ClearDepth);

	// 	// Render the framebuffer behind liquid glass
	// 	{
	// 		Renderer2D::BeginScene(IDENTITY_MAT4X4, m_ScreenCamera);
	// 		Renderer2D::DrawQuad({0, 0, -1}, glm::radians(0.0), Vector2{ 1 }, Color{ 1 }, 2);
	// 		finalFb->BindColorAttachment(5);
	// 		Renderer2D::EndScene();
	// 	}
	// }
}

void LiquidGlass::OnImGuiRender()
{
	Window& window = Application::Get().GetWindow();
	if (ImGui::DragInt("Antialiasing", &aa, 0.1, 1, 8)) {
		fb->Resize(window.GetWidth() * aa, window.GetHeight() * aa);
		// finalFb->Resize(window.GetWidth() * aa, window.GetHeight() * aa);
		blurred->Resize(window.GetWidth() * blurRatio, window.GetHeight() * blurRatio);
		blurIntermediate->Resize(window.GetWidth() * blurRatio, window.GetHeight() * blurRatio);
	}

	if (ImGui::DragFloat("Power", &u_powerFactor, 0.01, 1.0, 6.0)) UPLOAD_UNIFORM(u_powerFactor);
	ImGui::DragFloat("Size", &size, 0.01, 0.0, 10.0);
	
	ImGui::Separator();
	ImGui::DragInt("Blur Iters", &blurIters, 0.01, 0.0, 10.0);
	if (ImGui::DragFloat("Blur Radius", &u_blurRadius, 0.01, 0.0, 10.0)) {
		blurPass->GetShader()->Bind();
		blurPass->GetShader()->UploadUniformFloat("u_radius", u_blurRadius);
	}

	if (ImGui::DragFloat("Blur downscale", &blurRatio, 0.01, 0.1, 1.0)) {
		blurred->Resize(window.GetWidth() * blurRatio, window.GetHeight() * blurRatio);
		blurIntermediate->Resize(window.GetWidth() * blurRatio, window.GetHeight() * blurRatio);
	}

	if (ImGui::DragFloat("Noise", &u_noise, 0.01, 0, 1.0)) UPLOAD_UNIFORM(u_noise);
	if (ImGui::DragFloat("f(x) Power", &u_fPower, 0.01, -1.5, 6.0)) UPLOAD_UNIFORM(u_fPower);

	ImGui::Separator();

	if (ImGui::DragFloat("a", &u_a, 0.01, 0.0, 5.0)) UPLOAD_UNIFORM(u_a);
	if (ImGui::DragFloat("b", &u_b, 0.01, 0.0, 6.0)) UPLOAD_UNIFORM(u_b);
	if (ImGui::DragFloat("c", &u_c, 0.01, 0.0, 6.0)) UPLOAD_UNIFORM(u_c);
	if (ImGui::DragFloat("d", &u_d, 0.01, 0.0, 10.0)) UPLOAD_UNIFORM(u_d);
	
	// ImGui::Text("%f", velocityMultiplier);
	// ImGui::Text("%f", cameraVelocityMultiplier);
	ImGui::Text("%f FPS", 1 / lastDeltaTime);
	if (ImGui::Button("Reload shader")) {
		Renderer2D::ReloadShader();
		UPLOAD_UNIFORM(u_powerFactor)
		UPLOAD_UNIFORM(u_a);
		UPLOAD_UNIFORM(u_b);
		UPLOAD_UNIFORM(u_c);
		UPLOAD_UNIFORM(u_d);
		UPLOAD_UNIFORM(u_fPower);
		UPLOAD_UNIFORM(u_noise);
	}

	bool vSync = window.IsVSync();
	if (ImGui::Checkbox("VSync", &vSync)) {
		window.SetVSync(vSync);
	}

	// ImGui::Begin("fb");
	// float availWidth = ImGui::GetContentRegionAvail().x;
	// ImGui::Image((void*) fb->GetColorAttachmentRendererID(), { availWidth, availWidth / fb->GetProps().Width * fb->GetProps().Height }, { 0, 1 }, { 1, 0 });
	// ImGui::End();

	// ImGui::Begin("finalFb");
	// ImGui::Image((void*) finalFb->GetColorAttachmentRendererID(), ImGui::GetContentRegionAvail(), { 0, 1 }, { 1, 0 });
	// ImGui::End();

	// ImGui::Begin("blurIntermediate");
	// availWidth = ImGui::GetContentRegionAvail().x;
	// ImGui::Image((void*) blurIntermediate->GetColorAttachmentRendererID(), { availWidth, availWidth / fb->GetProps().Width * fb->GetProps().Height }, { 0, 1 }, { 1, 0 });
	// ImGui::End();

	// ImGui::Begin("blurred");
	// availWidth = ImGui::GetContentRegionAvail().x;
	// ImGui::Image((void*) blurred->GetColorAttachmentRendererID(), { availWidth, availWidth / fb->GetProps().Width * fb->GetProps().Height }, { 0, 1 }, { 1, 0 });
	// ImGui::End();
}

void LiquidGlass::OnEvent(Event& event)
{
	OE_PROFILE_FUNCTION();

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) {
		this->m_Camera.SetViewportSize(e.GetWidth(), e.GetHeight());
		fb->Resize(e.GetWidth() * aa, e.GetHeight() * aa);
		// finalFb->Resize(e.GetWidth() * aa, e.GetHeight() * aa);
		blurred->Resize(e.GetWidth() * blurRatio, e.GetHeight() * blurRatio);
		blurIntermediate->Resize(e.GetWidth() * blurRatio, e.GetHeight() * blurRatio);
		blurPass->OnResize(e.GetWidth(), e.GetHeight());

		return false;
	});

	dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& e) {
		float size = glm::max(0.1, m_Camera.GetOrthographicSize() - e.GetYOffset() * 0.7);
		m_Camera.SetOrthographicSize(size);
		return false;
	});
}
