#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Core/Time/TimeStep.h"
#include "OverEngine/Renderer/Camera.h"

#include "OverEngine/Renderer/Texture.h"
#include "OverEngine/Renderer/Renderer2D.h"

namespace OverEngine
{
	struct ParticleRenderingProps2D
	{
		Color BirthColor = Color(1.0f);
		Color DeathColor = Color(0.0f);

		Ref<Texture2D> Texture;

		Vector2 Tiling = Vector2(1.0f);
		Vector2 Offset = Vector2(0.0f);
		TextureFlip Flip = 0;

		Vec2T<TextureWrap> Wrap{ TextureWrap::None, TextureWrap::None };
		TextureFilter Filter = TextureFilter::None;
	};

	struct ParticleProps2D
	{
		// Transform
		Vector3 Position               = Vector3(0.0f); // Vector3 support's depth
		Vector2 Velocity               = Vector2(0.0f);
		Vector2 VelocityVariation      = Vector2(1.0f);

		float Rotation                 = 0.0f;
		float AngularVelocity          = 0.0f;
		float AngularVelocityVariation = 0.0f;

		Vector2 BirthSize = Vector2(0.0f);
		Vector2 DeathSize = Vector2(1.0f);

		// LifeTime in seconds
		float LifeTime = 1.0f;

		// Rendering
		ParticleRenderingProps2D RenderingProps;
	};

	class ParticleSystem2D
	{
	public:
		ParticleSystem2D(uint32_t poolSize = 100000);

		// Emit a single particle
		void Emit(const ParticleProps2D& props);

		void UpdateAndRender(TimeStep deltaTime, const Mat4x4& viewMatrix, const Camera& camera, bool useDepthTesting = true);

	private:

		struct Particle
		{
			Vector3 Position;
			Vector2 Velocity;

			float Rotation;
			float AngularVelocity;

			Vector2 BirthSize = Vector2(0.0f);
			Vector2 DeathSize = Vector2(1.0f);

			float LifeTime, LifeLeft;

			ParticleRenderingProps2D RenderingProps;

			bool Active;
		};

		Vector<Particle> m_ParticlePool;
		uint32_t m_NextParticleIndex;

		Ref<Texture2D> m_Sorosh;
	};
}
