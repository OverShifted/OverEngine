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
layout(location = 9)  in float a_TextureTilingFactorX;
layout(location = 10) in float a_TextureTilingFactorY;
layout(location = 11) in vec4 a_TextureBorderColor;
layout(location = 12) in vec4 a_TextureRect;
layout(location = 13) in vec2 a_TextureSize;
layout(location = 14) in vec2 a_TextureCoord;

out vec4 v_Color;
flat out int v_UseTexture;
flat out int v_TextureSWrapping;
flat out int v_TextureTWrapping;
flat out int v_TextureFilter;
flat out int v_TextureSlot;
flat out int v_TextureFlipX;
flat out int v_TextureFlipY;
out float v_TextureTilingFactorX;
out float v_TextureTilingFactorY;
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
	v_TextureTilingFactorX = a_TextureTilingFactorX;
	v_TextureTilingFactorY = a_TextureTilingFactorY;
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
in float v_TextureTilingFactorX;
in float v_TextureTilingFactorY;
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

vec4 Sample(sampler2D slot, vec2 coord)
{
	if (v_TextureFilter == 0)
			return PointSampleFromAtlas(slot, coord * vec2(v_TextureTilingFactorX, v_TextureTilingFactorY), v_TextureRect, v_TextureSWrapping, v_TextureTWrapping) * v_Color;
	return BiLinearSampleFromAtlas(slot, coord * vec2(v_TextureTilingFactorX, v_TextureTilingFactorY)) * v_Color;
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
		switch (v_TextureSlot)
		{
			case 0: o_Color = Sample(u_Slots[0], coord); break;
			case 1: o_Color = Sample(u_Slots[1], coord); break;
			case 2: o_Color = Sample(u_Slots[2], coord); break;
			case 3: o_Color = Sample(u_Slots[3], coord); break;
			case 4: o_Color = Sample(u_Slots[4], coord); break;
			case 5: o_Color = Sample(u_Slots[5], coord); break;
			case 6: o_Color = Sample(u_Slots[6], coord); break;
			case 7: o_Color = Sample(u_Slots[7], coord); break;
			case 8: o_Color = Sample(u_Slots[8], coord); break;
			case 9: o_Color = Sample(u_Slots[9], coord); break;
			case 10: o_Color = Sample(u_Slots[10], coord); break;
			case 11: o_Color = Sample(u_Slots[11], coord); break;
			case 12: o_Color = Sample(u_Slots[12], coord); break;
			case 13: o_Color = Sample(u_Slots[13], coord); break;
			case 14: o_Color = Sample(u_Slots[14], coord); break;
			case 15: o_Color = Sample(u_Slots[15], coord); break;
			case 16: o_Color = Sample(u_Slots[16], coord); break;
			case 17: o_Color = Sample(u_Slots[17], coord); break;
			case 18: o_Color = Sample(u_Slots[18], coord); break;
			case 19: o_Color = Sample(u_Slots[19], coord); break;
			case 20: o_Color = Sample(u_Slots[20], coord); break;
			case 21: o_Color = Sample(u_Slots[21], coord); break;
			case 22: o_Color = Sample(u_Slots[22], coord); break;
			case 23: o_Color = Sample(u_Slots[23], coord); break;
			case 24: o_Color = Sample(u_Slots[24], coord); break;
			case 25: o_Color = Sample(u_Slots[25], coord); break;
			case 26: o_Color = Sample(u_Slots[26], coord); break;
			case 27: o_Color = Sample(u_Slots[27], coord); break;
			case 28: o_Color = Sample(u_Slots[28], coord); break;
			case 29: o_Color = Sample(u_Slots[29], coord); break;
			case 30: o_Color = Sample(u_Slots[30], coord); break;
			case 31: o_Color = Sample(u_Slots[31], coord); break;
		}
	}
	else
	{
		o_Color = v_Color;
	}
}