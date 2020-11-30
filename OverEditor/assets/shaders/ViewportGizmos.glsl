#type vertex
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_UV;

uniform mat4 u_ViewProjMatrix;
uniform mat4 u_Transform;

out vec2 v_UV;

void main()
{
	gl_Position = u_ViewProjMatrix * u_Transform * vec4(a_Position, 0.0, 1.0);
	v_UV = a_UV;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

uniform vec4 u_Color;

in vec2 v_UV;

void main()
{
	o_Color = u_Color;

	// Point between arrow's triangle and its rectangle part
	float mid = 0.8;

	// Thickness of arrow's triangle rectangle part
	float rectangleThickness = 0.08;
	
	float vRemapped = abs(v_UV.y - 0.5);
	if (v_UV.x > mid)
	{
		if (vRemapped > (1 - v_UV.x) / (1 - mid) / 2)
			discard;
	}
	else if (vRemapped > rectangleThickness)
		discard;
}
