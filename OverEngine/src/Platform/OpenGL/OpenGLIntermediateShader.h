#pragma once

#include "OverEngine/Renderer/Shader.h"

#include "OpenGLShader.h"

namespace OverEngine
{
	class OpenGLIntermediateShader : public IntermediateShader
	{
	public:
		OpenGLIntermediateShader(String& Source, Type type);
		virtual ~OpenGLIntermediateShader();

		virtual uint32_t GetRendererID() const override { return m_RendererID; }
	private:
		uint32_t m_RendererID;
	};
}