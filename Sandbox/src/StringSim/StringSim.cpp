#include "StringSim.h"

#include <imgui.h>

#include <algorithm>

using namespace OverEngine;

static float g = 8.0f;
static float lastDeltaTime = 1.0f;
static bool mouseDownLastFrame = false;
static Vector2 mousePosLastFrame = Vector2(0.0f);
int numIters = 10;

StringSim::StringSim()
	: Layer("StringSim")
{
    m_Camera.SetOrthographic(15.0f, -10.0f, 10.0f);
    m_Camera.SetViewportSize(1600, 900);
//    Application::Get().GetWindow().SetVSync(false);

    // float x = 3.0f;

    // for (uint32_t i = 0; i < 10; i++)
    // {
    //     m_Nodes.push_back(Node { i == 0 , { x, 1.0 }, { x, 1.0 } });

    //     x -= 0.5;
    // }

    // for (uint32_t i = 0; i < 9; i++)
    // {
    //     m_Links.push_back(Link { i, i + 1, 0.5f });
    // }

    for (uint32_t x = 0; x < 20; x++) {
        for (uint32_t y = 0; y < 10; y++) {
            // if (y > 0) m_Links.push_back(Link { 10 * y + x - 1, 10 * y + x, 0.5f });
            if (y < 9) m_Links.push_back(Link { 
                x * 10 + y,
                x * 10 + y + 1,
                0.5
            });

            if (x < 19) m_Links.push_back(Link { 
                x * 10 + y,
                (x + 1) * 10 + y,
                0.5
             });
            Vector2 p = { (x * 0.5) - 5, (y * 0.5) - 2 };
            m_Nodes.push_back(Node { y == 9 , p, p });
        }
    }

    std::shuffle(std::begin(m_Links), std::end(m_Links), Random::GetMt19937());
}

float angleBetween(
    glm::vec2 a,
    glm::vec2 b
) {
    glm::vec2 da = glm::normalize(a);
    glm::vec2 db = glm::normalize(b);
    return glm::acos(glm::dot(da, db));
}

float crossVec2(Vector2 a, Vector2 b) {
    return a.x * b.y - a.y * b.x;
}

bool linesCollide(
    Vector2 a0, Vector2 a1,
    Vector2 b0, Vector2 b1
) {
    Vector2 delta_a = a1 - a0;
    Vector2 delta_b = b1 - b0;

    float t = crossVec2(a0 - b0, delta_a) / crossVec2(delta_b, delta_a);
    float u = crossVec2(a0 - b0, delta_b) / crossVec2(delta_b, delta_a);

    return t >= 0 && 1 >= t && u >= 0 && 1 >= u;
}

