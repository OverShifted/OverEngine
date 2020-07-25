#type vertex
#version 330 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in float a_UseTexture;
layout(location = 3) in float a_TextureSWrapping;
layout(location = 4) in float a_TextureTWrapping;
layout(location = 5) in float a_TextureFilter;
layout(location = 6) in float a_TextureSlot;
layout(location = 7) in float a_TextureFlipX;
layout(location = 8) in float a_TextureFlipY;
layout(location = 9) in float a_TextureTilingFactor;
layout(location = 10) in vec4 a_TextureBorderColor;
layout(location = 11) in vec4 a_TextureRect;
layout(location = 12) in vec2 a_TextureSize;
layout(location = 13) in vec2 a_TextureCoord;

out vec4 v_Color;
flat out int v_UseTexture;
flat out int v_TextureSWrapping;
flat out int v_TextureTWrapping;
flat out int v_TextureFilter;
flat out int v_TextureSlot;
flat out int v_TextureFlipX;
flat out int v_TextureFlipY;
out float v_TextureTilingFactor;
out vec4 v_TextureBorderColor;
out vec4 v_TextureRect;
out vec2 v_TextureSize;
out vec2 v_TextureCoord;

void main()
{
	gl_Position = a_Position;
	v_Color = a_Color;
	v_UseTexture = int(a_UseTexture);
	v_TextureSWrapping = int(a_TextureSWrapping);
	v_TextureTWrapping = int(a_TextureTWrapping);
	v_TextureFilter = int(a_TextureFilter);
	v_TextureSlot = int(a_TextureSlot);
	v_TextureFlipX = int(a_TextureFlipX);
	v_TextureFlipY = int(a_TextureFlipY);
	v_TextureTilingFactor = a_TextureTilingFactor;
	v_TextureBorderColor = a_TextureBorderColor;
	v_TextureRect = a_TextureRect;
	v_TextureSize = a_TextureSize;
	v_TextureCoord = a_TextureCoord;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

uniform sampler2D[32] u_Slots;

in vec4 v_Color;
flat in int v_UseTexture;
flat in int v_TextureSWrapping;
flat in int v_TextureTWrapping;
flat in int v_TextureFilter;
flat in int v_TextureSlot;
flat in int v_TextureFlipX;
flat in int v_TextureFlipY;
in float v_TextureTilingFactor;
in vec4 v_TextureBorderColor;
in vec4 v_TextureRect;
in vec2 v_TextureSize;
in vec2 v_TextureCoord;

vec4 PointSampleFromAtlas(sampler2D slot, vec2 coord, vec4 subTextureRect, int SWrapping, int TWrapping);

vec4 BiLinearSampleFromAtlas(sampler2D slot, vec2 texCoord)
{
	texCoord.x *= v_TextureRect.z * v_TextureSize.x;
	texCoord.y *= v_TextureRect.w * v_TextureSize.y;

	float xmin = texCoord.x;
	float xmax = xmin + 1.0;

	float ymin = texCoord.y;
	float ymax = ymin + 1.0;

	vec2 crd1 = vec2(xmin / v_TextureSize.x, ymin / v_TextureSize.y);
	vec2 crd2 = vec2(xmax / v_TextureSize.x, ymin / v_TextureSize.y);

	vec4 TopMix = mix(
		PointSampleFromAtlas(slot, crd1 / v_TextureRect.zw, v_TextureRect, 1, 1),
		PointSampleFromAtlas(slot, crd2 / v_TextureRect.zw, v_TextureRect, 1, 1),
		fract(texCoord.x)
	);

	crd1 = vec2(xmin / v_TextureSize.x, ymax / v_TextureSize.y);
	crd2 = vec2(xmax / v_TextureSize.x, ymax / v_TextureSize.y);
	vec4 DownMix = mix(
		PointSampleFromAtlas(slot, crd1 / v_TextureRect.zw, v_TextureRect, 1, 1),
		PointSampleFromAtlas(slot, crd2 / v_TextureRect.zw, v_TextureRect, 1, 1),
		fract(texCoord.x)
	);

	return mix(TopMix, DownMix, fract(texCoord.y));
}

vec4 PointSampleFromAtlas(sampler2D slot, vec2 coord, vec4 subTextureRect, int SWrapping, int TWrapping)
{
	coord.y = 1 - coord.y;

	switch (SWrapping)
	{
	case 0: // Repeat
		if (coord.x > 1)
			coord.x -= coord.x == int(coord.x) ? int(coord.x) - 1 : int(coord.x);

		else if (coord.x < 0)
			coord.x -= coord.x == int(coord.x) ? int(coord.x) - 2 : int(coord.x) - 1;
		break;
	case 1: // MirroredRepeat
		if (coord.x > 1)
		{
			if (int(coord.x) % 2 == 0)
				coord.x -= coord.x == int(coord.x) ? int(coord.x) - 1 : int(coord.x);
			else
				coord.x = (coord.x == int(coord.x) ? int(coord.x) : int(coord.x) + 1) - coord.x;
		}

		else if (coord.x < 0)
		{
			if (int(coord.x) % 2 == 0)
				coord.x = (coord.x == int(coord.x) ? int(coord.x) - 1 : int(coord.x)) - coord.x;
			else
				coord.x -= coord.x == int(coord.x) ? int(coord.x) - 2 : int(coord.x) - 1;

		}
		break;
	case 2: // ClampToEdge
		coord.x = clamp(coord.x, 0.001f, 0.999f);
		break;
	case 3: // ClampToBorder
		if (coord.x > 1 || coord.x < 0)
			return v_TextureBorderColor;
		break;
	}

	switch (TWrapping)
	{
	case 0: // Repeat
		if (coord.y > 1)
			coord.y -= coord.y == int(coord.y) ? int(coord.y) - 1 : int(coord.y);

		else if (coord.y < 0)
			coord.y -= coord.y == int(coord.y) ? int(coord.y) - 2 : int(coord.y) - 1;
		break;
	case 1: // MirroredRepeat
		if (coord.y > 1)
		{
			if (int(coord.y) % 2 == 0)
				coord.y -= coord.y == int(coord.y) ? int(coord.y) - 1 : int(coord.y);
			else
				coord.y = (coord.y == int(coord.y) ? int(coord.y) : int(coord.y) + 1) - coord.y;
		}
	
		else if (coord.y < 0)
		{
			if (int(coord.y) % 2 == 0)
				coord.y = (coord.y == int(coord.y) ? int(coord.y) - 1 : int(coord.y)) - coord.y;
			else
				coord.y -= coord.y == int(coord.y) ? int(coord.y) - 2 : int(coord.y) - 1;
		}
		break;
	case 2: // ClampToEdge
		coord.y = clamp(coord.y, 0.0f, 1.0f);
		break;
	case 3: // ClampToBorder
		if (coord.y > 1 || coord.y < 0)
			return v_TextureBorderColor;
		break;
	}

	return texture(slot, vec2(subTextureRect.x + coord.x * subTextureRect.z, subTextureRect.y + coord.y * subTextureRect.w));
}

void main()
{
	vec2 coord = v_TextureCoord;

	if (v_TextureFlipX != 0)
		coord.x = 1 - coord.x;

	if (v_TextureFlipY != 0)
		coord.y = 1 - coord.y;

	if (v_UseTexture != 0)
	{
		if (v_TextureFilter == 0)
			o_Color = PointSampleFromAtlas(u_Slots[v_TextureSlot], coord * v_TextureTilingFactor, v_TextureRect, v_TextureSWrapping, v_TextureTWrapping) * v_Color;
		else
			o_Color = BiLinearSampleFromAtlas(u_Slots[v_TextureSlot], coord * v_TextureTilingFactor) * v_Color;
	}
	else
		o_Color = v_Color;
}