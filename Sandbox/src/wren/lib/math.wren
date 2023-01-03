foreign class Vector3 {
	construct new(x, y, z) {}

	foreign x
	foreign y
	foreign z

	foreign x=(rhs)
	foreign y=(rhs)
	foreign z=(rhs)

	toString { "Vector3(%(x), %(y), %(z))" }
}
