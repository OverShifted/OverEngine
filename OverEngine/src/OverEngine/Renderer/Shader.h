#pragma once

#include "OverEngine/Core/Core.h"

namespace OverEngine
{
	class Shader
	{
	public:
		static Ref<Shader> Create(const String& filePath);
		static Ref<Shader> Create(const String& name, const String& vertexSrc, const String& fragmentSrc);
		static Ref<Shader> Create(const String& name, const char* vertexSrc, const char* fragmentSrc);

		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const String& GetName() const = 0;

		virtual void UploadUniformInt(const char* name, int value) = 0;
		virtual void UploadUniformIntArray(const char* name, const int* value, int count) = 0;

		virtual void UploadUniformFloat(const char* name, float value) = 0;
		virtual void UploadUniformFloat2(const char* name, const Math::Vector2& value) = 0;
		virtual void UploadUniformFloat3(const char* name, const Math::Vector3& value) = 0;
		virtual void UploadUniformFloat4(const char* name, const Math::Vector4& value) = 0;

		virtual void UploadUniformMat3(const char* name, const Math::Mat3x3& matrix) = 0;
		virtual void UploadUniformMat4(const char* name, const Math::Mat4x4& matrix) = 0;

		virtual bool Reload(const String& filePath = String()) = 0;
		virtual bool Reload(const String& vertexSrc, const String& fragmentSrc) = 0;
		virtual bool Reload(const char* vertexSrc, const char* fragmentSrc) = 0;
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
