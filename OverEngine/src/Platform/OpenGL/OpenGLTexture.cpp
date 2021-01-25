#include "pcheader.h"
#include "OpenGLTexture.h"

#include <glad/gl.h>
#include <stb_image.h>

namespace OverEngine
{
	static GLint GetOpenGLFilterValue(const TextureFilter& wrap)
	{
		switch (wrap)
		{
		case TextureFilter::Nearest: return GL_NEAREST;
		case TextureFilter::Linear:  return GL_LINEAR;
		}

		return GL_NEAREST;
	}

	static TextureFormat GetTextureFormat(int channels)
	{
		switch (channels)
		{
		case 3: return TextureFormat::RGB8;
		case 4: return TextureFormat::RGBA8;
		}

		return TextureFormat::None;
	}

	static std::pair<GLenum, GLenum> GetOpenGLTextureFormatValue(const TextureFormat& wrap)
	{
		GLenum internalFormat = 0;
		GLenum dataFormat = 0;
		
		if (wrap == TextureFormat::RGB8)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		else if (wrap == TextureFormat::RGBA8)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}

		return { internalFormat, dataFormat };
	}

	OpenGLTexture2D::OpenGLTexture2D(const String& path, TextureFilter minFilter, TextureFilter magFilter)
		: m_MinFilter(minFilter), m_MagFilter(magFilter), m_Format(TextureFormat::None)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		OE_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		m_Format = GetTextureFormat(channels);
		auto [internalFormat, dataFormat] = GetOpenGLTextureFormatValue(m_Format);

		OE_CORE_ASSERT(dataFormat & internalFormat, "Unsupported image format");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GetOpenGLFilterValue(minFilter));
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GetOpenGLFilterValue(magFilter));

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D()
		: m_Width(0), m_Height(0),
		m_MinFilter(TextureFilter::Linear), m_MagFilter(TextureFilter::Linear),
		m_UWrap(TextureWrap::Repeat), m_VWrap(TextureWrap::Repeat),
		m_Format(TextureFormat::None)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	static GLint GetOpenGLWrapValue(const TextureWrap& wrap)
	{
		switch (wrap)
		{
		case TextureWrap::Repeat:        return GL_REPEAT;
		case TextureWrap::Clamp:         return GL_CLAMP_TO_EDGE;
		case TextureWrap::Mirror:        return GL_MIRRORED_REPEAT;
		}

		return GL_REPEAT;
	}

	void OpenGLTexture2D::SetUWrap(TextureWrap wrap)
	{
		m_UWrap = wrap;
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GetOpenGLWrapValue(wrap));
	}

	void OpenGLTexture2D::SetVWrap(TextureWrap wrap)
	{
		m_VWrap = wrap;
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GetOpenGLWrapValue(wrap));
	}

	void OpenGLTexture2D::SetMinFilter(TextureFilter filter)
	{
		m_MinFilter = filter;
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GetOpenGLFilterValue(filter));
	}

	void OpenGLTexture2D::SetMagFilter(TextureFilter filter)
	{
		m_MagFilter = filter;
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GetOpenGLFilterValue(filter));
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture2D::AllocateStorage(TextureFormat format, uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		m_Format = format;

		auto [internalFormat, dataFormat] = GetOpenGLTextureFormatValue(m_Format);

		glDeleteTextures(1, &m_RendererID);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, width, height);

		uint32_t clear = 0;
		glClearTexImage(m_RendererID, 0, dataFormat, GL_UNSIGNED_BYTE, &clear);

		SetMinFilter(m_MinFilter);
		SetMagFilter(m_MagFilter);

		SetVWrap(m_VWrap);
		SetUWrap(m_UWrap);
	}

	void OpenGLTexture2D::SubImage(const uint8_t* pixels, uint32_t width, uint32_t height, TextureFormat dataFormat, int xOffset, int yOffset)
	{
		auto [internalFormat, GLdataFormat] = GetOpenGLTextureFormatValue(dataFormat);
		glTextureSubImage2D(m_RendererID, 0, xOffset, yOffset, width, height, GLdataFormat, GL_UNSIGNED_BYTE, pixels);
	}
}
