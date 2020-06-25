#pragma once

#include "OverEngine/Renderer/Texture.h"

namespace OverEngine
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const String& path, Filtering minFilter, Filtering magFilter);
		virtual ~OpenGLTexture2D();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }

		inline virtual Filtering GetMinFilter() const override { return m_MinFilter; };
		inline virtual Filtering GetMagFilter() const override { return m_MagFilter; };

		virtual void SetMinFilter(Filtering filter) override;
		virtual void SetMagFilter(Filtering filter) override;

		inline virtual Type GetType() const override { return m_Type; }

		virtual void Bind(uint32_t slot = 0) const override;

	private:
		String m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		Filtering m_MinFilter, m_MagFilter;
		Type m_Type;
	};
}