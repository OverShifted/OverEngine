#pragma once

namespace OverEngine
{
	class IntermediateShader;

	class Shader
	{
	public:
		virtual ~Shader() {};

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void UploadUniformInt(const String& name, int value) = 0;

		virtual void UploadUniformFloat(const String& name, float value) = 0;
		virtual void UploadUniformFloat2(const String& name, const Math::Vector2& value) = 0;
		virtual void UploadUniformFloat3(const String& name, const Math::Vector3& value) = 0;
		virtual void UploadUniformFloat4(const String& name, const Math::Vector4& value) = 0;

		virtual void UploadUniformMat3(const String& name, const Math::Mat3x3& matrix) = 0;
		virtual void UploadUniformMat4(const String& name, const Math::Mat4x4& matrix) = 0;

		static Shader* Create(const String& filePath);
		static Shader* Create(const String& vertexSrc, const String& fragmentSrc);
		static Shader* Create(const Ref<IntermediateShader>& vertexShader ,const Ref<IntermediateShader>& fragmentShader);
	};

	class IntermediateShader
	{
	public:
		enum class Type { Vertex, Fragment };
	public:
		virtual void* GetRendererID() const = 0;
	public:
		static IntermediateShader* Create(String& Source, Type type);
	};
}