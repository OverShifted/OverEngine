#type vertex
#version 330 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color; // Tint
layout(location = 2) in float a_TextureSlot; // Texture Unit or -1 for no texture
layout(location = 3) in float a_TextureFilter; // Linear / Point
layout(location = 4) in vec2 a_TextureWrapping; // Repeat, MirroredRepeat, ...
layout(location = 5) in vec4 a_TextureBorderColor; // Used for ClampToBorder
layout(location = 6) in vec4 a_TextureRect; // 0 -> 1 Rect from Atlas
layout(location = 7) in vec2 a_TextureSize; // Texture size in pixels
layout(location = 8) in vec2 a_TextureCoord; // Basic UV Coord

flat out vec4 v_Color;
flat out int v_TextureSlot;
flat out int v_TextureFilter;
flat out int v_TextureSWrapping;
flat out int v_TextureTWrapping;
flat out vec4 v_TextureBorderColor;
flat out vec4 v_TextureRect;
flat out vec2 v_TextureSize;
out vec2 v_TextureCoord;

void main()
{
	gl_Position = a_Position;
	v_Color = a_Color;
	v_TextureSlot = int(a_TextureSlot);
	v_TextureSWrapping = int(a_TextureWrapping.x);
	v_TextureTWrapping = int(a_TextureWrapping.y);
	v_TextureFilter = int(a_TextureFilter);
	v_TextureBorderColor = a_TextureBorderColor;
	v_TextureRect = a_TextureRect;
	v_TextureSize = a_TextureSize;
	v_TextureCoord = vec2(a_TextureCoord.x, 1 - a_TextureCoord.y);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

uniform sampler2D[32] u_Slots;

flat in vec4 v_Color;
flat in int v_TextureSlot;
flat in int v_TextureFilter;
flat in int v_TextureSWrapping;
flat in int v_TextureTWrapping;
flat in vec4 v_TextureBorderColor;
flat in vec4 v_TextureRect;
flat in vec2 v_TextureSize;
in vec2 v_TextureCoord;

vec4 PointSampleFromAtlas(sampler2D slot, vec2 coord);

vec4 BiLinearSampleFromAtlas(sampler2D slot, vec2 coord)
{
	bool mainCoordValid = coord.x >= 0 && coord.x <= 1 && coord.y >= 0 && coord.y <= 1;
	coord *= v_TextureSize;

	float xmin = coord.x;
	float xmax = xmin + 1.0;

	float ymin = coord.y;
	float ymax = ymin + 1.0;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	vec2 crd1 = vec2(xmin, ymin) / v_TextureSize;
	vec2 crd2 = vec2(xmax, ymin) / v_TextureSize;

	bool crd1Valid = true, crd2Valid = true;

	if (mainCoordValid)
	{
		if (crd1.x <= 0 || crd1.x >= 1 || crd1.y <= 0 || crd1.y >= 1)
			crd1Valid = false;

		if (crd2.x <= 0 || crd2.x >= 1 || crd2.y <= 0 || crd2.y >= 1)
			crd2Valid = false;
	}
	
	vec4 TopMix;
	bool topMixValid = crd1Valid || crd2Valid;
	if (!mainCoordValid || (crd1Valid && crd2Valid))
		TopMix = mix(
			PointSampleFromAtlas(slot, crd1),
			PointSampleFromAtlas(slot, crd2),
			fract(coord.x)
		);
	else if (crd1Valid)
		TopMix = PointSampleFromAtlas(slot, crd1);
	else if (crd2Valid)
		TopMix = PointSampleFromAtlas(slot, crd2);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	crd1Valid = true;
	crd2Valid = true;

	crd1 = vec2(xmin, ymax) / v_TextureSize;
	crd2 = vec2(xmax, ymax) / v_TextureSize;

	if (mainCoordValid)
	{
		if (crd1.x <= 0 || crd1.x >= 1 || crd1.y <= 0 || crd1.y >= 1)
			crd1Valid = false;
		
		if (crd2.x <= 0 || crd2.x >= 1 || crd2.y <= 0 || crd2.y >= 1)
			crd2Valid = false;
	}
	
	vec4 DownMix;
	bool downMixValid = crd1Valid || crd2Valid;
	if (!mainCoordValid || (crd1Valid && crd2Valid))
		DownMix = mix(
			PointSampleFromAtlas(slot, crd1),
			PointSampleFromAtlas(slot, crd2),
			fract(coord.x)
		);
	else if (crd1Valid)
		DownMix = PointSampleFromAtlas(slot, crd1);
	else if (crd2Valid)
		DownMix = PointSampleFromAtlas(slot, crd2);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	if (topMixValid && downMixValid)
		return mix(TopMix, DownMix, fract(coord.y));
	else if (topMixValid)
		return TopMix;
	else if (downMixValid)
		return DownMix;

	return vec4(0.0, 0.0, 1.0, 1.0);
}

float Wrap(float value, int wrapping)
{
	int valueInt = int(value);
	bool valueIsRound = value == valueInt;
	int valueIntMinusOne = valueInt - 1;
	int valueIntMinusTwo = valueInt - 2;
	int commonExpr1 = valueIsRound ? valueIntMinusOne : valueInt;
	int commonExpr2 = valueIsRound ? valueIntMinusTwo : valueIntMinusOne;

	switch (wrapping)
	{
	case 1: // Repeat
		if (value > 1)
			value -= commonExpr1;
		else if (value < 0)
			value -= commonExpr2;
		break;
	case 2: // MirroredRepeat
		bool commonCondition = valueInt % 2 == 0;
		if (value > 1)
		{
			if (commonCondition)
				value -= commonExpr1;
			else
				value = (valueIsRound ? valueInt : valueInt + 1) - value;
		}
		else if (value < 0)
		{
			if (commonCondition)
				value = commonExpr1 - value;
			else
				value -= commonExpr2;
		}
		break;
	case 3: // ClampToEdge
		value = clamp(value, 0.001, 0.999);
		break;
	}

	return value;
}

vec4 PointSampleFromAtlas(sampler2D slot, vec2 coord)
{
	if ((v_TextureSWrapping == 4 && (coord.x > 1 || coord.x < 0)) || (v_TextureTWrapping == 4 && (coord.y > 1 || coord.y < 0)))
		return v_TextureBorderColor; // ClampToBorder

	coord.x = Wrap(coord.x, v_TextureSWrapping);
	coord.y = Wrap(coord.y, v_TextureTWrapping);
	return texture(slot, vec2(v_TextureRect.x + coord.x * v_TextureRect.z, v_TextureRect.y + coord.y * v_TextureRect.w));
}

vec4 Sample(sampler2D slot)
{
	if (v_TextureFilter == 1)
	return    PointSampleFromAtlas(slot, v_TextureCoord) * v_Color;
	return BiLinearSampleFromAtlas(slot, v_TextureCoord) * v_Color;
}

void main()
{
	switch (v_TextureSlot)
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
	case  10: o_Color = Sample(u_Slots[10]); return;
	case  11: o_Color = Sample(u_Slots[11]); return;
	case  12: o_Color = Sample(u_Slots[12]); return;
	case  13: o_Color = Sample(u_Slots[13]); return;
	case  14: o_Color = Sample(u_Slots[14]); return;
	case  15: o_Color = Sample(u_Slots[15]); return;
	case  16: o_Color = Sample(u_Slots[16]); return;
	case  17: o_Color = Sample(u_Slots[17]); return;
	case  18: o_Color = Sample(u_Slots[18]); return;
	case  19: o_Color = Sample(u_Slots[19]); return;
	case  20: o_Color = Sample(u_Slots[20]); return;
	case  21: o_Color = Sample(u_Slots[21]); return;
	case  22: o_Color = Sample(u_Slots[22]); return;
	case  23: o_Color = Sample(u_Slots[23]); return;
	case  24: o_Color = Sample(u_Slots[24]); return;
	case  25: o_Color = Sample(u_Slots[25]); return;
	case  26: o_Color = Sample(u_Slots[26]); return;
	case  27: o_Color = Sample(u_Slots[27]); return;
	case  28: o_Color = Sample(u_Slots[28]); return;
	case  29: o_Color = Sample(u_Slots[29]); return;
	case  30: o_Color = Sample(u_Slots[30]); return;
	case  31: o_Color = Sample(u_Slots[31]); return;
	}
}
