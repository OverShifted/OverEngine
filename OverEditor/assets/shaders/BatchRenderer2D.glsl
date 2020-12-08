#type vertex
#version 450 core

layout(location = 0) in vec3  a_Position0;
layout(location = 1) in vec3  a_Position1;
layout(location = 2) in vec3  a_Position2;
layout(location = 3) in vec3  a_Position3;

layout(location = 4) in vec4  a_Color;       // Tint

layout(location = 5) in vec2   a_TexTiling;
layout(location = 6) in vec2   a_TexOffset;
layout(location = 7) in int    a_TexFlip;
layout(location = 8) in int    a_TexSlot;     // Texture Unit or -1 for no texture
layout(location = 9) in int    a_TexFilter;   // Linear / Point
layout(location = 10) in ivec2 a_TexWrapping; // Repeat, MirroredRepeat, ...
layout(location = 11) in vec2  a_TexSize;     // Texture size in pixels
layout(location = 12) in vec4  a_TexRect;     // 0 -> 1 Rect from Atlas

out VS_OUT {
	vec3  Position1;
	vec3  Position2;
	vec3  Position3;

	vec4  Color;

	vec2  TexTiling;
	vec2  TexOffset;
	int   TexFlip;
	int   TexSlot;
	int   TexFilter;
	ivec2 TexWrapping;
	vec2  TexSize;
	vec4  TexRect;
} vs_out;

void main()
{
	gl_Position    = vec4(a_Position0, 1.0);

	vs_out.Position1 = a_Position1;
	vs_out.Position2 = a_Position2;
	vs_out.Position3 = a_Position3;

	vs_out.Color = a_Color;

	vs_out.TexTiling   = a_TexTiling;
	vs_out.TexOffset   = a_TexOffset;
	vs_out.TexFlip     = a_TexFlip;
	vs_out.TexSlot     = a_TexSlot;
	vs_out.TexFilter   = a_TexFilter;
	vs_out.TexWrapping = a_TexWrapping;
	vs_out.TexSize     = a_TexSize;
	vs_out.TexRect     = a_TexRect;
}

#type geometry
#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT {
	vec3  Position1;
	vec3  Position2;
	vec3  Position3;

	vec4  Color;

	vec2  TexTiling;
	vec2  TexOffset;
	int   TexFlip;
	int   TexSlot;
	int   TexFilter;
	ivec2 TexWrapping;
	vec2  TexSize;
	vec4  TexRect;
} gs_in[];

flat out vec4 v_Color;
flat out int v_TexSlot;
flat out int v_TexFilter;
flat out ivec2 v_TexWrapping;
flat out vec2 v_TexSize;
flat out vec4 v_TexRect;
out vec2 v_UV;

#define FLIP_X (1 << 0)
#define FLIP_Y (1 << 1)

vec2 FlipUV(vec2 uv)
{
	if ((gs_in[0].TexFlip & FLIP_X) == FLIP_X)
		uv.x = 1.0 - uv.x;
	if ((gs_in[0].TexFlip & FLIP_Y) == FLIP_Y)
		uv.y = 1.0 - uv.y;
	return uv;
}

void main()
{
	v_Color       = gs_in[0].Color;
	v_TexSlot     = gs_in[0].TexSlot;
	v_TexFilter   = gs_in[0].TexFilter;
	v_TexWrapping = gs_in[0].TexWrapping;
	v_TexRect     = gs_in[0].TexRect;
	v_TexSize     = gs_in[0].TexSize;

	gl_Position = gl_in[0].gl_Position;
	v_UV = FlipUV(vec2(gs_in[0].TexOffset.x, 1.0 - gs_in[0].TexOffset.y));
	EmitVertex();

	gl_Position = vec4(gs_in[0].Position1, 1.0);
	v_UV = FlipUV(vec2(gs_in[0].TexTiling.x + gs_in[0].TexOffset.x, 1.0 - gs_in[0].TexOffset.y));
	EmitVertex();

	gl_Position = vec4(gs_in[0].Position2, 1.0);
	v_UV = FlipUV(vec2(gs_in[0].TexOffset.x, 1.0 - (gs_in[0].TexTiling.y + gs_in[0].TexOffset.y)));
	EmitVertex();

	gl_Position = vec4(gs_in[0].Position3, 1.0);
	v_UV = FlipUV(vec2(gs_in[0].TexTiling.x + gs_in[0].TexOffset.x, 1.0 - (gs_in[0].TexTiling.y + gs_in[0].TexOffset.y)));
	EmitVertex();

	EndPrimitive();
}

#type fragment
#version 450 core
#pragma precision highp float

layout(location = 0) out vec4 o_Color;

uniform sampler2D[32] u_Slots;

flat in vec4 v_Color;
flat in int v_TexSlot;
flat in int v_TexFilter;
flat in ivec2 v_TexWrapping;
flat in vec2 v_TexSize;
flat in vec4 v_TexRect;
in vec2 v_UV;

vec2 texelSize;

vec4 PointSampleFromAtlas(sampler2D slot, vec2 coord);

