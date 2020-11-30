#include "pcheader.h"
#include "ScriptingEngine.h"

#include "OverEngine/Input/Input.h"
#include "OverEngine/Scene/Entity.h"
#include "OverEngine/Scene/Components.h"

namespace OverEngine
{
	void ScriptingEngine::LoadApi(sol::state& state)
	{
		state.open_libraries();

		state.new_usertype<Vector2>("Vector2",
			"new", sol::constructors<Vector2(), Vector2(float), Vector2(float, float)>(),
			"x", &Vector2::x,
			"y", &Vector2::y
		);

		state.new_usertype<Vector3>("Vector3",
			"new", sol::constructors<Vector2(), Vector2(float), Vector2(float, float, float)>(),
			"x", &Vector3::x,
			"y", &Vector3::y,
			"z", &Vector3::z
		);

		state.new_usertype<Vector4>("Vector4",
			"new", sol::constructors<Vector2(), Vector2(float), Vector2(float, float, float, float)>(),
			"x", &Vector4::x, "r", &Vector4::r,
			"y", &Vector4::y, "g", &Vector4::g,
			"z", &Vector4::z, "b", &Vector4::b,
			"w", &Vector4::w, "a", &Vector4::a
		);

		state.new_usertype<Input>("Input",
			"IsKeyPressed"    , &Input::IsKeyPressed,
			"GetMousePosition", &Input::GetMousePosition,
			"GetMouseX"       , &Input::GetMouseX,
			"GetMouseY"       , &Input::GetMouseY
		);

		state.new_enum<KeyCode>("KeyCode", {
			{ "None",              KeyCode::None },
			{ "Space",             KeyCode::Space },
			{ "Apostrophe",        KeyCode::Apostrophe },
			{ "Comma",             KeyCode::Comma },
			{ "Minus",             KeyCode::Minus },
			{ "Period",            KeyCode::Period },
			{ "Slash",             KeyCode::Slash },
			{ "Alpha0",            KeyCode::Alpha0 },
			{ "Alpha1",            KeyCode::Alpha1 },
			{ "Alpha2",            KeyCode::Alpha2 },
			{ "Alpha3",            KeyCode::Alpha3 },
			{ "Alpha4",            KeyCode::Alpha4 },
			{ "Alpha5",            KeyCode::Alpha5 },
			{ "Alpha6",            KeyCode::Alpha6 },
			{ "Alpha7",            KeyCode::Alpha7 },
			{ "Alpha8",            KeyCode::Alpha8 },
			{ "Alpha9",            KeyCode::Alpha9 },
			{ "Semicolon",         KeyCode::Semicolon },
			{ "Equal",             KeyCode::Equal },
			{ "A",                 KeyCode::A },
			{ "B",                 KeyCode::B },
			{ "C",                 KeyCode::C },
			{ "D",                 KeyCode::D },
			{ "E",                 KeyCode::E },
			{ "F",                 KeyCode::F },
			{ "G",                 KeyCode::G },
			{ "H",                 KeyCode::H },
			{ "I",                 KeyCode::I },
			{ "J",                 KeyCode::J },
			{ "K",                 KeyCode::K },
			{ "L",                 KeyCode::L },
			{ "M",                 KeyCode::M },
			{ "N",                 KeyCode::N },
			{ "O",                 KeyCode::O },
			{ "P",                 KeyCode::P },
			{ "Q",                 KeyCode::Q },
			{ "R",                 KeyCode::R },
			{ "S",                 KeyCode::S },
			{ "T",                 KeyCode::T },
			{ "U",                 KeyCode::U },
			{ "V",                 KeyCode::V },
			{ "W",                 KeyCode::W },
			{ "X",                 KeyCode::X },
			{ "Y",                 KeyCode::Y },
			{ "Z",                 KeyCode::Z },
			{ "LeftBracket",       KeyCode::LeftBracket },
			{ "BackSlash",         KeyCode::BackSlash },
			{ "RightBracket",      KeyCode::RightBracket },
			{ "GraveAccent",       KeyCode::GraveAccent },
			{ "World1",            KeyCode::World1 },
			{ "World2",            KeyCode::World2 },
			{ "Escape",            KeyCode::Escape },
			{ "Enter",             KeyCode::Enter },
			{ "Tab",               KeyCode::Tab },
			{ "Backspace",         KeyCode::Backspace },
			{ "Insert",            KeyCode::Insert },
			{ "Delete",            KeyCode::Delete },
			{ "Right",             KeyCode::Right },
			{ "Left",              KeyCode::Left },
			{ "Down",              KeyCode::Down },
			{ "Up",                KeyCode::Up },
			{ "PageUp",            KeyCode::PageUp },
			{ "PageDown",          KeyCode::PageDown },
			{ "Home",              KeyCode::Home },
			{ "End",               KeyCode::End },
			{ "CapsLock",          KeyCode::CapsLock },
			{ "ScrollLock",        KeyCode::ScrollLock },
			{ "NumLock",           KeyCode::NumLock },
			{ "PrintScreen",       KeyCode::PrintScreen },
			{ "Pause",             KeyCode::Pause },
			{ "F1",                KeyCode::F1 },
			{ "F2",                KeyCode::F2 },
			{ "F3",                KeyCode::F3 },
			{ "F4",                KeyCode::F4 },
			{ "F5",                KeyCode::F5 },
			{ "F6",                KeyCode::F6 },
			{ "F7",                KeyCode::F7 },
			{ "F8",                KeyCode::F8 },
			{ "F9",                KeyCode::F9 },
			{ "F10",               KeyCode::F10 },
			{ "F11",               KeyCode::F11 },
			{ "F12",               KeyCode::F12 },
			{ "F13",               KeyCode::F13 },
			{ "F14",               KeyCode::F14 },
			{ "F15",               KeyCode::F15 },
			{ "F16",               KeyCode::F16 },
			{ "F17",               KeyCode::F17 },
			{ "F18",               KeyCode::F18 },
			{ "F19",               KeyCode::F19 },
			{ "F20",               KeyCode::F20 },
			{ "F21",               KeyCode::F21 },
			{ "F22",               KeyCode::F22 },
			{ "F23",               KeyCode::F23 },
			{ "F24",               KeyCode::F24 },
			{ "F25",               KeyCode::F25 },
			{ "KeyPad0",           KeyCode::KeyPad0 },
			{ "KeyPad1",           KeyCode::KeyPad1 },
			{ "KeyPad2",           KeyCode::KeyPad2 },
			{ "KeyPad3",           KeyCode::KeyPad3 },
			{ "KeyPad4",           KeyCode::KeyPad4 },
			{ "KeyPad5",           KeyCode::KeyPad5 },
			{ "KeyPad6",           KeyCode::KeyPad6 },
			{ "KeyPad7",           KeyCode::KeyPad7 },
			{ "KeyPad8",           KeyCode::KeyPad8 },
			{ "KeyPad9",           KeyCode::KeyPad9 },
			{ "KeyPadDecimal",     KeyCode::KeyPadDecimal },
			{ "KeyPadDivide",      KeyCode::KeyPadDivide },
			{ "KeyPadMultiply",    KeyCode::KeyPadMultiply },
			{ "KeyPadSubtract",    KeyCode::KeyPadSubtract },
			{ "KeyPadAdd",         KeyCode::KeyPadAdd },
			{ "KeyPadEnter",       KeyCode::KeyPadEnter },
			{ "KeyPadEqual",       KeyCode::KeyPadEqual },
			{ "LeftShift",         KeyCode::LeftShift },
			{ "LeftControl",       KeyCode::LeftControl },
			{ "LeftAlt",           KeyCode::LeftAlt },
			{ "LeftSuper",         KeyCode::LeftSuper },
			{ "RightShift",        KeyCode::RightShift },
			{ "RightControl",      KeyCode::RightControl },
			{ "RightAlt",          KeyCode::RightAlt },
			{ "RightSuper",        KeyCode::RightSuper },
			{ "Menu",              KeyCode::Menu },
			{ "MouseButton1",      KeyCode::MouseButton1 },
			{ "MouseButton2",      KeyCode::MouseButton2 },
			{ "MouseButton3",      KeyCode::MouseButton3 },
			{ "MouseButton4",      KeyCode::MouseButton4 },
			{ "MouseButton5",      KeyCode::MouseButton5 },
			{ "MouseButton6",      KeyCode::MouseButton6 },
			{ "MouseButton7",      KeyCode::MouseButton7 },
			{ "MouseButton8",      KeyCode::MouseButton8 },
			{ "MouseButtonLast",   KeyCode::MouseButtonLast },
			{ "MouseButtonLeft",   KeyCode::MouseButtonLeft },
			{ "MouseButtonRight",  KeyCode::MouseButtonRight },
			{ "MouseButtonMiddle", KeyCode::MouseButtonMiddle },
		});

		state.new_usertype<RigidBody2D>("RigidBody2D",
			"ApplyLinearImpulseToCenter", &RigidBody2D::ApplyLinearImpulseToCenter,
			"ApplyLinearImpulse", &RigidBody2D::ApplyLinearImpulse
		);

		state.new_usertype<RigidBody2DComponent>("RigidBody2DComponent",
			"RigidBody", &RigidBody2DComponent::RigidBody
		);

		state.new_usertype<Entity>("Entity",
			"GetRigidBody2DComponent", &Entity::GetComponent<RigidBody2DComponent>
		);
	}
}
