#include "pcheader.h"
#include "Wren.h"

#include <wren.h>

#include "OverEngine/Input/Input.h"
#include "OverEngine/Scene/Entity.h"
#include "OverEngine/Scene/Components.h"
#include "OverEngine/Scene/TransformComponent.h"

#define WRENPP_BIND_STATIC(f, sig) .bindFunction<decltype(&f), &f>(true, sig)
#define WRENPP_BIND_GETTER(f, sig) .bindGetter<decltype(f), &f>(sig)
#define WRENPP_BIND_SETTER(f, sig) .bindSetter<decltype(f), &f>(sig)

namespace OverEngine
{
	namespace WrenBindings
	{
		bool Input_isKeyPressed(double keycode)
		{
			return Input::IsKeyPressed((KeyCode)keycode);
		}

		String EntityInternals_getName(Entity& entity)
		{
			return entity.GetComponent<NameComponent>().Name;
		}

		Vector3 EntityInternals_getPosition(Entity& entity)
		{
			return entity.GetComponent<TransformComponent>().GetPosition();
		}

		void EntityInternals_setPosition(Entity& entity, const Vector3& position)
		{
			entity.GetComponent<TransformComponent>().SetPosition(position);
		}

		// TODO: Move into component classes (not all entities have rigidbodies)
		// TODO: Use Vector2
		void EntityInternals_applyLinearImpulseToCenter(Entity& entity, const Vector3& impulse)
		{
			entity.GetComponent<RigidBody2DComponent>().RigidBody->ApplyLinearImpulseToCenter(impulse);
		}
	}

	void Wren::InitializeBindings()
	{
		using namespace WrenBindings;

		m_VM.beginModule("input")
			.beginClass("Input")
				WRENPP_BIND_STATIC(Input_isKeyPressed, "isKeyPressed(_)")
			.endClass()
		.endModule();

		m_VM.beginModule("entity")
			.beginClass("EntityInternals")
				WRENPP_BIND_STATIC(EntityInternals_getName, "getName(_)")
				WRENPP_BIND_STATIC(EntityInternals_getPosition, "getPosition(_)")
				WRENPP_BIND_STATIC(EntityInternals_setPosition, "setPosition(_,_)")

				WRENPP_BIND_STATIC(EntityInternals_applyLinearImpulseToCenter, "applyLinearImpulseToCenter(_,_)")
			.endClass()

			.bindClass<Entity>("Entity")
			.endClass()
		.endModule();

		m_VM.beginModule("math")
			.bindClass<Vector3, float, float, float>("Vector3")
				WRENPP_BIND_GETTER(Vector3::x, "x")
				WRENPP_BIND_GETTER(Vector3::y, "y")
				WRENPP_BIND_GETTER(Vector3::z, "z")

				WRENPP_BIND_SETTER(Vector3::x, "x=(_)")
				WRENPP_BIND_SETTER(Vector3::y, "y=(_)")
				WRENPP_BIND_SETTER(Vector3::z, "z=(_)")
			.endClass()
		.endModule();

		LoadModule("lib");
	}
}

