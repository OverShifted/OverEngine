#pragma once

#include "pcheader.h"

// To be used in OverApps
#include "OverEngine/Core/Application.h"
#include "OverEngine/Core/Log.h"
#include "OverEngine/Core/Time/Time.h"
#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Layers/Layer.h"

#include "OverEngine/ImGui/ImGuiLayer.h"

// ------- Renderer ------------------
#include "OverEngine/Renderer/Renderer.h"

#include "OverEngine/Renderer/Buffer.h"
#include "OverEngine/Renderer/Shader.h"
#include "OverEngine/Renderer/VertexArray.h"
#include "OverEngine/Renderer/Camera.h"
// -----------------------------------

// ------- Input ---------------------
#include "OverEngine/Input/Input.h"
#include "OverEngine/Input/InputSystem.h"
// -----------------------------------

// ------- Entry Point ---------------
#ifdef OE_CLIENT_INCLUDE_ENTRY_POINT
#include "OverEngine/Core/EntryPoint.h"
#endif
//------------------------------------