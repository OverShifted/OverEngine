#pragma once

#include <OverEngine.h>

using namespace OverEngine;

struct Node {
    bool Locked;
    Vector2 P0, P1;
};

struct Link {
    uint32_t N0, N1;
    float Length;
    bool Enabled = true;
};

class StringSim : public Layer
{
public:
	StringSim();

	void OnUpdate(TimeStep deltaTime) override;
	void OnImGuiRender() override;
	void OnEvent(Event& event) override;

private:
    SceneCamera m_Camera;

    Vector<Node> m_Nodes;
    Vector<Link> m_Links;
};
