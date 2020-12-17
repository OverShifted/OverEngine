#include "pcheader.h"
#include "OpenGLTexture.h"

#include <glad/gl.h>
#include <stb_image.h>

namespace OverEngine
{
	namespace GAPI
	{
		static GLint GetOpenGLFilteringValue(const TextureFiltering& wrapping)
		{
			switch (wrapping)
			{
			case TextureFiltering::Nearest: return GL_NEAREST;
			case TextureFiltering::Linear:  return GL_LINEAR;
			}

			return GL_NEAREST;
		}

		static TextureFormat GetTextureFormat(int channels)
		{
			switch (channels)
			{
			case 3: return TextureFormat::RGB;
			case 4: return TextureFormat::RGBA;
			}

			return TextureFormat::None;
		}

		static std::pair<GLenum, GLenum> GetOpenGLTextureFormatValue(const TextureFormat& wrapping)
		{
			GLenum internalFormat = 0;
			GLenum dataFormat = 0;
			
			if (wrapping == TextureFormat::RGB)
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}
			else if (wrapping == TextureFormat::RGBA)
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}

			return { internalFormat, dataFormat };
		}

		OpenGLTexture2D::OpenGLTexture2D(const String& path, TextureFiltering minFilter, TextureFiltering magFilter)
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

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GetOpenGLFilteringValue(minFilter));
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GetOpenGLFilteringValue(magFilter));

			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}

		OpenGLTexture2D::OpenGLTexture2D()
			: m_Width(0), m_Height(0),
			m_MinFilter(TextureFiltering::Linear), m_MagFilter(TextureFiltering::Linear),
			m_SWrapping(TextureWrapping::Repeat), m_TWrapping(TextureWrapping::Repeat),
			m_BorderColor(1.0f), m_Format(TextureFormat::None)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		}

		OpenGLTexture2D::~OpenGLTexture2D()
		{
			glDeleteTextures(1, &m_RendererID);
		}

		static GLint GetOpenGLWrappingValue(const TextureWrapping& wrapping)
		{
			switch (wrapping)
			{
			case TextureWrapping::Repeat:        return GL_REPEAT;
			case TextureWrapping::Clamp:         return GL_CLAMP_TO_EDGE;
			case TextureWrapping::Mirror:        return GL_MIRRORED_REPEAT;
			case TextureWrapping::ClampToBorder: return GL_CLAMP_TO_BORDER;
			}

			return GL_REPEAT;
		}

		void OpenGLTexture2D::SetSWrapping(TextureWrapping wrapping)
		{
			m_SWrapping = wrapping;
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GetOpenGLWrappingValue(wrapping));
		}

		void OpenGLTexture2D::SetTWrapping(TextureWrapping wrapping)
		{
			m_TWrapping = wrapping;
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GetOpenGLWrappingValue(wrapping));
		}

		void OpenGLTexture2D::SetBorderColor(const Color& color)
		{
			m_BorderColor = color;
			glTextureParameterfv(m_RendererID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
		}

		void OpenGLTexture2D::SetMinFilter(TextureFiltering filter)
		{
			m_MinFilter = filter;
			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GetOpenGLFilteringValue(filter));
		}

		void OpenGLTexture2D::SetMagFilter(TextureFiltering filter)
		{
			m_MagFilter = filter;
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GetOpenGLFilteringValue(filter));
		}

		void OpenGLTexture2D::Bind(uint32_t slot /*= 0*/) const
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

			SetTWrapping(m_TWrapping);
			SetSWrapping(m_SWrapping);

			SetBorderColor(m_BorderColor);
		}

		void OpenGLTexture2D::SubImage(const uint8_t* pixels, uint32_t width, uint32_t height, TextureFormat dataFormat, int xOffset /*= 0*/, int yOffset /*= 0*/)
		{
			auto [internalFormat, GLdataFormat] = GetOpenGLTextureFormatValue(dataFormat);
			glTextureSubImage2D(m_RendererID, 0, xOffset, yOffset, width, height, GLdataFormat, GL_UNSIGNED_BYTE, pixels);
		}
	}
}
