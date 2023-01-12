import "script" for Script
import "keycodes" for KeyCode
import "input" for Input
import "math" for Math, Vector2
import "components" for RigidBody2DComponent
import "scheduler" for Scheduler, WaitSeconds

class Player is Script {
	construct new(e) {
		super(e)
	}

	onCreate() {
		System.print("My name is '%(entity.name)'")
		_rb = entity.getComponent(RigidBody2DComponent)

		Scheduler.add {
			var i = 10
			while (i >= 0) {
				System.print("%(this.name)'s countdown: %(i)")
				Fiber.yield(WaitSeconds.new(0.1))
				i = i - 1
			}
			System.print("Boom!")
		}
	}

	onUpdate(delta) {
		var impulse = Vector2.new(0, 0)

		if (Input.isKeyPressed(KeyCode.a)) { impulse.x = impulse.x - 1 }
		if (Input.isKeyPressed(KeyCode.d)) { impulse.x = impulse.x + 1 }
		if (Input.isKeyPressed(KeyCode.w)) { impulse.y = impulse.y + 1 }
		if (Input.isKeyPressed(KeyCode.s)) { impulse.y = impulse.y - 1 }

		_rb.applyLinearImpulseToCenter(impulse * 20 * delta)

		// System.print("%(Input.mouseX) %(Input.mouseY) %(Input.mousePosition)")
		Scheduler.poll()
	}
}

class CameraController is Script {
	construct new(e) {
		super(e)
	}

	player=(rhs) { _player = rhs }

	onLateUpdate(delta) {
		transform.position = Math.lerp(_player.transform.position, transform.position, 0.5)
	}
}
