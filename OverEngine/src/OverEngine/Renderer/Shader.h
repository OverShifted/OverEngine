#pragma once

#include "OverEngine/Core/Core.h"

namespace OverEngine
{
	class IntermediateShader;

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const String& GetName() const = 0;

		virtual void UploadUniformInt(const String& name, int value) = 0;
		virtual void UploadUniformIntArray(const String& name, const int* value, int count) = 0;

		virtual void UploadUniformFloat(const String& name, float value) = 0;
		virtual void UploadUniformFloat2(const String& name, const Math::Vector2& value) = 0;
		virtual void UploadUniformFloat3(const String& name, const Math::Vector3& value) = 0;
		virtual void UploadUniformFloat4(const String& name, const Math::Vector4& value) = 0;

		virtual void UploadUniformMat3(const String& name, const Math::Mat3x3& matrix) = 0;
		virtual void UploadUniformMat4(const String& name, const Math::Mat4x4& matrix) = 0;

		static Ref<Shader> Create(const String& filePath);
		static Ref<Shader> Create(const String& name, const String& vertexSrc, const String& fragmentSrc);
		static Ref<Shader> Create(const String& name, const char* vertexSrc, const char* fragmentSrc);
		static Ref<Shader> Create(const String& name, Ref<IntermediateShader>& vertexShader ,const Ref<IntermediateShader>& fragmentShader);
	};

	class IntermediateShader
	{
	public:
		enum class Type { Vertex, Fragment };
	public:
		virtual uint32_t GetRendererID() const = 0;
	public:
		static IntermediateShader* Create(String& Source, Type type);
	};

	class ShaderLibrary
	{
	public:
		void Add(const String& name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);
		Ref<Shader> Load(const String& filePath);
		Ref<Shader> Load(const String& name, const String& filePath);

		Ref<Shader> Get(const String& name);

		bool Exists(const String& name) const;
	private:
		UnorderedMap<String, Ref<Shader>> m_Shaders;
	};
}