#include "pcheader.h"
#include "TextureManager.h"

#include "OverEngine/Renderer/GAPI/GTexture.h"
#include "OverEngine/Renderer/RenderCommand.h"

#include <stb_rectpack.h>

namespace OverEngine
{
	struct TextureManagerData
	{
		Vector<Ref<Texture2D>> MasterTextures;

		Vector<stbrp_node> NodeCache;
		Vector<stbrp_rect> RectCache;

		Vector<Ref<GAPI::Texture2D>> GPUTextures;
		Vector<stbrp_context> RectanglePackers;
	};

	static TextureManagerData* s_ManagerData;

	void TextureManager::Init()
	{
		s_ManagerData = new TextureManagerData();
	}

	void TextureManager::Shutdown()
	{
		delete s_ManagerData;
	}

	void TextureManager::AddTexture(Ref<Texture2D>& texture)
	{
		if (texture->GetType() == TextureType::Subtexture)
		{
			OE_CORE_ERROR("Cannot add SubTextures to TextureManager!");
			return;
		}

		auto it = STD_CONTAINER_FIND(s_ManagerData->MasterTextures, texture);
		if (it != s_ManagerData->MasterTextures.end())
		{
			OE_CORE_WARN("Texture is already handeled by TextureManager!");
			return;
		}

		// Now we should handle a new valid Texture

		s_ManagerData->MasterTextures.push_back(texture);

		if (s_ManagerData->GPUTextures.empty())
		{
			s_ManagerData->GPUTextures.push_back(GAPI::Texture2D::Create());
			s_ManagerData->RectanglePackers.push_back(stbrp_context());

			s_ManagerData->GPUTextures[0]->SetMinFilter(TextureFiltering::Nearest);
			s_ManagerData->GPUTextures[0]->SetMagFilter(TextureFiltering::Nearest);

			s_ManagerData->GPUTextures[0]->SetSWrapping(TextureWrapping::ClampToBorder);
			s_ManagerData->GPUTextures[0]->SetTWrapping(TextureWrapping::ClampToBorder);
			s_ManagerData->GPUTextures[0]->SetBorderColor({ 1.0f, 0.5f, 1.0f, 1.0f });

			s_ManagerData->GPUTextures[0]->AllocateStorage(TextureFormat::RGBA, texture->GetWidth(), texture->GetHeight());
			s_ManagerData->GPUTextures[0]->SubImage(texture->GetPixels(), texture->GetWidth(), texture->GetHeight(), texture->GetFormat());
			s_ManagerData->GPUTextures[0]->GetMemberTextures().push_back(texture);
			std::get<MasterTextureData>(texture->m_Data).MappedTexture = s_ManagerData->GPUTextures[0];
			std::get<MasterTextureData>(texture->m_Data).MappedTextureRect = {
				0, 0, texture->GetWidth(), texture->GetHeight()
			};

			return;
		}

		// Loop through all GPUTextures to put our new Texture into them
		uint32_t maxSize = RenderCommand::GetMaxTextureSize();
		float wastedSpaceRatioIncrementValue = 0.03f;

		for (uint32_t tID = 0; tID < s_ManagerData->GPUTextures.size(); tID++)
		{
			uint32_t textureHeightSum = 0;
			uint32_t textureWidthSum = 0;

			float wastedSpaceRatio = 0.0f;

			Ref<GAPI::Texture2D> currentGPUTexture = s_ManagerData->GPUTextures[tID];
			stbrp_context* currentContext = &s_ManagerData->RectanglePackers[tID];

			auto& currentGPUTextureMembers = currentGPUTexture->GetMemberTextures();
			uint32_t currentGPUTextureMemberCount = (uint32_t)currentGPUTextureMembers.size();

			s_ManagerData->RectCache.clear();
			if (s_ManagerData->RectCache.capacity() < currentGPUTextureMemberCount + (uint32_t)1)
				s_ManagerData->RectCache.reserve(currentGPUTextureMemberCount + (uint32_t)1);

			for (auto& t : currentGPUTextureMembers)
			{
				stbrp_rect rect;
				rect.w = t->GetWidth();
				rect.h = t->GetHeight();
				textureWidthSum += rect.w;
				textureHeightSum += rect.h;
				s_ManagerData->RectCache.push_back(rect);
			}

			stbrp_rect rect;
			rect.w = texture->GetWidth();
			rect.h = texture->GetHeight();
			textureWidthSum += rect.w;
			textureHeightSum += rect.h;
			s_ManagerData->RectCache.push_back(rect);

			uint32_t totalWidth = (uint32_t)(textureWidthSum * (1 + wastedSpaceRatio));
			uint32_t totalHeigth = (uint32_t)(textureHeightSum * (1 + wastedSpaceRatio));

			s_ManagerData->NodeCache.clear();
			if (s_ManagerData->NodeCache.capacity() < totalWidth)
				s_ManagerData->NodeCache.reserve(totalWidth);

			for (uint32_t i = 0; i < totalWidth; i++)
			{
				stbrp_node node;
				s_ManagerData->NodeCache.push_back(node);
			}

			stbrp_init_target(currentContext, totalWidth, totalHeigth, s_ManagerData->NodeCache.data(), (int)s_ManagerData->NodeCache.size());
			bool allPacked = stbrp_pack_rects(currentContext, s_ManagerData->RectCache.data(), (int)s_ManagerData->RectCache.size());

			bool textureOutOfSize = false;

			if (totalHeigth >= maxSize || totalWidth >= maxSize)
				textureOutOfSize = true;

			while (!allPacked)
			{
				wastedSpaceRatio += wastedSpaceRatioIncrementValue;

				totalWidth = (uint32_t)(textureWidthSum * (1 + wastedSpaceRatio));
				totalHeigth = (uint32_t)(textureHeightSum * (1 + wastedSpaceRatio));

				if (totalHeigth >= maxSize || totalWidth >= maxSize)
				{
					textureOutOfSize = true;
					break;
				}

				stbrp_init_target(currentContext, totalWidth, totalHeigth, s_ManagerData->NodeCache.data(), (int)s_ManagerData->NodeCache.size());
				allPacked = stbrp_pack_rects(currentContext, s_ManagerData->RectCache.data(), (int)s_ManagerData->RectCache.size());
			}

			if (textureOutOfSize)
				continue;

			// Packed!
			currentGPUTextureMembers.push_back(texture);
			currentGPUTexture->AllocateStorage(TextureFormat::RGBA, totalWidth, totalHeigth);

			for (uint32_t i = 0; i < currentGPUTextureMemberCount + 1; i++)
			{
				currentGPUTexture->SubImage(
					currentGPUTextureMembers[i]->GetPixels(),
					s_ManagerData->RectCache[i].w, s_ManagerData->RectCache[i].h,
					currentGPUTextureMembers[i]->GetFormat(),
					s_ManagerData->RectCache[i].x, s_ManagerData->RectCache[i].y
				);

				std::get<MasterTextureData>(currentGPUTextureMembers[i]->m_Data).MappedTextureRect = {
					s_ManagerData->RectCache[i].x, s_ManagerData->RectCache[i].y,
					s_ManagerData->RectCache[i].w, s_ManagerData->RectCache[i].h
				};
			}

			std::get<MasterTextureData>(texture->m_Data).MappedTexture = currentGPUTexture;
			return;
		}

		// We need to create another GPUTexture and ...
		s_ManagerData->GPUTextures.push_back(GAPI::Texture2D::Create());
		s_ManagerData->RectanglePackers.push_back(stbrp_context());

		auto currentGPUTexture = s_ManagerData->GPUTextures[s_ManagerData->GPUTextures.size() - 1];

		currentGPUTexture->SetMinFilter(TextureFiltering::Nearest);
		currentGPUTexture->SetMagFilter(TextureFiltering::Nearest);

		currentGPUTexture->SetSWrapping(TextureWrapping::ClampToBorder);
		currentGPUTexture->SetTWrapping(TextureWrapping::ClampToBorder);
		currentGPUTexture->SetBorderColor({ 1.0f, 0.0f, 1.0f, 1.0f });

		currentGPUTexture->AllocateStorage(TextureFormat::RGBA, texture->GetWidth(), texture->GetHeight());
		currentGPUTexture->SubImage(texture->GetPixels(), texture->GetWidth(), texture->GetHeight(), texture->GetFormat());
		currentGPUTexture->GetMemberTextures().push_back(texture);
		std::get<MasterTextureData>(texture->m_Data).MappedTexture = currentGPUTexture;
		std::get<MasterTextureData>(texture->m_Data).MappedTextureRect = {
			0, 0, texture->GetWidth(), texture->GetHeight()
		};
	}
}
