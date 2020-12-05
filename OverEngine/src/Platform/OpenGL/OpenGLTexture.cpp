#include "pcheader.h"
#include "OpenGLTexture.h"

#include <glad/gl.h>
#include <stb_image.h>

namespace OverEngine
{
	namespace GAPI
	{
		OpenGLTexture2D::OpenGLTexture2D(const String& path, TextureFiltering minFilter, TextureFiltering magFilter)
			: m_MinFilter(minFilter), m_MagFilter(magFilter), m_Format(TextureFormat::None)
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
				m_Format = TextureFormat::RGB;
				dataFormat = GL_RGB;
				internalFormat = GL_RGB8;
			}
			else if (channels == 4)
			{
				m_Format = TextureFormat::RGBA;
				dataFormat = GL_RGBA;
				internalFormat = GL_RGBA8;
			}
			OE_CORE_ASSERT(dataFormat & internalFormat, "Unsupported image format");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter == TextureFiltering::Linear ? GL_LINEAR : GL_NEAREST);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, magFilter == TextureFiltering::Linear ? GL_LINEAR : GL_NEAREST);

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

		void OpenGLTexture2D::SetSWrapping(TextureWrapping wrapping)
		{
			GLint val = 0;
			if (wrapping == TextureWrapping::Repeat)
				val = GL_REPEAT;
			else if (wrapping == TextureWrapping::Mirror)
				val = GL_MIRRORED_REPEAT;
			else if (wrapping == TextureWrapping::Clamp)
				val = GL_CLAMP_TO_EDGE;
			else if (wrapping == TextureWrapping::ClampToBorder)
				val = GL_CLAMP_TO_BORDER;

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, val);
			m_SWrapping = wrapping;
		}

		void OpenGLTexture2D::SetTWrapping(TextureWrapping wrapping)
		{
			GLint val = 0;
			if (wrapping == TextureWrapping::Repeat)
				val = GL_REPEAT;
			else if (wrapping == TextureWrapping::Mirror)
				val = GL_MIRRORED_REPEAT;
			else if (wrapping == TextureWrapping::Clamp)
				val = GL_CLAMP_TO_EDGE;
			else if (wrapping == TextureWrapping::ClampToBorder)
				val = GL_CLAMP_TO_BORDER;

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, val);
			m_TWrapping = wrapping;
		}

		void OpenGLTexture2D::SetBorderColor(const Color& color)
		{
			glTextureParameterfv(m_RendererID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
			m_BorderColor = color;
		}

		void OpenGLTexture2D::SetMinFilter(TextureFiltering filter)
		{
			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, filter == TextureFiltering::Linear ? GL_LINEAR : GL_NEAREST);
			m_MinFilter = filter;
		}

		void OpenGLTexture2D::SetMagFilter(TextureFiltering filter)
		{
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, filter == TextureFiltering::Linear ? GL_LINEAR : GL_NEAREST);
			m_MagFilter = filter;
		}

		void OpenGLTexture2D::Bind(uint32_t slot /*= 0*/) const
		{
			glBindTextureUnit(slot, m_RendererID);
		}

		void OpenGLTexture2D::AllocateStorage(TextureFormat format, uint32_t width, uint32_t height)
		{
			GLenum internalFormat = 0;
			if (format == TextureFormat::RGB)
			{
				internalFormat = GL_RGB8;
			}
			else if (format == TextureFormat::RGBA)
			{
				internalFormat = GL_RGBA8;
			}

			m_Width = width;
			m_Height = height;
			m_Format = format;

			glDeleteTextures(1, &m_RendererID);
			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, internalFormat, width, height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, m_MinFilter == TextureFiltering::Linear ? GL_LINEAR : GL_NEAREST);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, m_MagFilter == TextureFiltering::Linear ? GL_LINEAR : GL_NEAREST);

			SetTWrapping(m_TWrapping);
			SetSWrapping(m_SWrapping);

			glTextureParameterfv(m_RendererID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(m_BorderColor));
		}

		void OpenGLTexture2D::SubImage(const unsigned char* pixels, uint32_t width, uint32_t height, TextureFormat dataFormat, int xOffset /*= 0*/, int yOffset /*= 0*/)
		{
			m_Format = dataFormat;

			GLenum GLdataFormat = 0;
			if (m_Format == TextureFormat::RGB)
			{
				GLdataFormat = GL_RGB;
			}
			else if (m_Format == TextureFormat::RGBA)
			{
				GLdataFormat = GL_RGBA;
			}
			glTextureSubImage2D(m_RendererID, 0, xOffset, yOffset, width, height, GLdataFormat, GL_UNSIGNED_BYTE, pixels);
		}
	}

}
