#include "ViewportPanel.h"

#include <imgui/imgui.h>
#include <glm/gtx/norm.hpp>

namespace OverEditor
{
	static float gridZoom = 6.0f;
	static float gridKernel = 0.2f;

	struct ViewportPanelData
	{
		// Gizmo
		Ref<Shader> GizmoShader = nullptr;
		Ref<VertexArray> GizmoVA = nullptr;
		Ref<VertexBuffer> GizmoVB = nullptr;
		Ref<IndexBuffer> GizmoIB = nullptr;

		// Grid
		Ref<Shader> GridShader = nullptr;
		Ref<VertexArray> GridVA = nullptr;
		Ref<VertexBuffer> GridVB = nullptr;
		Ref<IndexBuffer> GridIB = nullptr;
	};

	static ViewportPanelData* s_Data;

	ViewportPanel::ViewportPanel(const Ref<SceneEditor>& scene)
		: m_Context(scene)
	{
		m_Camera.SetOrthographic(1, -1.0f, 1.0f);
		m_Camera.SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
		m_Camera.SetClearFlags(ClearFlags_None);

		FrameBufferProps fbProps;
		fbProps.Width = 720;
		fbProps.Height = 1280;
		m_FrameBuffer = FrameBuffer::Create(fbProps);

		if (!s_Data)
		{
			s_Data = new ViewportPanelData();

			// Gizmo
			s_Data->GizmoShader = Shader::Create("assets/shaders/ViewportGizmos.glsl");

			s_Data->GizmoVA = VertexArray::Create();

			s_Data->GizmoVB = VertexBuffer::Create();
			s_Data->GizmoVB->SetLayout({
				{ ShaderDataType::Float2, "a_Position" },
				{ ShaderDataType::Float2, "a_UV" }
			});
			static constexpr float quadVertices[] = {
				-0.5f, -0.5f, 0.0f, 0.0f,
				 0.5f, -0.5f, 1.0f, 0.0f,
				 0.5f,  0.5f, 1.0f, 1.0f,
				-0.5f,  0.5f, 0.0f, 1.0f
			};
			s_Data->GizmoVB->BufferData(quadVertices, sizeof(quadVertices));
			s_Data->GizmoVA->AddVertexBuffer(s_Data->GizmoVB);

			s_Data->GizmoIB = IndexBuffer::Create();
			static constexpr uint32_t quadIndices[] = { 0, 1, 2, 2, 3, 0 };
			s_Data->GizmoIB->BufferData(quadIndices, OE_ARRAY_SIZE(quadIndices));
			s_Data->GizmoVA->SetIndexBuffer(s_Data->GizmoIB);

			// Grid
			s_Data->GridShader = Shader::Create("assets/shaders/ViewportGrid2D.glsl");

			s_Data->GridVA = VertexArray::Create();

			s_Data->GridVB = VertexBuffer::Create();
			s_Data->GridVB->SetLayout({
				{ ShaderDataType::Float2, "a_Position" },
				{ ShaderDataType::Float2, "a_UV" }
			});
			static constexpr float gridVertices[] = {
				-1.0f, -1.0f, 0.0f, 0.0f,
				 1.0f, -1.0f, 1.0f, 0.0f,
				 1.0f,  1.0f, 1.0f, 1.0f,
				-1.0f,  1.0f, 0.0f, 1.0f
			};
			s_Data->GridVB->BufferData(gridVertices, sizeof(gridVertices));
			s_Data->GridVA->AddVertexBuffer(s_Data->GridVB);

			s_Data->GridIB = IndexBuffer::Create();
			s_Data->GridIB->BufferData(quadIndices, OE_ARRAY_SIZE(quadIndices));
			s_Data->GridVA->SetIndexBuffer(s_Data->GridIB);
		}
	}

	void ViewportPanel::OnImGuiRender()
	{
		ImGui::Begin("Viewport Settings");
		{
			ImGui::DragFloat("u_GridZoom", &gridZoom);
			ImGui::DragFloat("u_LineKernel", &gridKernel);

			if (ImGui::Button("Reload Grid Shader"))
				s_Data->GridShader->Reload();
		}
		ImGui::End();

		if (m_Context->Context)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_MenuBar);

