#pragma once

#include "OverEngine/Renderer/Texture.h"

namespace OverEngine
{
	namespace GAPI
	{
		class OpenGLTexture2D : public Texture2D
		{
		public:
			OpenGLTexture2D(const String& path, TextureFiltering minFilter, TextureFiltering magFilter);
			OpenGLTexture2D();
			virtual ~OpenGLTexture2D();

			inline virtual uint32_t GetWidth() const override { return m_Width; }
			inline virtual uint32_t GetHeight() const override { return m_Height; }

			inline virtual TextureWrapping GetSWrapping() const override { return m_SWrapping; }
			inline virtual TextureWrapping GetTWrapping() const override { return m_TWrapping; }
			inline virtual const Color& GetBorderColor() const override { return m_BorderColor; }


			virtual void SetSWrapping(TextureWrapping wrapping) override;
			virtual void SetTWrapping(TextureWrapping wrapping) override;
			virtual void SetBorderColor(const Color& color) override;

			inline virtual TextureFiltering GetMinFilter() const override { return m_MinFilter; };
			inline virtual TextureFiltering GetMagFilter() const override { return m_MagFilter; };

			virtual void SetMinFilter(TextureFiltering filter) override;
			virtual void SetMagFilter(TextureFiltering filter) override;

			inline virtual TextureFormat GetFormat() const override { return m_Format; }

			virtual void Bind(uint32_t slot = 0) const override;
			inline virtual uint32_t GetRendererID() const { return m_RendererID; }

			inline virtual bool operator==(const Texture& other) const override
			{
				return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
			}

			virtual void AllocateStorage(TextureFormat format, uint32_t width, uint32_t height) override;
			virtual void SubImage(const uint8_t* image, uint32_t width, uint32_t height, TextureFormat dataFormat, int xOffset = 0, int yOffset = 0) override;

			inline virtual Vector<std::weak_ptr<::OverEngine::Texture2D>>& GetMemberTextures() override { return m_Members; }
		private:
			uint32_t m_Width, m_Height;
			TextureFiltering m_MinFilter, m_MagFilter;
			TextureWrapping m_SWrapping, m_TWrapping;
			Color m_BorderColor;
			TextureFormat m_Format;
			uint32_t m_RendererID = 0;
			Vector<std::weak_ptr<::OverEngine::Texture2D>> m_Members;
		};
	}
}
