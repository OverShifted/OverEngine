#nodoc
class EntityInternals {
	foreign static getName(entity)
	foreign static getPosition(entity)
	foreign static setPosition(entity, position)
}

/// A single entity in the scene.
foreign class Entity {
	/// The name of the entity.
	/// Under the hood, its stored in NameComponent.
	name { EntityInternals.getName(this) }

	/// Position of the entity in spatial space.
	position { EntityInternals.getPosition(this) }
	position=(rhs) { EntityInternals.setPosition(this, rhs) }

	hasComponent(type) { type.has(this) }
	getComponent(type) {
		if (hasComponent(type)) {
			return type.new(this)
		}

		Fiber.abort("Entity '%(name)' doesn't have %(type)")
	}
}
