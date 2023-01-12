import "components" for NameComponent, TransformComponent

/// A single entity in the scene.
foreign class Entity {
	/// The name of the entity.
	/// Under the hood, its stored in NameComponent.
	name { getComponent(NameComponent).name }
	toString { "Entity(\"%(name)\")" }

	transform { getComponent(TransformComponent) }

	hasComponent(type) { type.has(this) }
	getComponent(type) {
		if (hasComponent(type)) {
			return type.new(this)
		}

		Fiber.abort("Entity '%(name)' doesn't have %(type)")
	}
}
