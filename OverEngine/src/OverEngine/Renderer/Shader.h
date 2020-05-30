#pragma once

namespace OverEngine
{
	class Shader
	{
	public:
		virtual ~Shader() {};

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Shader* Create(String& vertexSrc, String& fragmentSrc);
	};
}