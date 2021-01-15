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

		virtual TextureWrapping GetSWrapping() const = 0;
		virtual TextureWrapping GetTWrapping() const = 0;
		virtual const Color& GetBorderColor() const = 0;

		virtual void SetSWrapping(TextureWrapping wrapping) = 0;
		virtual void SetTWrapping(TextureWrapping wrapping) = 0;
		virtual void SetBorderColor(const Color& color) = 0;

		virtual TextureFiltering GetMinFilter() const = 0;
		virtual TextureFiltering GetMagFilter() const = 0;

		virtual void SetMinFilter(TextureFiltering filter) = 0;
		virtual void SetMagFilter(TextureFiltering filter) = 0;

		virtual TextureFormat GetFormat() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual bool operator==(const GPUTexture& other) const = 0;
	};

	class GPUTexture2D : public GPUTexture
	{
	public:
		static Ref<GPUTexture2D> Create(const String& path, TextureFiltering minFilter = TextureFiltering::Linear, TextureFiltering magFilter = TextureFiltering::Linear);
		static Ref<GPUTexture2D> Create();

		virtual void AllocateStorage(TextureFormat format, uint32_t width, uint32_t height) = 0;
		virtual void SubImage(const uint8_t* pixels, uint32_t width, uint32_t height, TextureFormat dataFormat, int xOffset = 0, int yOffset = 0) = 0;

		virtual Vector<std::weak_ptr<Texture2D>>& GetMemberTextures() = 0;
	};
}
