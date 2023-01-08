import "script" for Script
import "keycodes" for KeyCode
import "input" for Input
import "math" for Vector3

class Player is Script {
	construct new(e) {
		super(e)
	}

	onCreate() {
		System.print("My name is '%(entity.name)'")
	}

	onUpdate(delta) {
		var impulse = Vector3.new(0, 0, 0)

		if (Input.isKeyPressed(KeyCode.a)) { impulse.x = impulse.x - 1 }
		if (Input.isKeyPressed(KeyCode.d)) { impulse.x = impulse.x + 1 }
		if (Input.isKeyPressed(KeyCode.w)) { impulse.y = impulse.y + 1 }
		if (Input.isKeyPressed(KeyCode.s)) { impulse.y = impulse.y - 1 }

		entity.applyLinearImpulseToCenter(impulse * 20 * delta)
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
