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

			virtual void UploadUniformInt(const std::string& name, int value) override;

			virtual void UploadUniformFloat(const std::string& name, float value) override;
			virtual void UploadUniformFloat2(const std::string& name, const Math::Vector2& value) override;
			virtual void UploadUniformFloat3(const std::string& name, const Math::Vector3& value) override;
			virtual void UploadUniformFloat4(const std::string& name, const Math::Vector4& value) override;

			virtual void UploadUniformMat3(const std::string& name, const Math::Mat3x3& matrix) override;
			virtual void UploadUniformMat4(const std::string& name, const Math::Mat4x4& matrix) override;
		private:
			uint32_t m_RendererID;
		};
	}
}