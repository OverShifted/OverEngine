class ComponentInternals {
	foreign static NameComponent_name(entity)
	foreign static NameComponent_set_name(entity, name)

	foreign static TransformComponent_position(entity)
	foreign static TransformComponent_set_position(entity, position)

	foreign static RigidBody2DComponent_applyLinearImpulseToCenter(entity, impulse)
}

class Component {
	construct new(entity) { _entity = entity }
	entity { _entity }
}

class NameComponent is Component {
	// Private to engine
	foreign static has(entity)
	construct new(entity) { super(entity) }

	name { ComponentInternals.NameComponent_name(entity) }
	name=(rhs) { ComponentInternals.NameComponent_set_name(entity, rhs) }
}

class TransformComponent is Component {
	// Private to engine
	foreign static has(entity)
	construct new(entity) { super(entity) }

	position { ComponentInternals.TransformComponent_position(entity) }
	position=(rhs) { ComponentInternals.TransformComponent_set_position(entity, rhs) }
}

class RigidBody2DComponent is Component {
	// Private to engine
	foreign static has(entity)
	construct new(entity) { super(entity) }

	applyLinearImpulseToCenter(impulse) { ComponentInternals.RigidBody2DComponent_applyLinearImpulseToCenter(entity, impulse) }
}
