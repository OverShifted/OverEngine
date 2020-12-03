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
		OpenGLShader(const String& name, const char* vertexSrc, const char* fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const String& GetName() const override { return m_Name; }

		virtual void UploadUniformInt(const char* name, int value) override;
		virtual void UploadUniformIntArray(const char* name, const int* value, int count) override;

		virtual void UploadUniformFloat(const char* name, float value) override;
		virtual void UploadUniformFloat2(const char* name, const Vector2& value) override;
		virtual void UploadUniformFloat3(const char* name, const Vector3& value) override;
		virtual void UploadUniformFloat4(const char* name, const Vector4& value) override;

		virtual void UploadUniformMat3(const char* name, const Mat3x3& matrix) override;
		virtual void UploadUniformMat4(const char* name, const Mat4x4& matrix) override;

		virtual bool Reload(String filePath = String()) override;
		virtual bool Reload(const String& vertexSrc, const String& fragmentSrc) override;
		virtual bool Reload(const char* vertexSrc, const char* fragmentSrc) override;
	private:
		UnorderedMap<GLenum, String> PreProcess(const String& source);
		void Compile(const UnorderedMap<GLenum, String>& shaderSources);
		void Compile(const UnorderedMap<GLenum, const char*>& shaderSources);

		GLint GetUniformLocation(const String& name) const;
	private:
		uint32_t m_RendererID = 0;
		String m_Name;
		String m_FilePath;

		mutable UnorderedMap<String, GLint> m_UniformLocationCache;
	};
}
