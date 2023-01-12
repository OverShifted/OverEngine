import "entity" for Entity

class Script {
	construct new(entity) {
		_entity = entity
	}

	entity { _entity }
	name { _entity.name }
	transform { _entity.transform }

	onCreate() {}
	onDestroy() {}
	onUpdate(delta) {}
	onLateUpdate(delta) {}

	onCollisionEnter() {}
	onCollisionExit() {}
}
