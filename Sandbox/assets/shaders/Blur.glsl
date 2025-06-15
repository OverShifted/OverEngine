#type vertex
#version 330 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main() {
	gl_Position = a_Position;
	v_TexCoord = a_TexCoord;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;

uniform sampler2D u_in;
uniform vec2 u_direction;
uniform vec2 u_resolution;
uniform float u_radius;

vec4 blur13(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
	vec4 color = vec4(0.0);
	vec2 off1 = vec2(1.411764705882353) * direction;
	vec2 off2 = vec2(3.2941176470588234) * direction;
	vec2 off3 = vec2(5.176470588235294) * direction;
	color += texture(image, uv) * 0.1964825501511404;
	color += texture(image, uv + (off1 / resolution)) * 0.2969069646728344;
	color += texture(image, uv - (off1 / resolution)) * 0.2969069646728344;
	color += texture(image, uv + (off2 / resolution)) * 0.09447039785044732;
	color += texture(image, uv - (off2 / resolution)) * 0.09447039785044732;
	color += texture(image, uv + (off3 / resolution)) * 0.010381362401148057;
	color += texture(image, uv - (off3 / resolution)) * 0.010381362401148057;
	return color;
}

float rand(vec2 co){
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
	// o_Color = vec4(v_TexCoord, 0.15, 1.0);
	// vec4 noise = vec4(vec3(rand(v_TexCoord) - 0.5), 0.0);
	// o_Color = blur13(u_in, v_TexCoord, u_resolution, u_direction * u_radius) + noise * 0.2;
	o_Color = blur13(u_in, v_TexCoord, u_resolution, u_direction * u_radius);
}
