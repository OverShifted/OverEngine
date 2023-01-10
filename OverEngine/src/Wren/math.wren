foreign class Vector3 {
	construct new(x, y, z) {}

	foreign x
	foreign y
	foreign z

	foreign x=(rhs)
	foreign y=(rhs)
	foreign z=(rhs)

	toString { "Vector3(%(x), %(y), %(z))" }

	*(rhs) { Vector3.new(x * rhs, y * rhs, z * rhs) }
	+(rhs) { Vector3.new(x + rhs.x, y + rhs.y, z + rhs.z) }

	lerp(b, t) { this * (1 - t) + b * t }
}
