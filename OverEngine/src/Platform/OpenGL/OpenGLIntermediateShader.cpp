#include "pcheader.h"
#include "OpenGLIntermediateShader.h"

#include <glad/gl.h>

namespace OverEngine
{

	static GLenum ShaderTypeFromEnum(IntermediateShader::Type type)
	{
		if (type == IntermediateShader::Type::Vertex)
			return GL_VERTEX_SHADER;
		if (type == IntermediateShader::Type::Fragment)
			return GL_FRAGMENT_SHADER;

		OE_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	OpenGLIntermediateShader::OpenGLIntermediateShader(String& Source, Type type)
	{
		GLenum glType = ShaderTypeFromEnum(type);
		m_RendererID = glCreateShader(glType);

		const GLchar* sourceCStr = Source.c_str();
		glShaderSource(m_RendererID, 1, &sourceCStr, 0);

		glCompileShader(m_RendererID);

		GLint isCompiled = 0;
		glGetShaderiv(m_RendererID, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			Vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(m_RendererID);

			OE_CORE_ERROR("{0}", infoLog.data());
			OE_CORE_ASSERT(false, "Shader compilation failure!");
			return;
		}
	}

	OpenGLIntermediateShader::~OpenGLIntermediateShader()
	{
		glDeleteShader(m_RendererID);
	}

}