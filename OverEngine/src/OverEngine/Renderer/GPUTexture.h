#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Renderer/TextureEnums.h"

namespace OverEngine
{
	class Texture2D;

	class GPUTexture
	{
	public:
		virtual ~GPUTexture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual TextureWrap GetUWrap() const = 0;
		virtual TextureWrap GetVWrap() const = 0;

		virtual void SetUWrap(TextureWrap wrap) = 0;
		virtual void SetVWrap(TextureWrap wrap) = 0;

		virtual TextureFilter GetMinFilter() const = 0;
		virtual TextureFilter GetMagFilter() const = 0;

		virtual void SetMinFilter(TextureFilter filter) = 0;
		virtual void SetMagFilter(TextureFilter filter) = 0;

		virtual TextureFormat GetFormat() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual bool operator==(const GPUTexture& other) const = 0;
	};

	class GPUTexture2D : public GPUTexture
	{
	public:
		static Ref<GPUTexture2D> Create(const String& path, TextureFilter minFilter = TextureFilter::Linear, TextureFilter magFilter = TextureFilter::Linear);
		static Ref<GPUTexture2D> Create();

		virtual void AllocateStorage(TextureFormat format, uint32_t width, uint32_t height) = 0;
		virtual void SubImage(const uint8_t* pixels, uint32_t width, uint32_t height, TextureFormat dataFormat, int xOffset = 0, int yOffset = 0) = 0;

		virtual Vector<std::weak_ptr<Texture2D>>& GetMemberTextures() = 0;
	};
}
