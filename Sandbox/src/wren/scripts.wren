import "script" for Script
import "keycodes" for KeyCode
import "input" for Input
import "math" for Math, Vector2, Vector3
import "components" for RigidBody2DComponent
import "scheduler" for Scheduler, WaitSeconds
import "imgui" for ImGui
import "time" for Time

class Player is Script {
	construct new(e) {
		super(e)
	}

	onCreate() {
		System.print("My name is '%(entity.name)'")
		_rb = entity.getComponent(RigidBody2DComponent)

		Scheduler.add { countdown() }
	}

	onUpdate(delta) {
		var impulse = Vector2.new(0, 0)

		if (Input.isKeyPressed(KeyCode.a)) { impulse.x = impulse.x - 1 }
		if (Input.isKeyPressed(KeyCode.d)) { impulse.x = impulse.x + 1 }
		if (Input.isKeyPressed(KeyCode.w)) { impulse.y = impulse.y + 1 }
		if (Input.isKeyPressed(KeyCode.s)) { impulse.y = impulse.y - 1 }

		_rb.applyLinearImpulseToCenter(impulse * 20 * delta)
	}

	countdown() {
		var i = 10
		while (i >= 0) {
			System.print("%(this.name)'s countdown: %(i)")
			Fiber.yield(WaitSeconds.new(0.1))
			i = i - 1
		}
		System.print("Boom!")
	}
}

class CameraController is Script {
	static instances { __instances }
	construct new(e) {
		super(e)
		_foo = 42
		_bar = "Me"
		_baz = [12, "Me", 42]
		_boo = [this, _baz, _foo, _bar]
		_baz.add(null)
		_baz.add({
			"name": "apple",
			"color": "red"
		})

		if (__instances == null) { __instances = [] }
		__instances.add(this)

		// It will cause infinite recursion
		// _baz.add(_baz)
	}

	player=(rhs) { _player = rhs }

	onUpdate(delta) {
		var rotDelta = 0
		if (Input.isKeyPressed(KeyCode.q)) { rotDelta = rotDelta - 1 }
		if (Input.isKeyPressed(KeyCode.e)) { rotDelta = rotDelta + 1 }
		rotDelta = rotDelta * delta * 80

		transform.eulerAngles = Vector3.new(0.0, 0.0, transform.eulerAngles.z + rotDelta)

		if (Input.isKeyPressed(KeyCode.escape)) {
			transform.position = Vector3.new(0, 0, 0)
			transform.eulerAngles = Vector3.new(0, 0, 0)
		}

		_foo = _foo + 1
		_bar = "Me %(_foo)"
		_baz[1] = _bar
		_baz[-1]["name"] = "apple %(_foo)"
	}

	onLateUpdate(delta) {
		transform.position = Math.lerp(_player.transform.position, transform.position, 0.5)
	}

	// TODO: Add abstract method to the Script class?
	onImGuiRender() {
		ImGui.begin("Wren")
		ImGui.text("FPS: %(1 / Time.deltaTime)")
		ImGui.text("Camera position: %(transform.position)")
		ImGui.end()
	}
}
