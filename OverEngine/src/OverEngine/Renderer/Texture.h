#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Renderer/GAPI/GTexture.h"
#include "OverEngine/Renderer/TextureEnums.h"

#include <variant>

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

	struct PlaceHolderTextureData
	{
		uint64_t Guid;
	};

	class TextureManager;

	#define __Texture2D_GetMasterTextureData std::get<MasterTextureData>(m_Data)
	#define __Texture2D_GetSubTextureData std::get<SubTextureData>(m_Data)
	#define __Texture2D_GetParentMasterTextureData std::get<MasterTextureData>(__Texture2D_GetSubTextureData.Parent->m_Data)

	#define __Texture2D_COMMON_GET(x)   if (m_Type == TextureType::Master)                     \
											return __Texture2D_GetMasterTextureData.x;                      \
										return __Texture2D_GetParentMasterTextureData.x; \

	class Texture2D : public Texture
	{
		friend class TextureManager;
		friend class Texture2DAsset;

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
			__Texture2D_COMMON_GET(Filtering);
		}

		virtual void SetFilter(TextureFiltering filter) override
		{
			if (m_Type == TextureType::Subtexture)
				OE_CORE_WARN("Cannot set Subtexture's Filtering!");
			else
				__Texture2D_GetMasterTextureData.Filtering = filter;
		}

		virtual TextureWrapping GetXWrapping() const override
		{
			__Texture2D_COMMON_GET(Wrapping.x);
		}

		virtual TextureWrapping GetYWrapping() const override
		{
			__Texture2D_COMMON_GET(Wrapping.y);
		}

		virtual void SetXWrapping(TextureWrapping wrapping) override
		{
			if (m_Type == TextureType::Subtexture)
				OE_CORE_WARN("Cannot set Subtexture's Wrapping!");
			else
				__Texture2D_GetMasterTextureData.Wrapping.x = wrapping;
		}

		virtual void SetYWrapping(TextureWrapping wrapping) override
		{
			if (m_Type == TextureType::Subtexture)
				OE_CORE_WARN("Cannot set Subtexture's Wrapping!");
			else
				__Texture2D_GetMasterTextureData.Wrapping.y = wrapping;
		}

		virtual const Color& GetBorderColor() const override
		{
			__Texture2D_COMMON_GET(BorderColor);
		}

		virtual void SetBorderColor(const Color& color) override
		{
			if (m_Type == TextureType::Subtexture)
				OE_CORE_WARN("Cannot set Subtexture's BorderColor!");
			else
				__Texture2D_GetMasterTextureData.BorderColor = color;
		}

		virtual TextureFormat GetFormat() const override
		{
			__Texture2D_COMMON_GET(Format);
		}

		inline virtual TextureType GetType() const override { return m_Type; }

		inline Ref<GAPI::Texture2D> GetGPUTexture() const
		{
			__Texture2D_COMMON_GET(MappedTexture);
		}

		inline uint8_t* GetPixels() const
		{
			if (m_Type == TextureType::Master)
				return __Texture2D_GetMasterTextureData.Pixels;

			OE_CORE_ERROR("SubTextures don't have PixelBuffer!");
			return nullptr;
		}

		virtual uint32_t GetRendererID() const
		{
			return GetGPUTexture()->GetRendererID();
		}

		virtual Rect GetRect() const
		{
			Rect finalRect;

			if (m_Type == TextureType::Master)
			{
				const auto& boundedGPUTexture = __Texture2D_GetMasterTextureData.MappedTexture;
				finalRect.x = __Texture2D_GetMasterTextureData.MappedTextureRect.x / boundedGPUTexture->GetWidth();
				finalRect.y = __Texture2D_GetMasterTextureData.MappedTextureRect.y / boundedGPUTexture->GetHeight();
				finalRect.z = __Texture2D_GetMasterTextureData.MappedTextureRect.z / boundedGPUTexture->GetWidth();
				finalRect.w = __Texture2D_GetMasterTextureData.MappedTextureRect.w / boundedGPUTexture->GetHeight();
			}
			else
			{
				const auto& boundedGPUTexture = __Texture2D_GetParentMasterTextureData.MappedTexture;

				const Rect& parentRect = __Texture2D_GetParentMasterTextureData.MappedTextureRect;
				Rect rect = __Texture2D_GetSubTextureData.Rect;

				rect.x += parentRect.x;
				rect.y += parentRect.y;

				finalRect.x = rect.x / boundedGPUTexture->GetWidth();
				finalRect.y = rect.y / boundedGPUTexture->GetHeight();
				finalRect.z = rect.z / boundedGPUTexture->GetWidth();
				finalRect.w = rect.w / boundedGPUTexture->GetHeight();
			}

			return finalRect;
		}

		Texture2DAsset* GetAsset() const
		{
			__Texture2D_COMMON_GET(Asset);
		}

		inline operator int() { return m_Width * m_Height; }
		inline operator uint32_t() { return m_Width * m_Height; }
	private:
		TextureType m_Type;

		uint32_t m_Width;
		uint32_t m_Height;

		/*union
		{
			MasterTextureData GetMasterTextureData;
			SubTextureData m_SubTextureData;
			PlaceHolderTextureData m_PlaceHolderData;
		};*/

		std::variant<MasterTextureData, SubTextureData, PlaceHolderTextureData> m_Data;
	};
}
