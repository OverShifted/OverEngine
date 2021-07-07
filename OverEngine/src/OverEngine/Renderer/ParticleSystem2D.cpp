#include "pcheader.h"
#include "ParticleSystem2D.h"

#include "OverEngine/Renderer/Renderer2D.h"
#include "OverEngine/Core/Random.h"

namespace OverEngine
{
	ParticleSystem2D::ParticleSystem2D(uint32_t poolSize)
		: m_NextParticleIndex(poolSize - 1)
	{
		m_ParticlePool.resize(poolSize);
	}

	void ParticleSystem2D::Emit(const Particle2DProps& props)
	{
		Particle& particle = m_ParticlePool[m_NextParticleIndex];

		particle.Active = true;

		particle.Position = props.Position;
		particle.Rotation = props.Rotation;

		particle.Velocity        = props.Velocity        + props.VelocityVariation        * Vector2(Random::Range(-1.0f, 1.0f), Random::Range(-1.0f, 1.0f));
		particle.AngularVelocity = props.AngularVelocity + props.AngularVelocityVariation * Random::Range(-1.0f, 1.0f);

		particle.RenderingProps = props.RenderingProps;

		particle.BirthSize = props.BirthSize;
		particle.DeathSize = props.DeathSize;

		particle.LifeTime = particle.LifeLeft = props.LifeTime;

		m_NextParticleIndex = ++m_NextParticleIndex % (uint32_t)m_ParticlePool.size();
	}

	void ParticleSystem2D::UpdateAndRender(TimeStep deltaTime, const Mat4x4& viewMatrix, const Camera& camera, bool useDepthTesting)
	{
		bool depthTestWasEnabled = RenderCommand::IsDepthTestingEnabled();
		bool badDepthTestingState = useDepthTesting != depthTestWasEnabled;

		if (badDepthTestingState)
			RenderCommand::SetDepthTesting(useDepthTesting);

		Renderer2D::BeginScene(viewMatrix, camera);

		float depth = 0.0f;
		float leftLifeRatio = 0.0f;

		for (auto& particle : m_ParticlePool)
		{
			if (!particle.Active)
				continue;

			// Process Life
			particle.LifeLeft -= deltaTime;
			if (particle.LifeLeft <= 0.0f)
			{
				particle.Active = false;
				continue;
			}

			// Move Particle
			particle.Position.x += particle.Velocity.x * deltaTime;
			particle.Position.y += particle.Velocity.y * deltaTime;
			particle.Rotation += particle.AngularVelocity * deltaTime;

			if (useDepthTesting)
				depth += 0.0001f;

			leftLifeRatio = particle.LifeLeft / particle.LifeTime; // 1 = New, 0 = Old
			Color color = glm::mix(particle.RenderingProps.DeathColor, particle.RenderingProps.BirthColor, leftLifeRatio);
			Vector2 size = glm::mix(particle.DeathSize, particle.BirthSize, leftLifeRatio);

			Vector3 position = particle.Position;
			position.z += depth;

			if (particle.RenderingProps.Sprite)
			{
				TexturedQuadProps props;
				props.Tint = color;
				props.Sprite = particle.RenderingProps.Sprite;
				props.Tiling = particle.RenderingProps.Tiling;
				props.Offset = particle.RenderingProps.Offset;
				props.Flip = particle.RenderingProps.Flip;

				Renderer2D::DrawQuad(position, particle.Rotation, size, props);
			}
			else
			{
				Renderer2D::DrawQuad(position, particle.Rotation, size, color);
			}
		}

		Renderer2D::EndScene();

		if (badDepthTestingState)
			RenderCommand::SetDepthTesting(depthTestWasEnabled);
	}
}
