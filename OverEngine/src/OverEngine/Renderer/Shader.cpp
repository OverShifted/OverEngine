#include "pcheader.h"
#include "Shader.h"

#include "OverEngine/Core/Core.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLIntermediateShader.h"

namespace OverEngine
{
	/////////////////////////////////////////////////////////////////////////////
	// Shader ///////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	Ref<Shader> Shader::Create(const String& filePath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLShader>(filePath);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const String& name, const String& vertexSrc, const String& fragmentSrc)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const String& name, const char* vertexSrc, const char* fragmentSrc)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const String& name, Ref<IntermediateShader>& vertexShader, const Ref<IntermediateShader>& fragmentShader)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLShader>(name, vertexShader, fragmentShader);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	/////////////////////////////////////////////////////////////////////////////
	// IntermediateShader ///////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	IntermediateShader* IntermediateShader::Create(String& Source, Type type)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return new OpenGLIntermediateShader(Source, type);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	/////////////////////////////////////////////////////////////////////////////
	// ShaderLibrary ////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	void ShaderLibrary::Add(const String& name, const Ref<Shader>& shader)
	{
		OE_CORE_ASSERT(!Exists(name), "Shader '{0}' already exists!", name);
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const String& filePath)
	{
		Ref<Shader> shader = Shader::Create(filePath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const String& name, const String& filePath)
	{
		Ref<Shader> shader = Shader::Create(filePath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const String& name)
	{
		OE_CORE_ASSERT(Exists(name), "Shader '{0}' not found!", name);
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const String& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}

}