class Math {
	static lerp(a, b, t) { a * (1 - t) + b * t }
}

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
}

foreign class Vector2 {
	construct new(x, y) {}

	foreign x
	foreign y

	foreign x=(rhs)
	foreign y=(rhs)

	toString { "Vector2(%(x), %(y))" }

	*(rhs) { Vector2.new(x * rhs, y * rhs) }
	+(rhs) { Vector2.new(x + rhs.x, y + rhs.y) }
}
