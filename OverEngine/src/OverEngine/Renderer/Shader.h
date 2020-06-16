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

			virtual void UploadUniformInt(const std::string& name, int value) = 0;

			virtual void UploadUniformFloat(const std::string& name, float value) = 0;
			virtual void UploadUniformFloat2(const std::string& name, const Math::Vector2& value) = 0;
			virtual void UploadUniformFloat3(const std::string& name, const Math::Vector3& value) = 0;
			virtual void UploadUniformFloat4(const std::string& name, const Math::Vector4& value) = 0;

			virtual void UploadUniformMat3(const std::string& name, const Math::Mat3x3& matrix) = 0;
			virtual void UploadUniformMat4(const std::string& name, const Math::Mat4x4& matrix) = 0;

			static Shader* Create(String& vertexSrc, String& fragmentSrc);
		};
	}
}