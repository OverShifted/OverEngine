#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Renderer/TextureEnums.h"

#include <optional>

namespace OverEngine
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void Bind(uint32_t slot = 0) = 0;

		virtual TextureFilter GetFilter() const = 0;
		virtual void SetFilter(TextureFilter filter) = 0;

		virtual TextureWrap GetUWrap() const = 0;
		virtual void SetUWrap(TextureWrap wrap) = 0;

		virtual TextureWrap GetVWrap() const = 0;
		virtual void SetVWrap(TextureWrap wrap) = 0;

		inline void SetWrap(TextureWrap wrap) { SetUWrap(wrap); SetVWrap(wrap); }

		// Other
		virtual TextureFormat GetFormat() const = 0;
		virtual TextureType GetType() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create();
		static Ref<Texture2D> Create(const String& path);
	};

	class SubTexture2D : public Texture2D
	{
	public:
		static Ref<Texture2D> Create(const Ref<Texture2D>& texture, const Rect& rect);

		SubTexture2D(const Ref<Texture2D>& texture, const Rect& rect);

		virtual uint32_t GetWidth() const override;
		virtual uint32_t GetHeight() const override;
		virtual uint32_t GetRendererID() const override;

		virtual void Bind(uint32_t slot = 0) override;

		// Filter
		virtual TextureFilter GetFilter() const override;
		virtual void SetFilter(TextureFilter filter) override;

		// Wrap
		virtual TextureWrap GetUWrap() const override;
		virtual void SetUWrap(TextureWrap wrap) override;

		virtual TextureWrap GetVWrap() const override;
		virtual void SetVWrap(TextureWrap wrap) override;

		inline void SetWrap(TextureWrap wrap) { SetUWrap(wrap); SetVWrap(wrap); }

		// Other
		virtual TextureFormat GetFormat() const override;
		virtual TextureType GetType() const override;

		const Rect& GetRect() const { return m_Rect; }
		Ref<Texture2D> GetMasterTexture() const { return m_MasterTexture; }

	private:
		Ref<Texture2D> m_MasterTexture = nullptr;
		Rect m_Rect;
	};
}
