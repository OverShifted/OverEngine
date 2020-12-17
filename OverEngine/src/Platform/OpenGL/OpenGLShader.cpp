#include "pcheader.h"
#include "OpenGLShader.h"

#include "OverEngine/Core/FileSystem/FileSystem.h"

#include <glad/gl.h>

namespace OverEngine
{
	static GLenum ShaderTypeFromString(const String& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;
		if (type == "geometry")
			return GL_GEOMETRY_SHADER;

		OE_CORE_ASSERT(false, "Unknown shader type '{0}'!", type);
		return 0;
	}

	OpenGLShader::OpenGLShader(const String& filePath)
		: m_FilePath(filePath)
	{
		Compile(PreProcess(FileSystem::ReadFile(filePath)));

		auto lastSlash = filePath.find_last_of("/\\");
		lastSlash = lastSlash == String::npos ? 0 : lastSlash + 1;
		auto lastDot = filePath.rfind('.');
		auto count = lastDot == String::npos ? filePath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filePath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const String& name, const String& vertexSrc, const String& fragmentSrc)
		: m_Name(name)
	{
		UnorderedMap<GLenum, const char*> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc.c_str();
		sources[GL_FRAGMENT_SHADER] = fragmentSrc.c_str();
		Compile(sources);
	}

	OpenGLShader::OpenGLShader(const String& name, const char* vertexSrc, const char* fragmentSrc)
		: m_Name(name)
	{
		UnorderedMap<GLenum, const char*> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	UnorderedMap<GLenum, String> OpenGLShader::PreProcess(const String& source)
	{
		UnorderedMap<GLenum, String> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); // Start of shader type declaration line
		while (pos != String::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); // End of shader type declaration line
			OE_CORE_ASSERT(eol != String::npos, "Syntax error!");
			size_t begin = pos + typeTokenLength + 1; // Start of shader type name (after "#type " keyword)
			String type = source.substr(begin, eol - begin);
			OE_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified!");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); // Start of shader code after shader type declaration line
			OE_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); // Start of next shader type declaration line
			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const UnorderedMap<GLenum, String>& shaderSources)
	{
		UnorderedMap<GLenum, const char*> sources;
		for (auto& src : shaderSources)
			sources[src.first] = src.second.c_str();
		Compile(sources);
	}

	void OpenGLShader::Compile(const UnorderedMap<GLenum, const char*>& shaderSources)
	{
		GLuint program = glCreateProgram();

		OE_CORE_ASSERT(shaderSources.size() <= 3, "{0} shader sources got but 3 is maximim", shaderSources.size());

		std::array<GLint, 3> glShaderIDs{ -1, -1, -1 };
		int glShaderIdIndex = 0;
		bool allCompiled = true;

		for (auto& src : shaderSources)
		{
			GLenum type = src.first;
			const char* source = src.second;

			GLuint shader = glCreateShader(type);

			glShaderSource(shader, 1, &source, nullptr);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				Vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog.data());

				glDeleteShader(shader);
				glDeleteProgram(program);

				OE_THROW("Shader compilation failure! {}", infoLog.data());
				allCompiled = false;

				break;
			}

			glAttachShader(program, shader);

			glShaderIDs[glShaderIdIndex++] = shader;
		}

		if (allCompiled)
		{
			m_RendererID = program;

			glLinkProgram(program);

			GLint isLinked = 0;
			glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
			if (isLinked == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				Vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());

				glDeleteProgram(program);

				for (auto id : glShaderIDs)
					glDeleteShader(id);

				OE_CORE_ERROR("{0}", infoLog.data());
				OE_CORE_ASSERT(false, "Shader link failure!");
				return;
			}

			for (auto id : glShaderIDs)
			{
				if (id != -1)
				{
					glDetachShader(program, (GLint)id);
					glDeleteShader((GLint)id);
				}
			}
		}
	}

	GLint OpenGLShader::GetUniformLocation(const String& name) const
	{
		if (m_UniformLocationCache.count(name))
			return m_UniformLocationCache[name];

		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		m_UniformLocationCache[name] = location;
		return location;
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::UploadUniformInt(const char* name, int value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const char* name, const int* value, int count)
	{
		GLint location = GetUniformLocation(name);
		glUniform1iv(location, count, value);
	}

	void OpenGLShader::UploadUniformFloat(const char* name, float value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const char* name, const Vector2& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const char* name, const Vector3& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const char* name, const Vector4& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(const char* name, const Mat3x3& matrix)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const char* name, const Mat4x4& matrix)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	bool OpenGLShader::Reload(String filePath)
	{
		if (filePath.empty())
		{
			if (m_FilePath.empty())
				return false;

			Compile(PreProcess(FileSystem::ReadFile(m_FilePath)));
			return true;
		}

		Compile(PreProcess(FileSystem::ReadFile(filePath)));
		return true;
	}

	bool OpenGLShader::Reload(const String& vertexSrc, const String& fragmentSrc)
	{
		UnorderedMap<GLenum, const char*> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc.c_str();
		sources[GL_FRAGMENT_SHADER] = fragmentSrc.c_str();
		Compile(sources);
		return true;
	}

	bool OpenGLShader::Reload(const char* vertexSrc, const char* fragmentSrc)
	{
		UnorderedMap<GLenum, const char*> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
		return true;
	}
}
