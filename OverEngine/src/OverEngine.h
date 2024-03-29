#pragma once

#include "OverEngine/Core/Runtime/Application.h"
#include "OverEngine/Core/Log.h"
#include "OverEngine/Core/Time/Time.h"
#include "OverEngine/Core/Math/Math.h"
#include "OverEngine/Core/Random.h" 
#include "OverEngine/Layers/Layer.h"
#include "OverEngine/Debug/Instrumentor.h"

#include "OverEngine/ImGui/ImGuiLayer.h"
#include "OverEngine/ImGui/ExtraImGui.h"

// ------- ECS -----------------------
#include "OverEngine/Scene/Scene.h"
#include "OverEngine/Scene/Entity.h"
#include "OverEngine/Scene/ScriptableEntity.h"
#include "OverEngine/Scene/Components.h"
#include "OverEngine/Scene/TransformComponent.h"
// -----------------------------------

// ------- Renderer ------------------
#include "OverEngine/Renderer/Renderer.h"
#include "OverEngine/Renderer/Renderer2D.h"
#include "OverEngine/Renderer/ParticleSystem2D.h"

#include "OverEngine/Renderer/VertexArray.h"
#include "OverEngine/Renderer/Buffer.h"
#include "OverEngine/Renderer/Shader.h"
#include "OverEngine/Renderer/Texture.h"
#include "OverEngine/Renderer/FrameBuffer.h"
#include "OverEngine/Renderer/Camera.h"
// -----------------------------------

// ------- Physics -------------------
#include "OverEngine/Physics/PhysicsWorld2D.h"
#include "OverEngine/Physics/RigidBody2D.h"
#include "OverEngine/Physics/Collider2D.h"
// -----------------------------------

// ------- Input ---------------------
#include "OverEngine/Input/Input.h"
// -----------------------------------

// ------- Entry Point ---------------
#ifdef OE_CLIENT_INCLUDE_ENTRY_POINT
	#include "OverEngine/Core/Runtime/EntryPoint.h"
#endif
//------------------------------------
