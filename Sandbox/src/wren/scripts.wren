import "src/wren/lib/script" for Script

class Player is Script {
	construct new(e) {
		super(e)
	}

	onCreate() {
		System.print("My name is '%(entity.name)'")
	}
}

class CameraController is Script {
	construct new(e) {
		super(e)
	}

	player=(rhs) { _player = rhs }

	onLateUpdate(delta) {
		entity.position = _player.position
	}
}