		if (m_Context->Context)
		{
			ImGui::PopStyleVar();

			ImGui::BeginMenuBar();
			if (ImGui::Button(m_Context->Simulating ? "Stop" : "Start"))
			{
				OE_BOOL_SWAP(m_Context->Simulating);

				if (m_Context->Simulating)
				{
					m_Context->SimulationScene = new Scene(*m_Context->Context);
					m_Context->SimulationScene->InitializePhysics();

					for (auto& selection : m_Context->SelectionContext)
					{
						selection = { selection.GetRuntimeID(), m_Context->SimulationScene };
					}
				}
				else if (m_Context->SimulationScene)
				{
					delete m_Context->SimulationScene;
					m_Context->SimulationScene = nullptr;

					for (auto& selection : m_Context->SelectionContext)
					{
						selection = { selection.GetRuntimeID(), m_Context->Context.get() };
					}
				}
			}

			if (ImGui::Button(m_Context->SimulationRunning ? "Pause" : "Resume"))
			{
				OE_BOOL_SWAP(m_Context->SimulationRunning);
			}
			ImGui::EndMenuBar();

			// Resize
			ImVec2 panelSize = ImGui::GetContentRegionAvail();
			if (m_PanelSize.x != panelSize.x || m_PanelSize.y != panelSize.y)
			{
				m_PanelSize = IMVEC2_2_VECTOR2(panelSize);
				m_Camera.SetViewportSize((uint32_t)panelSize.x, (uint32_t)panelSize.y);
			}

			// Draw FrameBuffer
			ImGui::Image((void*)(intptr_t)m_FrameBuffer->GetColorAttachmentRendererID(), panelSize, { 0, 1 }, { 1, 0 });

			bool hovered = ImGui::IsWindowHovered();
			bool rightMouseButtonDown = ImGui::IsMouseDown(ImGuiMouseButton_Right);

			bool fDown = Input::IsKeyPressed(KeyCode::F);

			if (m_Context->SelectionContext.size() == 1)
			{
				auto& entityTransform = m_Context->SelectionContext[0].GetComponent<TransformComponent>();

				// Gizmo
				DrawGizmo(entityTransform, hovered);

				// Focus
				if (hovered && fDown && !m_FDownLastFrame)
					m_CameraTransform.SetPosition(entityTransform.GetPosition());
			}

			m_FDownLastFrame = fDown;

			// Panning
			if (!m_Panning && hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				m_Panning = true;
			}
			else if (m_Panning && !(hovered && rightMouseButtonDown))
			{
				m_Panning = false;
				m_LastMouseDelta = { 0.0f, 0.0f };
			}

			if (m_Panning)
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
				Vector2 mouseDelta = IMVEC2_2_VECTOR2(ImGui::GetMouseDragDelta(ImGuiMouseButton_Right));
				if (mouseDelta != m_LastMouseDelta)
				{
					Vector3 cameraPosition = m_CameraTransform.GetPosition();
					float multiplier = m_Camera.GetOrthographicSize() / m_PanelSize.y;
					cameraPosition.x += (m_LastMouseDelta.x - mouseDelta.x) * multiplier;
					cameraPosition.y += (mouseDelta.y - m_LastMouseDelta.y) * multiplier;
					m_CameraTransform.SetPosition(cameraPosition);

					m_LastMouseDelta = mouseDelta;
				}
			}

