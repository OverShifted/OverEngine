#include "pcheader.h"
#include "TextureManager.h"

#include "OverEngine/Renderer/GAPI/GTexture.h"
#include "OverEngine/Renderer/RenderCommand.h"

#include <stb_rectpack.h>

namespace OverEngine
{
	struct TextureManagerData
	{
		Vector<std::weak_ptr<Texture2D>> MasterTextures;

		stbrp_node* Nodes = nullptr;
		uint32_t NodeCount = 0;
		Vector<stbrp_rect> RectCache;

		Vector<Ref<GAPI::Texture2D>> GPUTextures;
	};

	static TextureManagerData* s_ManagerData = nullptr;

	void TextureManager::Init()
	{
		s_ManagerData = new TextureManagerData();

		s_ManagerData->Nodes = new stbrp_node;
		s_ManagerData->NodeCount = 1;
	}

	void TextureManager::Shutdown()
	{
		delete s_ManagerData;
		s_ManagerData = nullptr;
	}

	void TextureManager::AddTexture(const Ref<Texture2D>& texture)
	{
		if (!s_ManagerData)
			return;

		if (texture->GetType() == TextureType::Subtexture)
		{
			OE_CORE_ERROR("Cannot add SubTextures to TextureManager!");
			return;
		}

		auto it = std::find_if(
			s_ManagerData->MasterTextures.begin(),
			s_ManagerData->MasterTextures.end(),
			[&texture](const auto& v)
			{
				auto t = v.lock();
				return t && texture == t;
			}
		);

		if (it != s_ManagerData->MasterTextures.end())
		{
			OE_CORE_WARN("Texture is already handeled by TextureManager!");
			return;
		}

		// Now we should handle a new valid Texture
		s_ManagerData->MasterTextures.push_back(texture);

		// Loop through all GPUTextures to put our new Texture into them
		uint32_t maxSize = RenderCommand::GetMaxTextureSize();
		float wastedSpaceRatioGrowRate = 0.1f;

		for (auto& currentGPUTexture : s_ManagerData->GPUTextures)
		{
			uint32_t textureHeightSum = 0;
			uint32_t textureWidthSum = 0;

			float wastedSpaceRatio = 0.0f;

			stbrp_context currentContext;
			auto& gpuTextureMembers = currentGPUTexture->GetMemberTextures();
			uint32_t gpuTextureMemberCount = (uint32_t)gpuTextureMembers.size();

			s_ManagerData->RectCache.clear();
			if (s_ManagerData->RectCache.capacity() < gpuTextureMemberCount + 1u)
				s_ManagerData->RectCache.reserve(gpuTextureMemberCount + 1u);

			for (auto& t : gpuTextureMembers)
			{
				if (auto ref = t.lock())
				{
					stbrp_rect rect;
					rect.w = ref->GetWidth();
					rect.h = ref->GetHeight();
					textureWidthSum += rect.w;
					textureHeightSum += rect.h;
					s_ManagerData->RectCache.push_back(rect);
				}
			}

			stbrp_rect rect;
			rect.w = texture->GetWidth();
			rect.h = texture->GetHeight();
			textureWidthSum += rect.w;
			textureHeightSum += rect.h;
			s_ManagerData->RectCache.push_back(rect);

			textureHeightSum /= 100;
			textureWidthSum  /= 100;
			uint32_t totalWidth = (uint32_t)(textureWidthSum * (1 + wastedSpaceRatio));
			uint32_t totalHeigth = (uint32_t)(textureHeightSum * (1 + wastedSpaceRatio));

			if (s_ManagerData->NodeCount < totalWidth)
			{
				delete[] s_ManagerData->Nodes;
				s_ManagerData->Nodes = new stbrp_node[totalWidth];
				s_ManagerData->NodeCount = totalWidth;
			}

			stbrp_init_target(&currentContext, totalWidth, totalHeigth, s_ManagerData->Nodes, (int)s_ManagerData->NodeCount);
			bool allPacked = stbrp_pack_rects(&currentContext, s_ManagerData->RectCache.data(), (int)s_ManagerData->RectCache.size());

			bool textureOutOfSize = false;

			if (totalHeigth >= maxSize || totalWidth >= maxSize)
			{
				textureOutOfSize = true;
			}
			else
			{
				while (!allPacked)
				{
					wastedSpaceRatio += wastedSpaceRatioGrowRate;

					totalWidth = (uint32_t)(textureWidthSum * (1 + wastedSpaceRatio));
					totalHeigth = (uint32_t)(textureHeightSum * (1 + wastedSpaceRatio));

					if (totalHeigth >= maxSize || totalWidth >= maxSize)
					{
						textureOutOfSize = true;
						break;
					}

					stbrp_init_target(&currentContext, totalWidth, totalHeigth, s_ManagerData->Nodes, (int)s_ManagerData->NodeCount);
					allPacked = stbrp_pack_rects(&currentContext, s_ManagerData->RectCache.data(), (int)s_ManagerData->RectCache.size());
				}
			}

			if (textureOutOfSize)
				continue;

			// Packed!
			gpuTextureMembers.push_back(texture);
			currentGPUTexture->AllocateStorage(TextureFormat::RGBA, totalWidth, totalHeigth);

			for (uint32_t i = 0; i < gpuTextureMemberCount + 1; i++)
			{
				if (auto ref = gpuTextureMembers[i].lock())
				{
					currentGPUTexture->SubImage(
						ref->GetPixels(),
						s_ManagerData->RectCache[i].w, s_ManagerData->RectCache[i].h,
						ref->GetFormat(),
						s_ManagerData->RectCache[i].x, s_ManagerData->RectCache[i].y
					);

					std::get<MasterTextureData>(ref->m_Data).MappedPos = {
						s_ManagerData->RectCache[i].x, s_ManagerData->RectCache[i].y,
					};
				}
			}

			std::get<MasterTextureData>(texture->m_Data).MappedTexture = currentGPUTexture;

			return;
		}

		// We need to create another GPUTexture
		auto currentGPUTexture = GAPI::Texture2D::Create();

		s_ManagerData->GPUTextures.push_back(currentGPUTexture);

		currentGPUTexture->SetMinFilter(TextureFiltering::Nearest);
		currentGPUTexture->SetMagFilter(TextureFiltering::Nearest);

		currentGPUTexture->SetSWrapping(TextureWrapping::ClampToBorder);
		currentGPUTexture->SetTWrapping(TextureWrapping::ClampToBorder);
		currentGPUTexture->SetBorderColor({ 1.0f, 0.0f, 1.0f, 1.0f });

		currentGPUTexture->AllocateStorage(TextureFormat::RGBA, texture->GetWidth(), texture->GetHeight());
		currentGPUTexture->SubImage(texture->GetPixels(), texture->GetWidth(), texture->GetHeight(), texture->GetFormat());
		currentGPUTexture->GetMemberTextures().push_back(texture);

		auto& masterTData = std::get<MasterTextureData>(texture->m_Data);
		masterTData.MappedTexture = currentGPUTexture;
		masterTData.MappedPos = { 0, 0 };
	}

	void TextureManager::RemoveTexture(Texture2D* texture)
	{
		if (!s_ManagerData)
			return;

		auto mts = s_ManagerData->MasterTextures;

		mts.erase(
			std::remove_if(
				mts.begin(),
				mts.end(),
				[](const auto& v) { return v.expired(); }
			),
			mts.end()
		);

		auto& members = texture->GetGPUTexture()->GetMemberTextures();
		auto it = std::find_if(members.begin(), members.end(), [&texture](const auto& v)
		{
			return v.expired() || v.lock().get() == texture;
		});

		OE_CORE_ASSERT(it != members.end(), "Texture {} not found in its mapped(GPU)Texture's members!", (void*)texture);
		members.erase(it);
	}

	void TextureManager::ReloadTexture(Texture2D* texture)
	{
		if (!s_ManagerData)
			return;

		auto mts = s_ManagerData->MasterTextures;
		auto it = std::find_if(mts.begin(), mts.end(), [&texture](const auto& v)
		{
			auto t = v.lock();
			return t && t.get() == texture;
		});
		auto textureRef = it->lock();

		RemoveTexture(texture);
		AddTexture(textureRef);
	}
}
