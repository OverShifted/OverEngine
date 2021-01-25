#pragma once

#include "OverEngine/Renderer/GPUTexture.h"

namespace OverEngine
{
	class OpenGLTexture2D : public GPUTexture2D
	{
	public:
		OpenGLTexture2D(const String& path, TextureFilter minFilter, TextureFilter magFilter);
		OpenGLTexture2D();
		virtual ~OpenGLTexture2D();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }

		inline virtual TextureWrap GetUWrap() const override { return m_UWrap; }
		inline virtual TextureWrap GetVWrap() const override { return m_VWrap; }

		virtual void SetUWrap(TextureWrap wrapping) override;
		virtual void SetVWrap(TextureWrap wrapping) override;

		inline virtual TextureFilter GetMinFilter() const override { return m_MinFilter; };
		inline virtual TextureFilter GetMagFilter() const override { return m_MagFilter; };

		virtual void SetMinFilter(TextureFilter filter) override;
		virtual void SetMagFilter(TextureFilter filter) override;

		inline virtual TextureFormat GetFormat() const override { return m_Format; }

		virtual void Bind(uint32_t slot = 0) const override;
		inline virtual uint32_t GetRendererID() const { return m_RendererID; }

		inline virtual bool operator==(const GPUTexture& other) const override
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}

		virtual void AllocateStorage(TextureFormat format, uint32_t width, uint32_t height) override;
		virtual void SubImage(const uint8_t* image, uint32_t width, uint32_t height, TextureFormat dataFormat, int xOffset = 0, int yOffset = 0) override;

		inline virtual Vector<std::weak_ptr<Texture2D>>& GetMemberTextures() override { return m_Members; }
	private:
		uint32_t m_RendererID = 0;

		TextureFormat m_Format;
		uint32_t m_Width, m_Height;

		TextureFilter m_MinFilter, m_MagFilter;
		TextureWrap m_UWrap, m_VWrap;

		Vector<std::weak_ptr<Texture2D>> m_Members;
	};
}
