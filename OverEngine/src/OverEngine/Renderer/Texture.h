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

		virtual TextureWrapping GetXWrapping() const = 0;
		virtual TextureWrapping GetYWrapping() const = 0;

		virtual void SetXWrapping(TextureWrapping wrapping) = 0;
		virtual void SetYWrapping(TextureWrapping wrapping) = 0;

		virtual const Color& GetBorderColor() const = 0;
		virtual void SetBorderColor(const Color& color) = 0;

		virtual TextureFormat GetFormat() const = 0;
		virtual TextureType GetType() const = 0;

		virtual uint32_t GetRendererID() const = 0;
		virtual Rect GetRect() const = 0;
	};

	class Texture2DAsset;

	struct MasterTextureData
	{
		TextureFormat Format;

		TextureFiltering Filtering;
		Vec2T<TextureWrapping> Wrapping = { TextureWrapping::Repeat, TextureWrapping::Repeat };
		Color BorderColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		uint8_t* Pixels;

		Ref<GAPI::Texture2D> MappedTexture;
		Rect MappedTextureRect;

		Texture2DAsset* Asset = nullptr;
	};

	class Texture2D;

	struct SubTextureData
	{
		Ref<Texture2D> Parent;
		OverEngine::Rect Rect;
	};

	class TextureManager;
	class Renderer2D;

	class Texture2D : public Texture
	{
		friend class TextureManager;
		friend class Texture2DAsset;
		friend class Renderer2D;
	public:
		static Ref<Texture2D> CreateMaster(const String& path);
		static Ref<Texture2D> CreateSubTexture(Ref<Texture2D> masterTexture, Rect rect);

		Texture2D(const String& path);
		Texture2D(Ref<Texture2D> masterTexture, Rect rect);
		virtual ~Texture2D();

		const String& GetName() const;

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }

		virtual TextureFiltering GetFilter() const override
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.Filtering;
			return m_SubTextureData.Parent->GetFilter();
		}

		virtual void SetFilter(TextureFiltering filter) override
		{
			if (m_Type == TextureType::Subtexture)
				OE_CORE_WARN("Cannot set Subtexture's Filtering!");
			else
				m_MasterTextureData.Filtering = filter;
		}

		virtual TextureWrapping GetXWrapping() const override
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.Wrapping.x;
			return m_SubTextureData.Parent->m_MasterTextureData.Wrapping.x;
		}

		virtual TextureWrapping GetYWrapping() const override
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.Wrapping.y;
			return m_SubTextureData.Parent->m_MasterTextureData.Wrapping.y;
		}

		virtual void SetXWrapping(TextureWrapping wrapping) override
		{
			if (m_Type == TextureType::Subtexture)
				OE_CORE_WARN("Cannot set Subtexture's Wrapping!");
			else
				m_MasterTextureData.Wrapping.x = wrapping;
		}

		virtual void SetYWrapping(TextureWrapping wrapping) override
		{
			if (m_Type == TextureType::Subtexture)
				OE_CORE_WARN("Cannot set Subtexture's Wrapping!");
			else
				m_MasterTextureData.Wrapping.y = wrapping;
		}

		virtual const Color& GetBorderColor() const override
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.BorderColor;
			return m_SubTextureData.Parent->m_MasterTextureData.BorderColor;
		}

		virtual void SetBorderColor(const Color& color) override
		{
			if (m_Type == TextureType::Subtexture)
				OE_CORE_WARN("Cannot set Subtexture's BorderColor!");
			else
				m_MasterTextureData.BorderColor = color;
		}

		virtual TextureFormat GetFormat() const override
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.Format;
			return m_SubTextureData.Parent->GetFormat();
		}

		inline virtual TextureType GetType() const override { return m_Type; }

		inline Ref<GAPI::Texture2D> GetGPUTexture() const
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.MappedTexture;

			OE_CORE_ERROR("SubTextures don't have GPU texture!");
			return nullptr;
		}

		inline uint8_t* GetPixels() const
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.Pixels;

			OE_CORE_ERROR("SubTextures don't have PixelBuffer!");
			return nullptr;
		}

		virtual uint32_t GetRendererID() const
		{
			if (m_Type == TextureType::Master)
				return m_MasterTextureData.MappedTexture->GetRendererID();
			else
				return m_SubTextureData.Parent->m_MasterTextureData.MappedTexture->GetRendererID();
		}

		virtual Rect GetRect() const
		{
			Rect finalRect;

			if (m_Type == TextureType::Master)
			{
				const auto& boundedGPUTexture = m_MasterTextureData.MappedTexture;
				finalRect.x = m_MasterTextureData.MappedTextureRect.x / boundedGPUTexture->GetWidth();
				finalRect.y = m_MasterTextureData.MappedTextureRect.y / boundedGPUTexture->GetHeight();
				finalRect.z = m_MasterTextureData.MappedTextureRect.z / boundedGPUTexture->GetWidth();
				finalRect.w = m_MasterTextureData.MappedTextureRect.w / boundedGPUTexture->GetHeight();
			}
			else
			{
				const auto& boundedGPUTexture = m_SubTextureData.Parent->m_MasterTextureData.MappedTexture;

				const Rect& parentRect = m_SubTextureData.Parent->m_MasterTextureData.MappedTextureRect;
				Rect rect = m_SubTextureData.Rect;

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
