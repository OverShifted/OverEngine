import "src/wren/lib/entity" for Entity

class Script {
	construct new(entity) {
		_entity = entity
	}

	entity { _entity }

	onCreate() {}
	onDestroy() {}
	onUpdate(delta) {}
	onLateUpdate(delta) {}

	onCollisionEnter() {}
	onCollisionExit() {}
}
