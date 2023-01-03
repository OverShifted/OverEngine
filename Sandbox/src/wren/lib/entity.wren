class EntityInternals {
	foreign static getName(entity)
	foreign static getPosition(entity)
	foreign static setPosition(entity, position)
}

foreign class Entity {
	name { EntityInternals.getName(this) }

	position { EntityInternals.getPosition(this) }
	position=(rhs) { EntityInternals.setPosition(this, rhs) }
}
