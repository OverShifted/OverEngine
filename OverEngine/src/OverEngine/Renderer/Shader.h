#pragma once

namespace OverEngine
{
	class Shader
	{
	public:
		Shader(String& vertexSrc, String& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

	private:
		uint32_t m_RendererID;
	};
}