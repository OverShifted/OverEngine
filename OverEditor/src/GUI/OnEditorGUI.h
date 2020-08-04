#pragma once

#include <OverEngine.h>

using namespace OverEngine;

void OnMainMenubarGUI();
void OnMainDockSpaceGUI();

void OnToolbarGUI();
Vector2 OnViewportGUI(Ref<FrameBuffer> frameBuffer, Camera* camera);
void OnSceneGraphGUI(Ref<Scene> scene);
void OnAssetsGUI();
void OnConsoleGUI();