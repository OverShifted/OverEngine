#include "pcheader.h"
#include "OpenGLTexture.h"

#include <glad/gl.h>
#include <stb_image.h>

namespace OverEngine
{

	OpenGLTexture2D::OpenGLTexture2D(const String& path, Filtering minFilter, Filtering magFilter)
		: m_Path(path), m_MinFilter(minFilter), m_MagFilter(magFilter), m_Type(Type::None)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		OE_CORE_ASSERT(data, "Failde to load image!");
		m_Width = width;
		m_Height = height;

		GLenum dataFormat = 0, internalFormat = 0;
		if (channels == 3)
		{
			m_Type = Type::RGB;
			dataFormat = GL_RGB;
			internalFormat = GL_RGB8;
		}
		else if (channels == 4)
		{
			m_Type = Type::RGBA;
			dataFormat = GL_RGBA;
			internalFormat = GL_RGBA8;
		}
		OE_CORE_ASSERT(dataFormat & internalFormat, "Unsupported image format");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter == Texture2D::Filtering::Linear ? GL_LINEAR : GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, magFilter == Texture2D::Filtering::Linear ? GL_LINEAR : GL_NEAREST);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetMinFilter(Filtering filter)
	{
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, filter == Texture2D::Filtering::Linear ? GL_LINEAR : GL_NEAREST);
	}

	void OpenGLTexture2D::SetMagFilter(Filtering filter)
	{
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, filter == Texture2D::Filtering::Linear ? GL_LINEAR : GL_NEAREST);

	}

	void OpenGLTexture2D::Bind(uint32_t slot /*= 0*/) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

}