vec4 BiLinearSampleFromAtlas(sampler2D slot, vec2 coord)
{
	float a = mod(coord.x, texelSize.x) * v_TexSize.x;
	float b = mod(coord.y, texelSize.y) * v_TexSize.y;

	if (a > 0.5) a -= 0.5;
	else if (a < 0.5) a += 0.5;

	if (b > 0.5) b -= 0.5;
	else if (b < 0.5) b += 0.5;

	float xmin = coord.x - texelSize.x / 2;
	float xmax = coord.x + texelSize.x / 2;

	float ymin = coord.y - texelSize.y / 2;
	float ymax = coord.y + texelSize.y / 2;

	float xtile = floor(v_UV.x);
	float ytile = floor(v_UV.y);

	if (floor(xmin) < xtile) xmin = xtile + texelSize.x;
	if (floor(xmax) > xtile) xmax = xtile - texelSize.x;

	if (floor(ymin) < ytile) ymin = ytile + texelSize.y;
	if (floor(ymax) > ytile) ymax = ytile - texelSize.y;

	vec4 upperLeft  = PointSampleFromAtlas(slot, vec2(xmin, ymin));
	vec4 upperRight = PointSampleFromAtlas(slot, vec2(xmax, ymin));
	vec4 lowerLeft  = PointSampleFromAtlas(slot, vec2(xmin, ymax));
	vec4 lowerRight = PointSampleFromAtlas(slot, vec2(xmax, ymax));

	return mix(
		mix(upperLeft, upperRight, a),
		mix(lowerLeft, lowerRight, a),
		b
	);
}

// Based on https://www.khronos.org/registry/OpenGL/specs/gl/glspec46.core.pdf
// page 260
float mirror(float a, float texel_size) { return (a >= 0 ? a : -(texel_size + a)); }
float wrap(float value, int wrapping, float texel_size)
{
	switch (wrapping)
	{
	case 1: return clamp(mod(value, 1.0), 0.0, 1.0 - texel_size);                       // Repeat
	case 2: return clamp(value, 0.0, 1.0 - texel_size);            // Clamp
	case 3: return clamp((1 - texel_size) - mirror(mod(value, 2.0) - 1.0, texel_size), 0.0, 1.0 - texel_size); // Mirror
	}
}

vec4 PointSampleFromAtlas(sampler2D slot, vec2 coord)
{
	coord.x = wrap(coord.x, v_TexWrapping.x, texelSize.x);
	coord.y = wrap(coord.y, v_TexWrapping.y, texelSize.y);

	return texture(slot, vec2(
		v_TexRect.x + (coord.x * v_TexRect.z),
		v_TexRect.y + (coord.y * v_TexRect.w)
	));
}

vec4 Sample(sampler2D slot)
{
	vec4 color = v_Color;

	texelSize = vec2(1 / v_TexSize.x, 1 / v_TexSize.y);

	if (v_TexFilter == 1)
		color *= PointSampleFromAtlas(slot, v_UV);
	else
		color *= BiLinearSampleFromAtlas(slot, v_UV);

	if (color.a <= 0.0) discard;
	return color;
}

void main()
{
	// if (v_TexWrapping.y == 0)
	// {
	// 	o_Color = vec4(v_TexWrapping.y,0.0,0.0,1.0);
	// 	return;
	// }
	switch (v_TexSlot)
	{
	case -1 : o_Color = v_Color; return;
	case  0 : o_Color = Sample(u_Slots[0 ]); return;
	case  1 : o_Color = Sample(u_Slots[1 ]); return;
	case  2 : o_Color = Sample(u_Slots[2 ]); return;
	case  3 : o_Color = Sample(u_Slots[3 ]); return;
	case  4 : o_Color = Sample(u_Slots[4 ]); return;
	case  5 : o_Color = Sample(u_Slots[5 ]); return;
	case  6 : o_Color = Sample(u_Slots[6 ]); return;
	case  7 : o_Color = Sample(u_Slots[7 ]); return;
	case  8 : o_Color = Sample(u_Slots[8 ]); return;
	case  9 : o_Color = Sample(u_Slots[9 ]); return;
	case 10 : o_Color = Sample(u_Slots[10]); return;
	case 11 : o_Color = Sample(u_Slots[11]); return;
	case 12 : o_Color = Sample(u_Slots[12]); return;
	case 13 : o_Color = Sample(u_Slots[13]); return;
	case 14 : o_Color = Sample(u_Slots[14]); return;
	case 15 : o_Color = Sample(u_Slots[15]); return;
	case 16 : o_Color = Sample(u_Slots[16]); return;
	case 17 : o_Color = Sample(u_Slots[17]); return;
	case 18 : o_Color = Sample(u_Slots[18]); return;
	case 19 : o_Color = Sample(u_Slots[19]); return;
	case 20 : o_Color = Sample(u_Slots[20]); return;
	case 21 : o_Color = Sample(u_Slots[21]); return;
	case 22 : o_Color = Sample(u_Slots[22]); return;
	case 23 : o_Color = Sample(u_Slots[23]); return;
	case 24 : o_Color = Sample(u_Slots[24]); return;
	case 25 : o_Color = Sample(u_Slots[25]); return;
	case 26 : o_Color = Sample(u_Slots[26]); return;
	case 27 : o_Color = Sample(u_Slots[27]); return;
	case 28 : o_Color = Sample(u_Slots[28]); return;
	case 29 : o_Color = Sample(u_Slots[29]); return;
	case 30 : o_Color = Sample(u_Slots[30]); return;
	case 31 : o_Color = Sample(u_Slots[31]); return;
	}
}
