class ComponentInternals {
	foreign static RigidBody2DComponent_applyLinearImpulseToCenter(entity, impulse)
}

class TransformComponent {
	foreign static has(entity)

	construct new(entity) { _entity = entity }
}

class RigidBody2DComponent {
	foreign static has(entity)

	construct new(entity) { _entity = entity }

	applyLinearImpulseToCenter(impulse) { ComponentInternals.RigidBody2DComponent_applyLinearImpulseToCenter(_entity, impulse) }
}
