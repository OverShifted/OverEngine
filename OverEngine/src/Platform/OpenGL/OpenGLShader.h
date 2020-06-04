#pragma once

#include "OverEngine/Renderer/Shader.h"

namespace OverEngine
{
	namespace Renderer
	{
		class OpenGLShader : public Shader
		{
		public:
			OpenGLShader(String& vertexSrc, String& fragmentSrc);
			virtual ~OpenGLShader();

			virtual void Bind() const override;
			virtual void Unbind() const override;
		private:
			uint32_t m_RendererID;
		};
	}
}