#include "pcheader.h"
#include "OpenGLShader.h"

#include "OpenGLIntermediateShader.h"

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

		OE_CORE_ASSERT(false, "Unknown shader type '{0}'!", type);
		return 0;
	}

	OpenGLShader::OpenGLShader(const String& filePath)
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
		UnorderedMap<GLenum, String> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	OpenGLShader::OpenGLShader(const String& name, const Ref<IntermediateShader>& vertexShader, const Ref<IntermediateShader>& fragmentShader)
		: m_Name(name)
	{
		m_RendererID = glCreateProgram();

		uint32_t vsID = (uint32_t)(intptr_t)(vertexShader->GetRendererID());
		uint32_t fsID = (uint32_t)(intptr_t)(fragmentShader->GetRendererID());

		glAttachShader(m_RendererID, vsID);
		glAttachShader(m_RendererID, fsID);

		glLinkProgram(m_RendererID);

		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			Vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(m_RendererID);

			OE_CORE_ERROR("{0}", infoLog.data());
			OE_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		glDetachShader(m_RendererID, vsID);
		glDetachShader(m_RendererID, fsID);
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
		GLuint program = glCreateProgram();

		OE_CORE_ASSERT(shaderSources.size() <= 2, "{0} shader sources got but 2 is maximim", shaderSources.size());

		std::array<GLenum, 2> glShaderIDs;
		int glShaderIdIndex = 0;

		for (auto& src : shaderSources)
		{
			GLenum type = src.first;
			const String& source = src.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				Vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				OE_CORE_ERROR("{0}", infoLog.data());
				OE_CORE_ASSERT(false, "Shader compilation failure!");
				break;
			}

			glAttachShader(program, shader);
			
			glShaderIDs[glShaderIdIndex++] = shader;
		}

		m_RendererID = program;

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			Vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (auto id : glShaderIDs)
				glDeleteShader(id);

			OE_CORE_ERROR("{0}", infoLog.data());
			OE_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::UploadUniformInt(const String& name, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const String& name, int* value, int count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, value);
	}

	void OpenGLShader::UploadUniformFloat(const String& name, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const String& name, const Math::Vector2& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const String& name, const Math::Vector3& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const String& name, const Math::Vector4& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(const String& name, const Math::Mat3x3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const String& name, const Math::Mat4x4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

}