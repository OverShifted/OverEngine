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

layout(location = 9) in int a_LiquidGlass;

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

	int LiquidGlass;
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
	vs_out.LiquidGlass = a_LiquidGlass;
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

	int LiquidGlass;
} gs_in[];

flat out vec4 v_Color;
flat out int v_TexSlot;
flat out vec4 v_TexRegion;
flat out int v_TexRepeat;
out vec2 v_TexCoord;
flat out int v_LiquidGlass;
flat out vec3 v_MidPoint; // NDC space
out vec2 v_NDC;
flat out vec2 v_quadNDC2ScreenNDCScale;

#define FLIP_X (1 << 0)
#define FLIP_Y (1 << 1)

void main()
{
	v_Color     = gs_in[0].Color;
	v_TexSlot   = gs_in[0].TexSlot;
	v_TexRegion = gs_in[0].TexRegion;
	v_TexRepeat = gs_in[0].TexRepeat;
	v_LiquidGlass = gs_in[0].LiquidGlass;
	v_MidPoint = (gl_in[0].gl_Position.xyz + gs_in[0].Position3) * 0.5;
	v_quadNDC2ScreenNDCScale = gs_in[0].Position3.xy - v_MidPoint.xy;

	gl_Position = gl_in[0].gl_Position;
	v_NDC = gl_Position.xy;
	v_TexCoord  = gs_in[0].TexCoord.xy + vec2(0.0, gs_in[0].TexCoord.w);
	EmitVertex();

	gl_Position = vec4(gs_in[0].Position1, 1.0);
	v_NDC = gl_Position.xy;
	v_TexCoord  = gs_in[0].TexCoord.xy + gs_in[0].TexCoord.zw;
	EmitVertex();

	gl_Position = vec4(gs_in[0].Position2, 1.0);
	v_NDC = gl_Position.xy;
	v_TexCoord  = gs_in[0].TexCoord.xy;
	EmitVertex();

	gl_Position = vec4(gs_in[0].Position3, 1.0);
	v_NDC = gl_Position.xy;
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
flat in int v_LiquidGlass;
flat in vec3 v_MidPoint; // Screen space
in vec2 v_NDC;
flat in vec2 v_quadNDC2ScreenNDCScale;

uniform sampler2D[32] u_Slots;
uniform float u_powerFactor;

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

float sdSuperellipse(vec2 p, float n, float r) {
    // Take the absolute value of the coordinates, as the formula uses |x| and |y|.
    // This makes the shape symmetrical in all quadrants.
    vec2 p_abs = abs(p);

    // Numerator of the distance formula: |x|^n + |y|^n - r^n
    // This is the implicit equation of the superellipse. Its value is 0 on the
    // boundary, negative inside, and positive outside.
    float numerator = pow(p_abs.x, n) + pow(p_abs.y, n) - pow(r, n);

    // Denominator: n * sqrt(|x|^(2n-2) + |y|^(2n-2))
    // This is the magnitude of the gradient of the implicit function. Dividing by it
    // normalizes the result, making it a better approximation of true Euclidean distance.
    // Note: The pow() function can be computationally expensive. For a fixed, integer 'n'
    // (like n=4), you would get better performance by using direct multiplication.
    float den_x = pow(p_abs.x, 2.0 * n - 2.0);
    float den_y = pow(p_abs.y, 2.0 * n - 2.0);

    // Add a small epsilon to prevent division by zero at the origin (0,0).
    float denominator = n * sqrt(den_x + den_y) + 0.00001;

    // The final signed distance
    return numerator / denominator;
}

const float M_E = 2.71828;

uniform float u_a = 0.7;
uniform float u_b = 2.3;
uniform float u_c = 5.2;
uniform float u_d = 6.9;

float f(float x) {
	return -u_b * pow(u_c*M_E, -u_d*x-u_a) + 1.0;
}

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec4 blur5(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 off1 = vec2(1.3333333333333333) * direction;
  color += texture(image, uv) * 0.29411764705882354;
  color += texture(image, uv + (off1 / resolution)) * 0.35294117647058826;
  color += texture(image, uv - (off1 / resolution)) * 0.35294117647058826;
  return color; 
}

vec4 blur9(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 off1 = vec2(1.3846153846) * direction;
  vec2 off2 = vec2(3.2307692308) * direction;
  color += texture(image, uv) * 0.2270270270;
  color += texture(image, uv + (off1 / resolution)) * 0.3162162162;
  color += texture(image, uv - (off1 / resolution)) * 0.3162162162;
  color += texture(image, uv + (off2 / resolution)) * 0.0702702703;
  color += texture(image, uv - (off2 / resolution)) * 0.0702702703;
  return color;
}

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

void LiquidGlass() {

	// float rsample = int(floor(rand(v_TexCoord.x * 12 + v_TexCoord.y * 148120))) % 1000;
	// float rsample = rand(v_TexCoord);
	// rsample /= 1000;
	// o_Color = vec4(rsample, rsample, rsample, 1);
	// return;


	vec2 center = vec2(0.5);
	vec2 p = (v_TexCoord - center) * 2;
	float r = 1;
	float d = sdSuperellipse(p, u_powerFactor, r);

	if (d > 0) {
		discard;
	}

	float dist = -d;
	vec2 sampleP = vec2(1, -1) * p * f(dist);

	// vec2 quadNDC2ScreenNDCScale = vec2(
	// 	(v_NDC.x - v_MidPoint.x) / p.x,
	// 	(v_NDC.y - v_MidPoint.y) / p.y
	// );

	vec2 targetNDC = sampleP * v_quadNDC2ScreenNDCScale + v_MidPoint.xy;
	vec2 coord = targetNDC * 0.5 + vec2(0.5);

	if (max(coord.x, coord.y) > 1.0 || min(coord.x, coord.y) < 0.0) {
		o_Color = vec4(1.0, 0.0, 1.0, 1.0);
		return;
	}

	// o_Color = texture(u_Slots[5], coord) + vec4(rand(v_TexCoord));
	// o_Color = texture(u_Slots[5], coord) + vec4(rand(v_TexCoord) - 0.5) * 0.2;
	// vec4 sum = vec4(0);
	// sum += blur13(u_Slots[5], coord, vec2(300), vec2(0, 1));
	// sum += blur13(u_Slots[5], coord, vec2(300), vec2(0, -1));
	// sum += blur13(u_Slots[5], coord, vec2(300), vec2(1, 0));
	// sum += blur13(u_Slots[5], coord, vec2(300), vec2(-1, 0));
	
	// sum += blur13(u_Slots[5], coord, vec2(300), normalize(vec2(1, 1)));
	// sum += blur13(u_Slots[5], coord, vec2(300), normalize(vec2(1, -1)));
	// sum += blur13(u_Slots[5], coord, vec2(300), normalize(vec2(-1, 1)));
	// sum += blur13(u_Slots[5], coord, vec2(300), normalize(vec2(-1, -1)));

	// o_Color = sum / 8 + vec4(rand(v_TexCoord) - 0.5) * 0.2;
	// o_Color = texture(u_Slots[5], coord) + vec4(rand(v_TexCoord) - 0.5) * 0.2;
	o_Color = texture(u_Slots[5], coord) + 0.1;
	// o_Color = vec4(rand(v_TexCoord), rand(v_TexCoord), rand(v_TexCoord), 1);
}

void main()
{
	if (v_LiquidGlass == 1) {
		LiquidGlass();
		return;
	}

	if (v_LiquidGlass == 2) {
		o_Color = texture(u_Slots[5], vec2(v_TexCoord.x, 1 - v_TexCoord.y));
		// o_Color = vec4(v_TexCoord, 0.0, 1.0);
		// discard;
		
		return;
	}

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
