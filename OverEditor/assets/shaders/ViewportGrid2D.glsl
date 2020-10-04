#type vertex
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_UV;

out vec2 v_UV;

void main()
{
	gl_Position = vec4(a_Position, 0.0, 1.0);
	v_UV = a_UV;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

in vec2 v_UV;

uniform vec2 u_CameraPos;
uniform vec2 u_WorldSpaceSize;

uniform float u_GridZoom = 6.0;
uniform float u_LineKernel = 0.0;

#define STEPS_LEN 8
uniform float u_GridSteps[STEPS_LEN] = float[](0.001, 0.01, 0.1, 1.0, 10.0, 100.0, 1000.0, 10000.0);

/*
// Old versions
#define DISC_RADIUS (0.5641895835477563 * 1.05)
#define GRID_LINE_SMOOTH_START (0.5 - DISC_RADIUS)
#define GRID_LINE_SMOOTH_END (0.5 + DISC_RADIUS)
*/

// New versions
#define GRID_LINE_SMOOTH_START (-0.09239906272514409)
#define GRID_LINE_SMOOTH_END (1.092399062725144)

float get_grid(vec2 co, vec2 fwidthCos, float grid_size)
{
	float half_size = grid_size / 2.0;
	/* triangular wave pattern, amplitude is [0, half_size] */
	vec2 grid_domain = abs(mod(co + half_size, grid_size) - half_size);
	/* modulate by the absolute rate of change of the coordinates
	 * (make lines have the same width under perspective) */
	grid_domain /= fwidthCos;

	/* collapse waves */
	float line_dist = min(grid_domain.x, grid_domain.y);

	return 1.0 - smoothstep(GRID_LINE_SMOOTH_START, GRID_LINE_SMOOTH_END, line_dist - u_LineKernel);
}

vec3 get_axes(vec3 co, vec3 fwidthCos, float line_size)
{
	vec3 axes_domain = abs(co);
	/* modulate by the absolute rate of change of the coordinates
	 * (make line have the same width under perspective) */
	axes_domain /= fwidthCos;

	return 1.0 - smoothstep(GRID_LINE_SMOOTH_START,
							GRID_LINE_SMOOTH_END,
							axes_domain - (line_size + u_LineKernel));
}

#define linearstep(p0, p1, v) (clamp(((v) - (p0)) / abs((p1) - (p0)), 0.0, 1.0))
void main()
{
	vec3 wPos = vec3(v_UV * u_WorldSpaceSize * 2 - u_WorldSpaceSize + u_CameraPos, 0.0);
	vec3 fwidthPos = abs(dFdx(wPos)) + abs(dFdy(wPos));

	float grid_res = u_GridZoom * u_WorldSpaceSize.y * 0.0004;

	vec4 scale;
	if (u_GridSteps[0] > grid_res)
		scale = vec4(0.0, u_GridSteps[0], u_GridSteps[1], u_GridSteps[2]);
	else if (u_GridSteps[1] > grid_res)
		scale = vec4(u_GridSteps[0], u_GridSteps[1], u_GridSteps[2], u_GridSteps[3]);
	else if (u_GridSteps[2] > grid_res)
		scale = vec4(u_GridSteps[1], u_GridSteps[2], u_GridSteps[3], u_GridSteps[4]);
	else if (u_GridSteps[3] > grid_res)
		scale = vec4(u_GridSteps[2], u_GridSteps[3], u_GridSteps[4], u_GridSteps[5]);
	else if (u_GridSteps[4] > grid_res)
		scale = vec4(u_GridSteps[3], u_GridSteps[4], u_GridSteps[5], u_GridSteps[6]);
	else if (u_GridSteps[5] > grid_res)
		scale = vec4(u_GridSteps[4], u_GridSteps[5], u_GridSteps[6], u_GridSteps[7]);
	else if (u_GridSteps[6] > grid_res)
		scale = vec4(u_GridSteps[5], u_GridSteps[6], u_GridSteps[7], u_GridSteps[7]);
	else
		scale = vec4(u_GridSteps[6], u_GridSteps[7], u_GridSteps[7], u_GridSteps[7]);

	float blend = 1.0 - linearstep(scale[0], scale[1], grid_res);
	blend = blend * blend * blend;

	o_Color = vec4(0.0);
	o_Color.a *= get_grid(wPos.xy, fwidthPos.xy, scale[1]) * blend;
	o_Color = mix(o_Color, mix(vec4(0.0), vec4(1.0), blend), get_grid(wPos.xy, fwidthPos.xy, scale[2]));
	o_Color = mix(o_Color, vec4(1.0), get_grid(wPos.xy, fwidthPos.xy, scale[3]));

	/* Computing all axes at once using vec3 */
	vec3 axes = get_axes(
		vec3(wPos.y + wPos.z, wPos.x + wPos.z, 0.0),
		vec3(fwidthPos.y + fwidthPos.z, fwidthPos.x + fwidthPos.z, 0.0), 0.1);
	o_Color.rgb = vec3(1.0);
	o_Color.rgb = (axes.x < 1e-8) ? o_Color.rgb : vec3(1.0, 0.0, 0.0);
	o_Color.rgb = (axes.y < 1e-8) ? o_Color.rgb : vec3(0.0, 1.0, 0.0);
	o_Color.a *= 1.0 - smoothstep(0.0, 0.5, clamp(gl_FragCoord.z * 2.0 - 1.0, 0.0, 1.0) - 0.5);
}