void StringSim::OnUpdate(TimeStep deltaTime)
{
    float deltaDelta = deltaTime.GetSeconds() / lastDeltaTime;
    if (!isinf(deltaDelta)) {
        for (Node& node : m_Nodes) {
            if (!node.Locked) {
                Vector2 oldPos = node.P1;
                node.P1 += (node.P1 - node.P0) * deltaTime.GetSeconds() / lastDeltaTime;
                node.P1.y -= g * deltaTime * deltaTime;
                node.P0 = oldPos;
            }
        }
    }

    for (uint32_t i = 0; i < numIters; i++) {
        for (Link& link : m_Links) {
            if (!link.Enabled) continue;
            Vector2 center = (m_Nodes[link.N0].P1 + m_Nodes[link.N1].P1) * 0.5f;
            Vector2 dir = glm::normalize(m_Nodes[link.N0].P1 - m_Nodes[link.N1].P1);
            if (!m_Nodes[link.N0].Locked) m_Nodes[link.N0].P1 = center + dir * link.Length * 0.5f;
            if (!m_Nodes[link.N1].Locked) m_Nodes[link.N1].P1 = center - dir * link.Length * 0.5f;
        }
    }

    RenderCommand::SetClearColor(Color{ 0.1, 0.1, 0.1, 1.0 });
    RenderCommand::Clear(ClearFlags_ClearColor | ClearFlags_ClearDepth);

    Renderer2D::BeginScene(IDENTITY_MAT4X4, m_Camera);

    // float xp = Input::GetMouseX() / Application::Get().GetWindow().GetWidth()  * 2 - 1.0;
    // float yp = -(Input::GetMouseY() / Application::Get().GetWindow().GetHeight() * 2 - 1.0);

    auto m = Vector2(
        Input::GetMouseX() / Application::Get().GetWindow().GetWidth()  * 2 - 1.0,
        -(Input::GetMouseY() / Application::Get().GetWindow().GetHeight() * 2 - 1.0)
    );

    float xp = m.x;
    float yp = m.y;

    for (Link& link : m_Links) {
        if (!link.Enabled) continue;

        Vector3 center = Vector3((m_Nodes[link.N0].P1 + m_Nodes[link.N1].P1) * 0.5f, -1.0);
        Vector2 diff = m_Nodes[link.N0].P1 - m_Nodes[link.N1].P1;
        float diffYSign = diff.y >= 0 ? 1.0 : -1.0;

        Mat4x4 transform = 
            glm::translate(Mat4x4(1.0f), center) *
            glm::rotate(Mat4x4(1.0f), angleBetween(diffYSign * diff, Vector2 { 1.0f, 0.0f }), Vector3(0, 0, 1)) *
			glm::scale(Mat4x4(1.0f), Vector3(glm::length(diff), 0.05, 1.0f));

        Mat4x4 mat = m_Camera.GetProjection() * IDENTITY_MAT4X4 * transform;

        auto a = Vector2(Vector3(mat * Vector4(-0.5, -0.5, 0.0, 1.0)));
		auto b = Vector2(Vector3(mat * Vector4( 0.5, -0.5, 0.0, 1.0)));
		auto c = Vector2(Vector3(mat * Vector4( 0.5,  0.5, 0.0, 1.0)));
		auto d = Vector2(Vector3(mat * Vector4(-0.5,  0.5, 0.0, 1.0)));

        if (
            Input::IsMouseButtonPressed(KeyCode::MouseButtonLeft) &&
            0 <= glm::dot(b - a, m - a) && glm::dot(b - a, m - a) <= glm::dot(b - a, b - a) &&
            0 <= glm::dot(c - b, m - b) && glm::dot(c - b, m - b) <= glm::dot(c - b, c - b)
        ) {
            link.Enabled = false;
            continue;
        }

        if (Input::IsMouseButtonPressed(KeyCode::MouseButtonLeft) && mouseDownLastFrame) {
//            Vector2 dir = m - mousePosLastFrame;

//            uint32_t iters = 100;
//            dir /= float(iters);
//            for (uint32_t i = 0; i < iters; i++) {
//                Vector2 mp = mousePosLastFrame + dir * float(i);
//                if (
//                    0 <= glm::dot(b - a, mp - a) && glm::dot(b - a, mp - a) <= glm::dot(b - a, b - a) &&
//                    0 <= glm::dot(c - b, mp - b) && glm::dot(c - b, mp - b) <= glm::dot(c - b, c - b)
//                ) {
//                    link.Enabled = false;
//                }
//            }

            if (
                linesCollide(m, mousePosLastFrame, a, b) ||
                linesCollide(m, mousePosLastFrame, b, c) ||
                linesCollide(m, mousePosLastFrame, c, d) ||
                linesCollide(m, mousePosLastFrame, d, a)
            )
                link.Enabled = false;
        }
        
        Renderer2D::DrawQuad(transform, Color{ 0.6 });
    }

    for (Node& node : m_Nodes) {
        Renderer2D::DrawQuad(node.P1, 0.0, Vector2{ 1.0, 1.0 } * 0.1f, 
            node.Locked ? Color{ 1.0, 0.2, 0.2, 1.0 } : Color{ 1.0 });
    }

    Renderer2D::EndScene();

    mouseDownLastFrame = Input::IsMouseButtonPressed(KeyCode::MouseButtonLeft);
    mousePosLastFrame = m;
    lastDeltaTime = deltaTime;
}

void StringSim::OnImGuiRender()
{
    ImGui::DragFloat("Gravity", &g);
    ImGui::DragInt("Iters", &numIters, 0.1, 1, 1000);
    ImGui::Text("FPS: %f", 1.0 / lastDeltaTime);
}

void StringSim::OnEvent(Event& event)
{
	OE_PROFILE_FUNCTION();

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) {
	    this->m_Camera.SetViewportSize(e.GetWidth(), e.GetHeight());
        return false;
    });

	dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& e) {
		m_Camera.SetOrthographicSize(m_Camera.GetOrthographicSize() - e.GetYOffset());
		return false;
    });
}
