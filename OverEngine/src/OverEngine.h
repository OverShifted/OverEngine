#pragma once

// To be used in OverPlayer
#include "OverEngine/Core/Application.h"
#include "OverEngine/Core/Log.h"
#include "OverEngine/Layers/Layer.h"

#include "OverEngine/ImGui/ImGuiLayer.h"

// ------- Renderer ------------------
#include "OverEngine/Renderer/Renderer.h"

#include "OverEngine/Renderer/Buffer.h"
#include "OverEngine/Renderer/Shader.h"
#include "OverEngine/Renderer/VertexArray.h"
// -----------------------------------

// ------- Input ---------------------
#include "OverEngine/Input/Input.h"
#include "OverEngine/Input/InputSystem.h"
// -----------------------------------

#ifdef OE_CLIENT_INCLUDE_ENTRY_POINT
// ------- Entry Point ---------------
#include "OverEngine/Core/EntryPoint.h"
//------------------------------------
#endif