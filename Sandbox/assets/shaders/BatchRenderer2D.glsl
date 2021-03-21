#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position0;
layout(location = 1) in vec3 a_Position1;
layout(location = 2) in vec3 a_Position2;
layout(location = 3) in vec3 a_Position3;

layout(location = 4) in vec4 a_Color;
layout(location = 5) in int  a_TexSlot;
layout(location = 6) in vec4 a_TexCoord;
layout(location = 7) in vec4 a_TexRegion;
layout(location = 8) in int a_TexRepeat;

out VS_OUT {
	// Position0 -> gl_Position;
	vec3 Position1;
	vec3 Position2;
	vec3 Position3;

	vec4 Color;
	int  TexSlot;
	vec4 TexCoord;
	vec4 TexRegion;
	int TexRepeat;
} vs_out;

void main()
{
	// gl_Position is used insted of vs_out.Position0
	gl_Position      = vec4(a_Position0, 1.0);
	vs_out.Position1 = a_Position1;
	vs_out.Position2 = a_Position2;
	vs_out.Position3 = a_Position3;

	vs_out.Color     = a_Color;
	vs_out.TexSlot   = a_TexSlot;
	vs_out.TexCoord  = a_TexCoord;
	vs_out.TexRegion = a_TexRegion;
	vs_out.TexRepeat = a_TexRepeat;
}

#type geometry
#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT {
	// Position0 -> gl_Position;
	vec3 Position1;
	vec3 Position2;
	vec3 Position3;

	vec4 Color;
	int  TexSlot;
	vec4 TexCoord;
	vec4 TexRegion;
	int TexRepeat;
} gs_in[];

flat out vec4 v_Color;
flat out int v_TexSlot;
flat out vec4 v_TexRegion;
flat out int v_TexRepeat;
out vec2 v_TexCoord;

#define FLIP_X (1 << 0)
#define FLIP_Y (1 << 1)

void main()
{
	v_Color     = gs_in[0].Color;
	v_TexSlot   = gs_in[0].TexSlot;
	v_TexRegion = gs_in[0].TexRegion;
	v_TexRepeat = gs_in[0].TexRepeat;

	gl_Position = gl_in[0].gl_Position;
	v_TexCoord  = gs_in[0].TexCoord.xy + vec2(0.0, gs_in[0].TexCoord.w);
	EmitVertex();

	gl_Position = vec4(gs_in[0].Position1, 1.0);
	v_TexCoord  = gs_in[0].TexCoord.xy + gs_in[0].TexCoord.zw;
	EmitVertex();

	gl_Position = vec4(gs_in[0].Position2, 1.0);
	v_TexCoord  = gs_in[0].TexCoord.xy;
	EmitVertex();

	gl_Position = vec4(gs_in[0].Position3, 1.0);
	v_TexCoord  = gs_in[0].TexCoord.xy + vec2(gs_in[0].TexCoord.z, 0.0);
	EmitVertex();

	EndPrimitive();
}

#type fragment
#version 450 core
#pragma precision highp float

layout(location = 0) out vec4 o_Color;

flat in vec4 v_Color;
flat in int v_TexSlot;
flat in vec4 v_TexRegion;
flat in int v_TexRepeat;
in vec2 v_TexCoord;

uniform sampler2D[32] u_Slots;

#define EPSILON1 (0.0000000000001)
#define EPSILON2 (0.0001)
#define INT_NOT(x) (int(!bool(x)))

void Sample(sampler2D slot)
{
	vec2 coord = v_TexCoord;

	coord = (v_TexRepeat == 1) ? vec2(mod(coord.x - v_TexRegion.x, v_TexRegion.z) + v_TexRegion.x, mod(coord.y - v_TexRegion.x, v_TexRegion.w) + v_TexRegion.y) : coord;
	coord += EPSILON2 * v_TexRepeat * vec2(coord.x < v_TexRegion.x + EPSILON1, coord.y < v_TexRegion.y + EPSILON1);

	o_Color *= texture(slot, coord);
	if (o_Color.a == 0.0) discard;
}

void main()
{
	o_Color = v_Color;
	switch (v_TexSlot)
	{
	case  0: Sample(u_Slots[0 ]); return;
	case  1: Sample(u_Slots[1 ]); return;
	case  2: Sample(u_Slots[2 ]); return;
	case  3: Sample(u_Slots[3 ]); return;
	case  4: Sample(u_Slots[4 ]); return;
	case  5: Sample(u_Slots[5 ]); return;
	case  6: Sample(u_Slots[6 ]); return;
	case  7: Sample(u_Slots[7 ]); return;
	case  8: Sample(u_Slots[8 ]); return;
	case  9: Sample(u_Slots[9 ]); return;
	case 10: Sample(u_Slots[10]); return;
	case 11: Sample(u_Slots[11]); return;
	case 12: Sample(u_Slots[12]); return;
	case 13: Sample(u_Slots[13]); return;
	case 14: Sample(u_Slots[14]); return;
	case 15: Sample(u_Slots[15]); return;
	case 16: Sample(u_Slots[16]); return;
	case 17: Sample(u_Slots[17]); return;
	case 18: Sample(u_Slots[18]); return;
	case 19: Sample(u_Slots[19]); return;
	case 20: Sample(u_Slots[20]); return;
	case 21: Sample(u_Slots[21]); return;
	case 22: Sample(u_Slots[22]); return;
	case 23: Sample(u_Slots[23]); return;
	case 24: Sample(u_Slots[24]); return;
	case 25: Sample(u_Slots[25]); return;
	case 26: Sample(u_Slots[26]); return;
	case 27: Sample(u_Slots[27]); return;
	case 28: Sample(u_Slots[28]); return;
	case 29: Sample(u_Slots[29]); return;
	case 30: Sample(u_Slots[30]); return;
	case 31: Sample(u_Slots[31]); return;
	}
}
