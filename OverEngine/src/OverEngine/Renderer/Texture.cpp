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

	Ref<Texture2D> Texture2D::CreatePlaceholder(const uint64_t& assetGuid, const uint64_t& textureGuid)
	{
		return CreateRef<Texture2D>(assetGuid, textureGuid);
	}

	Texture2D::Texture2D(const String& path)
		: m_Type(TextureType::Master), m_Data(MasterTextureData())
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(0);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		OE_CORE_ASSERT(data, "Failed to load image! reason: '{}'", stbi_failure_reason());

		__Texture2D_GetMasterTextureData.Width = width;
		__Texture2D_GetMasterTextureData.Height = height;
		__Texture2D_GetMasterTextureData.ImageFilePath = path;

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

	Texture2D::Texture2D(const uint64_t& assetGuid, const uint64_t& textureGuid)
		: m_Type(TextureType::Placeholder), m_Data(PlaceHolderTextureData{ assetGuid, textureGuid })
	{
	}

	Texture2D::~Texture2D()
	{
		OE_CORE_INFO("Hey!! Bye Bye!!");
		if (m_Type == TextureType::Master)
		{
			TextureManager::RemoveTexture(this);
			stbi_image_free(__Texture2D_GetMasterTextureData.Pixels);
		}
	}

	const String& Texture2D::GetName() const
	{
		static String placeholder = "Placeholder Texture";

		Texture2DAsset* asset = nullptr;
		if (m_Type == TextureType::Master)
			asset = __Texture2D_GetMasterTextureData.Asset;
		else if (m_Type == TextureType::Subtexture)
			asset = __Texture2D_GetParentMasterTextureData.Asset;

		if (asset)
			return asset->GetName();
		else if (m_Type == TextureType::Placeholder)
			return placeholder;

		static String untitled = "Untitled Texture";
		return untitled;
	}

	uint32_t Texture2D::GetWidth() const
	{
		if (m_Type == TextureType::Master)
			return __Texture2D_GetMasterTextureData.Width;
		if (m_Type == TextureType::Subtexture)
			return __Texture2D_GetSubTextureData.Rect.z;
		return 0;
	}

	uint32_t Texture2D::GetHeight() const
	{
		if (m_Type == TextureType::Master)
			return __Texture2D_GetMasterTextureData.Height;
		if (m_Type == TextureType::Subtexture)
			return __Texture2D_GetSubTextureData.Rect.w;
		return 0;
	}

	Rect Texture2D::GetRect() const
	{
		Rect finalRect;

		if (m_Type == TextureType::Master)
		{
			const auto& master = __Texture2D_GetMasterTextureData;
			const auto& boundedGPUTexture = master.MappedTexture;
			finalRect.x = ((float)master.MappedPos.x / (float)boundedGPUTexture->GetWidth());
			finalRect.y = ((float)master.MappedPos.y / (float)boundedGPUTexture->GetHeight());
			finalRect.z = ((float)master.Width       / (float)boundedGPUTexture->GetWidth());
			finalRect.w = ((float)master.Height      / (float)boundedGPUTexture->GetHeight());
		}
		else
		{
			const auto& boundedGPUTexture = __Texture2D_GetParentMasterTextureData.MappedTexture;

			Rect rect = __Texture2D_GetSubTextureData.Rect;

			rect.x += __Texture2D_GetParentMasterTextureData.MappedPos.x;
			rect.y += __Texture2D_GetParentMasterTextureData.MappedPos.y;

			finalRect.x = rect.x / boundedGPUTexture->GetWidth();
			finalRect.y = rect.y / boundedGPUTexture->GetHeight();
			finalRect.z = rect.z / boundedGPUTexture->GetWidth();
			finalRect.w = rect.w / boundedGPUTexture->GetHeight();
		}

		return finalRect;
	}

	bool Texture2D::Reload(const String& filePath /*= String()*/)
	{
		const char* realPath = filePath.c_str();
		if (filePath.empty())
		{
			const auto& internalImagefilePath = __Texture2D_GetMasterTextureData.ImageFilePath;

			if (internalImagefilePath.empty())
				return false;

			realPath = internalImagefilePath.c_str();
		}

		int width, height, channels;
		stbi_set_flip_vertically_on_load(0);
		stbi_uc* data = stbi_load(realPath, &width, &height, &channels, 0);
		OE_CORE_ASSERT(data, "Failed to load image! reason: '{}'", stbi_failure_reason());

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

		TextureManager::ReloadTexture(this);

		return true;
	}
}
