#pragma once

#include "OverEngine/Renderer/Shader.h"

#include <glad/gl.h>

namespace OverEngine
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const String& filePath);
		OpenGLShader(const String& name, const String& vertexSrc, const String& fragmentSrc);
		OpenGLShader(const String& name, const Ref<IntermediateShader>& vertexShader, const Ref<IntermediateShader>& fragmentShader);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const String& GetName() const override { return m_Name; }

		virtual void UploadUniformInt(const String& name, int value) override;
		virtual void UploadUniformIntArray(const String& name, int* value, int count) override;

		virtual void UploadUniformFloat(const String& name, float value) override;
		virtual void UploadUniformFloat2(const String& name, const Math::Vector2& value) override;
		virtual void UploadUniformFloat3(const String& name, const Math::Vector3& value) override;
		virtual void UploadUniformFloat4(const String& name, const Math::Vector4& value) override;

		virtual void UploadUniformMat3(const String& name, const Math::Mat3x3& matrix) override;
		virtual void UploadUniformMat4(const String& name, const Math::Mat4x4& matrix) override;
	private:
		UnorderedMap<GLenum, String> PreProcess(const String& source);
		void Compile(const UnorderedMap<GLenum, String>& shaderSources);
	private:
		uint32_t m_RendererID = 0;
		String m_Name;
	};
}