player = {}

function player.update(delta)

	vel = Vector2.new()

	mult = 25 * delta
	if Input.IsKeyPressed(KeyCode.LeftShift or KeyCode.RightShift) then
		mult = 2 * mult
	end

	if Input.IsKeyPressed(KeyCode.A) then vel.x = vel.x - mult end
	if Input.IsKeyPressed(KeyCode.D) then vel.x = vel.x + mult end
	if Input.IsKeyPressed(KeyCode.W) then vel.y = vel.y + mult end
	if Input.IsKeyPressed(KeyCode.S) then vel.y = vel.y - mult end

	rb = player.entity:GetRigidBody2DComponent().RigidBody
	rb:ApplyLinearImpulseToCenter(vel, true)

end
