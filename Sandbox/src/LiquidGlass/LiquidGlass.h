#pragma once

#include <OverEngine.h>

using namespace OverEngine;

class LiquidGlass : public Layer
{
public:
	LiquidGlass();

	void OnUpdate(TimeStep deltaTime) override;
	void OnImGuiRender() override;
	void OnEvent(Event& event) override;

private:
    SceneCamera m_Camera;
    SceneCamera m_ScreenCamera;
    Vector3 m_Position = { 0, 0, 0 };
    Vector3 m_CameraPosition = { 0, 0, 0 };
};
