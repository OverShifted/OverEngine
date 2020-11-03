#include "pcheader.h"
#include "Texture.h"
#include "OverEngine/Assets/Texture2DAsset.h"

#include "OverEngine/Renderer/TextureManager.h"
#include <stb_image.h>

namespace OverEngine
{
	Ref<Texture2D> Texture2D::CreateMaster(const String& path)
	{
		Ref<Texture2D> texture = CreateRef<Texture2D>(path);
		TextureManager::AddTexture(texture);
		return texture;
	}

	Ref<Texture2D> Texture2D::CreateSubTexture(Ref<Texture2D> masterTexture, Rect rect)
	{
		if (masterTexture->GetType() == TextureType::Subtexture)
		{
			OE_CORE_ERROR("Cannot create a subtexture from another subtexture!");
			return nullptr;
		}

		return CreateRef<Texture2D>(masterTexture, rect);
	}

	Ref<Texture2D> Texture2D::CreatePlaceholder(const uint64_t& guid)
	{
		return CreateRef<Texture2D>(guid);
	}

	Texture2D::Texture2D(const String& path)
		: m_Type(TextureType::Master), m_Data(MasterTextureData())
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(0);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		OE_CORE_ASSERT(data, "Failde to load image!");

		__Texture2D_GetMasterTextureData.Width = width;
		__Texture2D_GetMasterTextureData.Height = height;

		TextureFormat format = TextureFormat::None;
		if (channels == 3)
		{
			format = TextureFormat::RGB;
		}
		else if (channels == 4)
		{
			format = TextureFormat::RGBA;
		}
		OE_CORE_ASSERT(format != TextureFormat::None, "Unsupported image format");

		__Texture2D_GetMasterTextureData.Format = format;
		__Texture2D_GetMasterTextureData.Filtering = TextureFiltering::Linear;
		__Texture2D_GetMasterTextureData.Pixels = data;
	}

	Texture2D::Texture2D(Ref<Texture2D> masterTexture, Rect rect)
		: m_Type(TextureType::Subtexture), m_Data(SubTextureData{ masterTexture, rect })
	{
	}

	Texture2D::Texture2D(const uint64_t& guid)
		: m_Type(TextureType::Placeholder), m_Data(PlaceHolderTextureData{ guid })
	{
	}

	Texture2D::~Texture2D()
	{
		if (m_Type == TextureType::Master)
			stbi_image_free(__Texture2D_GetMasterTextureData.Pixels);
	}

	const String& Texture2D::GetName() const
	{
		static String untitled = "Untitled Texture";

		Texture2DAsset* asset = nullptr;
		if (m_Type == TextureType::Master)
			asset = __Texture2D_GetMasterTextureData.Asset;
		else
			asset = __Texture2D_GetParentMasterTextureData.Asset;

		if (asset)
			return asset->GetName();

		return untitled;
	}

	Rect Texture2D::GetRect() const
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
}
