#include "pcheader.h"
#include "Texture.h"

#include "OverEngine/Renderer/TextureManager.h"
#include <stb_image.h>

namespace OverEngine
{
	Ref<Texture2D> Texture2D::MasterFromFile(const String& path)
	{
		Ref<Texture2D> texture = CreateRef<Texture2D>(path);
		TextureManager::AddTexture(texture);
		return texture;
	}

	Ref<Texture2D> Texture2D::SubTextureFromExistingOne(Ref<Texture2D> masterTexture, Rect rect)
	{
		if (masterTexture->GetType() == TextureType::Subtexture)
		{
			OE_CORE_ERROR("Cannot create a subtexture from another one!");
			return nullptr;
		}

		return CreateRef<Texture2D>(masterTexture, rect);
	}

	Texture2D::Texture2D(const String& path)
		: m_Type(TextureType::Master), m_MasterTextureData()
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(0);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		OE_CORE_ASSERT(data, "Failde to load image!");
		m_Width = width;
		m_Height = height;

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

		m_MasterTextureData.m_Format = format;
		m_MasterTextureData.m_Filter = TextureFiltering::Linear;
		m_MasterTextureData.m_Pixels = data;
	}

	Texture2D::Texture2D(Ref<Texture2D> masterTexture, Rect rect)
		: m_Type(TextureType::Subtexture), m_SubTextureData()
	{
		m_SubTextureData.m_Parent = masterTexture;
		m_SubTextureData.m_Rect = rect;

		m_Width = (uint32_t)rect.z;
		m_Height = (uint32_t)rect.w;
	}

	Texture2D::~Texture2D()
	{
		if (m_Type == TextureType::Master)
			stbi_image_free(m_MasterTextureData.m_Pixels);
	}
}
