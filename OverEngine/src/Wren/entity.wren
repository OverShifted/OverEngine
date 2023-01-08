#nodoc
class EntityInternals {
	foreign static getName(entity)
	foreign static getPosition(entity)
	foreign static setPosition(entity, position)
	foreign static applyLinearImpulseToCenter(entity, impulse)
}

/// A single entity in the scene.
foreign class Entity {
	/// The name of the entity.
	/// Under the hood, its stored in NameComponent.
	name { EntityInternals.getName(this) }

	/// Position of the entity in spatial space.
	position { EntityInternals.getPosition(this) }
	position=(rhs) { EntityInternals.setPosition(this, rhs) }

	/// Will be removed :)
	applyLinearImpulseToCenter(impulse) { EntityInternals.applyLinearImpulseToCenter(this, impulse) }
}
