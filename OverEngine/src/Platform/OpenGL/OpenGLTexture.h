#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Renderer/Texture.h"

namespace OverEngine
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const String& path);
		OpenGLTexture2D(const uint64_t& guid);
		virtual void Acquire(Ref<Asset> other) override;

		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override;
		virtual uint32_t GetHeight() const override;
		virtual uint32_t GetRendererID() const override;

		virtual void Bind(uint32_t slot = 0) override;

		// Filter
		virtual TextureFilter GetFilter() const override;
		virtual void SetFilter(TextureFilter filter) override;

		// Wrap
		virtual TextureWrap GetUWrap() const override;
		virtual void SetUWrap(TextureWrap wrap) override;

		virtual TextureWrap GetVWrap() const override;
		virtual void SetVWrap(TextureWrap wrap) override;

		// Other
		virtual TextureFormat GetFormat() const override;
		virtual TextureType GetType() const override;

		// Asset
		virtual bool IsRefrence() const override { return m_RendererID == 0; }
	private:

		uint32_t m_RendererID = 0;

		uint32_t m_Width = 0, m_Height = 0;
		TextureFormat m_Format = TextureFormat::None;
		TextureFilter m_Filter = TextureFilter::None;
		Vec2T<TextureWrap> m_Wrap = { TextureWrap::None, TextureWrap::None };
	};
}
