#include "pcheader.h"
#include "OpenGLTexture.h"

#include <glad/gl.h>
#include <stb_image.h>

namespace OverEngine
{
	static TextureFormat GetFormatFromChannelCount(int channels)
	{
		switch (channels)
		{
		case 3: return TextureFormat::RGB8;
		case 4: return TextureFormat::RGBA8;

		default: return TextureFormat::None;
		}
	}

	static auto GetOpenGLDataAndInternalFormat(TextureFormat format)
	{
		struct _out
		{
			_out(GLenum internalFormat, GLenum dataFormat)
				: InternalFormat(internalFormat), DataFormat(dataFormat)
			{
			}

			GLenum InternalFormat;
			GLenum DataFormat;
		};

		switch (format)
		{
		case TextureFormat::RGB8: return _out{ GL_RGB8, GL_RGB };
		case TextureFormat::RGBA8: return _out{ GL_RGBA8, GL_RGBA };

		default: return _out{ 0, 0 };
		}
	}

	static GLenum GetOpenGLTextureFilter(TextureFilter filter)
	{
		switch (filter)
		{
		case TextureFilter::Nearest:  return GL_NEAREST;
		case TextureFilter::BiLinear: return GL_LINEAR;

		default: return 0;
		}
	}

	static GLenum GetOpenGLTextureWrap(TextureWrap wrap)
	{
		switch (wrap)
		{
		case TextureWrap::Repeat: return GL_REPEAT;
		case TextureWrap::Clamp:  return GL_CLAMP_TO_EDGE;
		case TextureWrap::Mirror: return GL_MIRRORED_REPEAT;

		default: return 0;
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(const String& path)
	{
		// Load image using stb_image
		int width, height, channels;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		OE_CORE_ASSERT(data, "Failed to load image at path '{}'! reason: '{}'", path, stbi_failure_reason());

		// Check channel count and format
		m_Format = GetFormatFromChannelCount(channels);
		OE_CORE_ASSERT(m_Format != TextureFormat::None, "Unsupported image format (channel count = {}).", channels);

		// Put values in members
		m_Width = width;
		m_Height = height;
		m_Filter = TextureFilter::BiLinear;
		m_Wrap = { TextureWrap::Repeat, TextureWrap::Repeat };

		// Upload image to GPU
		auto glFormat = GetOpenGLDataAndInternalFormat(m_Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, glFormat.InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, glFormat.DataFormat, GL_UNSIGNED_BYTE, data);

		// Free image buffer created by stb_image
		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(const uint64_t& guid)
	{
		SetGuid(guid);
	}

	void OpenGLTexture2D::Acquire(Ref<Asset> other)
	{
		if (auto otherGLTexture = std::dynamic_pointer_cast<OpenGLTexture2D>(other))
		{
			m_RendererID = otherGLTexture->m_RendererID;

			m_Width  = otherGLTexture->m_Width;
			m_Height = otherGLTexture->m_Height;
			m_Format = otherGLTexture->m_Format;
			m_Filter = otherGLTexture->m_Filter;
			m_Wrap   = otherGLTexture->m_Wrap;
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		if (m_RendererID != 0)
			glDeleteTextures(1, &m_RendererID);
	}

	uint32_t OpenGLTexture2D::GetWidth() const
	{
		return m_Width;
	}

	uint32_t OpenGLTexture2D::GetHeight() const
	{
		return m_Height;
	}

	uint32_t OpenGLTexture2D::GetRendererID() const
	{
		return m_RendererID;
	}

	void OpenGLTexture2D::Bind(uint32_t slot)
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	TextureFilter OpenGLTexture2D::GetFilter() const
	{
		return m_Filter;
	}

	void OpenGLTexture2D::SetFilter(TextureFilter filter)
	{
		m_Filter = filter;

		GLenum glFilter = GetOpenGLTextureFilter(filter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, glFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, glFilter);
	}

	TextureWrap OpenGLTexture2D::GetUWrap() const
	{
		return m_Wrap.u;
	}

	void OpenGLTexture2D::SetUWrap(TextureWrap wrap)
	{
		m_Wrap.u = wrap;
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GetOpenGLTextureWrap(wrap));
	}

	TextureWrap OpenGLTexture2D::GetVWrap() const
	{
		return m_Wrap.v;
	}

	void OpenGLTexture2D::SetVWrap(TextureWrap wrap)
	{
		m_Wrap.v = wrap;
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GetOpenGLTextureWrap(wrap));
	}

	TextureFormat OpenGLTexture2D::GetFormat() const
	{
		return m_Format;
	}

	TextureType OpenGLTexture2D::GetType() const
	{
		return TextureType::Master;
	}
}