			// Zooming
			if (hovered)
			{
				float mouseWheel = ImGui::GetIO().MouseWheel;
				m_Camera.SetOrthographicSize(Math::Clamp(m_Camera.GetOrthographicSize() + -mouseWheel / 3.0f, 0.01f, FLT_MAX));
			}
		}
		else if (m_Context->Context)
		{
			ImGui::TextUnformatted("No camera is rendering");
		}
		else
		{
			ImGui::TextUnformatted("Open a Scene to start editing");
		}

		ImGui::End();
	}

	void ViewportPanel::OnRender()
	{
		if (m_Context->Context)
		{
			if (FrameBufferProps props = m_FrameBuffer->GetProps();
				m_PanelSize.x > 0.0f && m_PanelSize.y > 0.0f && // zero sized framebuffer is invalid
				(props.Width != m_PanelSize.x || props.Height != m_PanelSize.y))
			{
				m_FrameBuffer->Resize((uint32_t)m_PanelSize.x, (uint32_t)m_PanelSize.y);
			}
		}

		m_FrameBuffer->Bind();

		if (m_Context->Context)
		{
			Scene* scene = m_Context->GetActiveScene();

			if (m_Context->SimulationRunning)
				scene->OnPhysicsUpdate(Time::GetDeltaTime());

			RenderCommand::SetClearColor(m_Camera.GetClearColor());
			RenderCommand::Clear();
			DrawGrid();
			RenderCommand::Clear(ClearFlags_ClearDepth);

			Renderer2D::BeginScene(glm::inverse(m_CameraTransform.GetMatrix()), m_Camera);
			scene->RenderSprites();
			Renderer2D::EndScene();
		}
		else
		{
			RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
			RenderCommand::Clear();
		}

		m_FrameBuffer->Unbind();
	}

	void ViewportPanel::SetContext(const Ref<SceneEditor> context)
	{
		m_Context = context;
	}

	float ViewportPanel::ClosestDistanceBetweenLines(ViewportRay& l1, ViewportRay& l2)
	{
		const Vector3 dp = l2.Origin - l1.Origin;
		const float v12 = dot(l1.Direction, l1.Direction);
		const float v22 = dot(l2.Direction, l2.Direction);
		const float v1v2 = dot(l1.Direction, l2.Direction);

		const float det = v12 * v22 - v1v2 * v1v2;

		if (std::abs(det) > FLT_MIN)
		{
			const float inv_det = 1.f / det;

			const float dpv1 = dot(dp, l1.Direction);
			const float dpv2 = dot(-dp, l2.Direction);

			l1.T = inv_det * (v22 * dpv1 - v1v2 * dpv2);
			l2.T = -inv_det * (v1v2 * dpv1 - v12 * dpv2);

			return glm::fastLength(dp + l2.Direction * l2.T - l1.Direction * l1.T);
		}
		else
		{
			const Vector3 a = cross(dp, l1.Direction);
			return std::sqrt(dot(a, a) / v12);
		}
	}

	void ViewportPanel::DrawGizmo(TransformComponent& entityTransform, bool hovered)
	{
		float arrowScale = 0.25f;
		const auto& cameraOrthoSize = m_Camera.GetOrthographicSize();

		// Render Gizmo
		if (m_Tool == ViewportTool::Translate2D)
		{
			m_FrameBuffer->Bind();
			RenderCommand::Clear(ClearFlags_ClearDepth);

			s_Data->GizmoShader->Bind();
			s_Data->GizmoVA->Bind();

			Transform gizmoTransform;
			gizmoTransform.SetScale({ arrowScale * cameraOrthoSize, arrowScale / 10 * cameraOrthoSize, 1.0f * cameraOrthoSize });
			float offset = gizmoTransform.GetScale().x / 2.0f - gizmoTransform.GetScale().y / 10.0f;

			static auto highlightAxis = [this](Axis axis) -> bool
			{
				return m_HoveredTranslateAxis == axis || m_ActiveTranslateAxis == axis;
			};

			gizmoTransform.SetPosition({ entityTransform.GetPosition().x + offset, entityTransform.GetPosition().y, 0.0f });
			s_Data->GizmoShader->UploadUniformMat4("u_ViewProjMatrix", m_Camera.GetProjection() * glm::inverse((Mat4x4)m_CameraTransform));
			s_Data->GizmoShader->UploadUniformMat4("u_Transform", gizmoTransform);
			s_Data->GizmoShader->UploadUniformFloat4("u_Color", { 1.0, 0.0, 0.0, highlightAxis(Axis::X) ? 1.0 : 0.7 });
			RenderCommand::DrawIndexed(s_Data->GizmoVA);

			gizmoTransform.SetPosition({ entityTransform.GetPosition().x, entityTransform.GetPosition().y + offset, 0.0f });
			gizmoTransform.SetEulerAngles({ 0.0f, 0.0f, 90.0f });
			s_Data->GizmoShader->UploadUniformMat4("u_ViewProjMatrix", m_Camera.GetProjection() * glm::inverse((Mat4x4)m_CameraTransform));
			s_Data->GizmoShader->UploadUniformMat4("u_Transform", gizmoTransform);
			s_Data->GizmoShader->UploadUniformFloat4("u_Color", { 0.0, 1.0, 0.0, highlightAxis(Axis::Y) ? 1.0 : 0.7 });
			RenderCommand::DrawIndexed(s_Data->GizmoVA);

			m_FrameBuffer->Unbind();

			ViewportRay xAxisRay, yAxisRay;

			if (hovered)
			{
				auto mousePos = ImGui::GetMousePos();
				auto winPos = ImGui::GetWindowPos();
				auto winSize = ImGui::GetWindowSize();

				Vector2 n(((mousePos.x - winPos.x) / winSize.x - 0.5f) * 2, -((mousePos.y - winPos.y) / winSize.y - 0.5f) * 2);

				Vector4 rayStart, rayEnd;
				Mat4x4 viewProjInverse = glm::inverse(m_Camera.GetProjection() * glm::inverse(m_CameraTransform.GetMatrix()));

				rayStart = viewProjInverse * Vector4(n.x, n.y, 0.f, 1.f);
				rayStart *= 1.f / rayStart.w;

				rayEnd = viewProjInverse * Vector4(n.x, n.y, 1.f, 1.f);
				rayEnd *= 1.f / rayEnd.w;

				ViewportRay cameraRay;
				cameraRay.Origin = (Vector3)rayStart;
				cameraRay.Direction = (Vector3)glm::fastNormalize(rayEnd - rayStart);
				cameraRay.T = FLT_MAX;

				xAxisRay = { entityTransform.GetPosition(), { 1.0f, 0.0, 0.0 }, FLT_MAX };
				float distanceToXHandle = ClosestDistanceBetweenLines(xAxisRay, cameraRay);

				yAxisRay = { entityTransform.GetPosition(), { 0.0f, 1.0, 0.0 }, FLT_MAX };
				float distanceToYHandle = ClosestDistanceBetweenLines(yAxisRay, cameraRay);

				if (m_ActiveTranslateAxis == Axis::None)
				{
					m_HoveredTranslateAxis = Axis::None;
					if (distanceToXHandle < distanceToYHandle)
					{
						if (distanceToXHandle <= arrowScale / 10.0f * cameraOrthoSize &&
							xAxisRay.T > 0.0f && xAxisRay.T < arrowScale * cameraOrthoSize)
							m_HoveredTranslateAxis = Axis::X;
					}
					else
					{
						if (distanceToYHandle <= arrowScale / 10.0f * cameraOrthoSize &&
							yAxisRay.T > 0.0f && yAxisRay.T < arrowScale * cameraOrthoSize)
							m_HoveredTranslateAxis = Axis::Y;
					}

					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && m_HoveredTranslateAxis != Axis::None)
					{
						m_ActiveTranslateAxis = m_HoveredTranslateAxis;

						if (m_ActiveTranslateAxis == Axis::X)
							m_Translate2DToolLastIntersect = xAxisRay.Origin + xAxisRay.Direction * xAxisRay.T;
						else if (m_ActiveTranslateAxis == Axis::Y)
							m_Translate2DToolLastIntersect = yAxisRay.Origin + yAxisRay.Direction * yAxisRay.T;
					}
				}
			}

			if (m_ActiveTranslateAxis != Axis::None && (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || !hovered))
			{
				m_ActiveTranslateAxis = Axis::None;
				m_HoveredTranslateAxis = Axis::None;
			}

			if (m_ActiveTranslateAxis != Axis::None)
			{
				Vector3 intersect;
				if (m_ActiveTranslateAxis == Axis::X)
					intersect = xAxisRay.Origin + xAxisRay.Direction * xAxisRay.T;
				else if (m_ActiveTranslateAxis == Axis::Y)
					intersect = yAxisRay.Origin + yAxisRay.Direction * yAxisRay.T;

				entityTransform.SetPosition(entityTransform.GetPosition() + intersect - m_Translate2DToolLastIntersect);
				m_Translate2DToolLastIntersect = intersect;
			}
		}
	}

	void ViewportPanel::DrawGrid()
	{
		s_Data->GridShader->Bind();
		const auto& cameraPos = m_CameraTransform.GetPosition();
		const auto& cameraOrthoSize = m_Camera.GetOrthographicSize();
		s_Data->GridShader->UploadUniformFloat2("u_CameraPos", { cameraPos.x, cameraPos.y });
		s_Data->GridShader->UploadUniformFloat2("u_WorldSpaceSize", Vector2{ m_PanelSize.x / m_PanelSize.y, 1 } * cameraOrthoSize / 2.0f);
		s_Data->GridShader->UploadUniformFloat("u_GridZoom", gridZoom);
		s_Data->GridShader->UploadUniformFloat("u_LineKernel", gridKernel);
		s_Data->GridVA->Bind();
		RenderCommand::DrawIndexed(s_Data->GridVA);
	}
}
