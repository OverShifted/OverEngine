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

		virtual TextureFiltering GetFiltering() const = 0;
		virtual void SetFiltering(TextureFiltering filter) = 0;

		virtual TextureWrapping GetXWrapping() const = 0;
		virtual TextureWrapping GetYWrapping() const = 0;

		virtual void SetXWrapping(TextureWrapping wrapping) = 0;
		virtual void SetYWrapping(TextureWrapping wrapping) = 0;

		virtual TextureFormat GetFormat() const = 0;
		virtual TextureType GetType() const = 0;

		virtual Ref<GAPI::Texture2D> GetGPUTexture() const = 0;
		virtual Rect GetRect() const = 0;
	};

	class Texture2DAsset;

	struct MasterTextureData
	{
		TextureFormat Format;

		uint32_t Width;
		uint32_t Height;

		TextureFiltering Filtering;
		Vec2T<TextureWrapping> Wrapping = { TextureWrapping::Repeat, TextureWrapping::Repeat };

		uint8_t* Pixels;

		Ref<GAPI::Texture2D> MappedTexture;
		Vec2T<uint32_t> MappedPos;

		Texture2DAsset* Asset = nullptr;

		String ImageFilePath;
	};

	class Texture2D;

	struct SubTextureData
	{
		Ref<Texture2D> Parent;
		glm::uvec4 Rect;
	};

	struct PlaceHolderTextureData
	{
		uint64_t AssetGuid;
		uint64_t Texture2DGuid;
	};

	class TextureManager;

	#define __Texture2D_GetMasterTextureData std::get<MasterTextureData>(m_Data)
	#define __Texture2D_GetSubTextureData std::get<SubTextureData>(m_Data)
	#define __Texture2D_GetParentMasterTextureData std::get<MasterTextureData>(__Texture2D_GetSubTextureData.Parent->m_Data)

	#define __Texture2D_COMMON_GET(x, nil)  if (m_Type == TextureType::Master)                   \
												return __Texture2D_GetMasterTextureData.x;       \
											if (m_Type == TextureType::Subtexture)               \
												return __Texture2D_GetParentMasterTextureData.x; \
											return nil;
	
	#define __Texture2D_COMMON_ASSERT(what_to_set, set_to_what, msg)             \
		if (m_Type == TextureType::Master)                                       \
			__Texture2D_GetMasterTextureData.what_to_set = set_to_what;          \
		else                                                                     \
			OE_CORE_ERROR("Cannot set Subtexture's and Placeholder's " msg "!");

	class Texture2D : public Texture
	{
		friend class TextureManager;
		friend class Texture2DAsset;

	public:
		static Ref<Texture2D> CreateMaster(const String& path);
		static Ref<Texture2D> CreateSubTexture(Ref<Texture2D> masterTexture, Rect rect);
		static Ref<Texture2D> CreatePlaceholder(const uint64_t& assetGuid, const uint64_t& textureGuid);

		Texture2D(const String& path); // CreateMaster
		Texture2D(Ref<Texture2D> masterTexture, Rect rect); // CreateSubTexture
		Texture2D(const uint64_t& assetGuid, const uint64_t& textureGuid); // CreatePlaceholder
		virtual ~Texture2D();

		const String& GetName() const;

		virtual uint32_t GetWidth() const override;
		virtual uint32_t GetHeight() const override;

		virtual TextureFiltering GetFiltering() const override { __Texture2D_COMMON_GET(Filtering, TextureFiltering::None); }
		virtual void SetFiltering(TextureFiltering filtering) override { __Texture2D_COMMON_ASSERT(Filtering, filtering, "Filtering"); }

		virtual TextureWrapping GetXWrapping() const override { __Texture2D_COMMON_GET(Wrapping.x, TextureWrapping::None); }
		virtual TextureWrapping GetYWrapping() const override { __Texture2D_COMMON_GET(Wrapping.y, TextureWrapping::None); }

		virtual void SetXWrapping(TextureWrapping wrapping) override { __Texture2D_COMMON_ASSERT(Wrapping.x, wrapping, "Wrapping"); }
		virtual void SetYWrapping(TextureWrapping wrapping) override { __Texture2D_COMMON_ASSERT(Wrapping.y, wrapping, "Wrapping"); }

		virtual TextureFormat GetFormat() const override { __Texture2D_COMMON_GET(Format, TextureFormat::None); }
		inline virtual TextureType GetType() const override { return m_Type; }

		inline virtual Ref<GAPI::Texture2D> GetGPUTexture() const override { __Texture2D_COMMON_GET(MappedTexture, nullptr); }
		virtual Rect GetRect() const;

		inline uint8_t* GetPixels() const
		{
			if (m_Type == TextureType::Master)
				return __Texture2D_GetMasterTextureData.Pixels;

			OE_CORE_ERROR("SubTextures don't have PixelBuffer!");
			return nullptr;
		}

		bool Reload(const String& filePath = String());

		Texture2DAsset* GetAsset() const { __Texture2D_COMMON_GET(Asset, nullptr); }
		const auto& GetData() const { return m_Data; }
	private:
		TextureType m_Type;

		std::variant<MasterTextureData, SubTextureData, PlaceHolderTextureData> m_Data;
	};
}
