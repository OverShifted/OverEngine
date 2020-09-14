#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Renderer/GAPI/GTexture.h"
#include "OverEngine/Renderer/TextureEnums.h"

namespace OverEngine
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual TextureFiltering GetFilter() const = 0;

		virtual void SetFilter(TextureFiltering filter) = 0;

		virtual TextureWrapping GetSWrapping() const = 0;
		virtual TextureWrapping GetTWrapping() const = 0;

		virtual void SetSWrapping(TextureWrapping wrapping) = 0;
		virtual void SetTWrapping(TextureWrapping wrapping) = 0;

		virtual const Color& GetBorderColor() const = 0;
		virtual void SetBorderColor(const Color& color) = 0;

		virtual TextureFormat GetFormat() const = 0;
		virtual TextureType GetType() const = 0;

		virtual uint32_t GetRendererID() const = 0;
		virtual Rect GetRect() const = 0;
	};

	class Texture2D;
	class TextureManager;
	class Renderer2D;

	struct MasterTextureData
	{
		TextureFormat m_Format;

		TextureFiltering m_Filter;

		TextureWrapping m_SWrapping = TextureWrapping::Repeat;
		TextureWrapping m_TWrapping = TextureWrapping::Repeat;
		Color m_BorderColor;

		uint8_t* m_Pixels;

		Ref<GAPI::Texture2D> m_MappedTexture;
		Rect m_MappedTextureRect;
	};

	struct SubTextureData
	{
		Ref<Texture2D> m_Parent;
		Rect m_Rect;
	};

	class Texture2D : public Texture
	{
		friend class TextureManager;
		friend class Renderer2D;
	public:
		static Ref<Texture2D> MasterFromFile(const String& path);
		static Ref<Texture2D> SubTextureFromExistingOne(Ref<Texture2D> masterTexture, Rect rect);

		Texture2D(const String& path);
		Texture2D(Ref<Texture2D> masterTexture, Rect rect);
		virtual ~Texture2D();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }

		virtual TextureFiltering GetFilter() const override
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.m_Filter;
			return m_SubTextureData.m_Parent->GetFilter();
		}

		virtual void SetFilter(TextureFiltering filter) override
		{
			if (m_Type == TextureType::Subtexture)
				OE_CORE_WARN("Cannot set Subtexture's Filtering!");
			else
				m_MasterTextureData.m_Filter = filter;
		}

		virtual TextureWrapping GetSWrapping() const override
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.m_SWrapping;
			return m_SubTextureData.m_Parent->m_MasterTextureData.m_SWrapping;
		}

		virtual TextureWrapping GetTWrapping() const override
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.m_TWrapping;
			return m_SubTextureData.m_Parent->m_MasterTextureData.m_TWrapping;
		}

		virtual void SetSWrapping(TextureWrapping wrapping) override
		{
			if (m_Type == TextureType::Subtexture)
				OE_CORE_WARN("Cannot set Subtexture's Wrapping!");
			else
				m_MasterTextureData.m_SWrapping = wrapping;
		}

		virtual void SetTWrapping(TextureWrapping wrapping) override
		{
			if (m_Type == TextureType::Subtexture)
				OE_CORE_WARN("Cannot set Subtexture's Wrapping!");
			else
				m_MasterTextureData.m_TWrapping = wrapping;
		}

		virtual const Color& GetBorderColor() const override
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.m_BorderColor;
			return m_SubTextureData.m_Parent->m_MasterTextureData.m_BorderColor;
		}

		virtual void SetBorderColor(const Color& color) override
		{
			if (m_Type == TextureType::Subtexture)
				OE_CORE_WARN("Cannot set Subtexture's BorderColor!");
			else
				m_MasterTextureData.m_BorderColor = color;
		}

		virtual TextureFormat GetFormat() const override
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.m_Format;
			return m_SubTextureData.m_Parent->GetFormat();
		}

		inline virtual TextureType GetType() const override { return m_Type; }

		inline Ref<GAPI::Texture2D> GetGPUTexture() const
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.m_MappedTexture;

			OE_CORE_ERROR("SubTextures don't have GPU texture!");
			return nullptr;
		}

		inline uint8_t* GetPixels() const
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.m_Pixels;

			OE_CORE_ERROR("SubTextures don't have PixelBuffer!");
			return nullptr;
		}

		virtual uint32_t GetRendererID() const
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.m_MappedTexture->GetRendererID();
			else
				return m_SubTextureData.m_Parent->m_MasterTextureData.m_MappedTexture->GetRendererID();
		}

		virtual Rect GetRect() const
		{
			Rect finalRect;

			if (m_Type == TextureType::Master)
			{
				const auto& boundedGPUTexture = m_MasterTextureData.m_MappedTexture;
				finalRect.x = m_MasterTextureData.m_MappedTextureRect.x / boundedGPUTexture->GetWidth();
				finalRect.y = m_MasterTextureData.m_MappedTextureRect.y / boundedGPUTexture->GetHeight();
				finalRect.z = m_MasterTextureData.m_MappedTextureRect.z / boundedGPUTexture->GetWidth();
				finalRect.w = m_MasterTextureData.m_MappedTextureRect.w / boundedGPUTexture->GetHeight();
			}
			else
			{
				const auto& boundedGPUTexture = m_SubTextureData.m_Parent->m_MasterTextureData.m_MappedTexture;

				const Rect& parentRect = m_SubTextureData.m_Parent->m_MasterTextureData.m_MappedTextureRect;
				Rect rect = m_SubTextureData.m_Rect;

				rect.x += parentRect.x;
				rect.y += parentRect.y;

				finalRect.x = rect.x / boundedGPUTexture->GetWidth();
				finalRect.y = rect.y / boundedGPUTexture->GetHeight();
				finalRect.z = rect.z / boundedGPUTexture->GetWidth();
				finalRect.w = rect.w / boundedGPUTexture->GetHeight();
			}

			return finalRect;
		}

		inline operator int() { return m_Width * m_Height; }
		inline operator uint32_t() { return m_Width * m_Height; }
	private:
		TextureType m_Type;

		uint32_t m_Width;
		uint32_t m_Height;

		union
		{
			MasterTextureData m_MasterTextureData;
			SubTextureData m_SubTextureData;
		};
	};
}
