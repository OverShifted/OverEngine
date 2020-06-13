#pragma once

namespace OverEngine
{
	namespace Renderer
	{
		class Shader
		{
		public:
			virtual ~Shader() {};

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual void UploadUniform(const String& name, const Math::Mat4x4& matrix) = 0;

			static Shader* Create(String& vertexSrc, String& fragmentSrc);
		};
	}
}