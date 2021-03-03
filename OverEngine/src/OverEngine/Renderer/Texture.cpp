#include "pcheader.h"

#include "OverEngine/Renderer/Texture.h"
#include "OverEngine/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>

namespace OverEngine
{
	Ref<Texture2D> Texture2D::Create(const String& path)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const uint64_t& guid)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    OE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(guid);
		}

		OE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> SubTexture2D::Create(const Ref<Texture2D>& texture, const Rect& rect)
	{
		return CreateRef<SubTexture2D>(texture, rect);
	}

	Ref<Texture2D> SubTexture2D::Create(const uint64_t& guid)
	{
		return CreateRef<SubTexture2D>(guid);
	}

	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const Rect& rect)
		: m_MasterTexture(texture), m_Rect(rect)
	{
		m_Rect /= Rect(
			texture->GetWidth(), texture->GetHeight(),
			texture->GetWidth(), texture->GetHeight()
		);
	}

	SubTexture2D::SubTexture2D(const uint64_t& guid)
		: m_Rect(0, 0, 0, 0)
	{
		SetGuid(guid);
	}

	uint32_t SubTexture2D::GetWidth() const
	{
		return m_Rect.z;
	}

	uint32_t SubTexture2D::GetHeight() const
	{
		return m_Rect.w;
	}

	uint32_t SubTexture2D::GetRendererID() const
	{
		return m_MasterTexture->GetRendererID();
	}

	void SubTexture2D::Bind(uint32_t slot)
	{
		m_MasterTexture->Bind(slot);
	}

	TextureFilter SubTexture2D::GetFilter() const
	{
		return m_MasterTexture->GetFilter();
	}

	void SubTexture2D::SetFilter(TextureFilter)
	{
		OE_CORE_WARN("Cant SubTexture2D::SetFilter");
	}

	TextureWrap SubTexture2D::GetUWrap() const
	{
		return m_MasterTexture->GetUWrap();
	}

	void SubTexture2D::SetUWrap(TextureWrap)
	{
		OE_CORE_WARN("Cant SubTexture2D::SetUWrap");
	}

	TextureWrap SubTexture2D::GetVWrap() const
	{
		return m_MasterTexture->GetVWrap();
	}

	void SubTexture2D::SetVWrap(TextureWrap)
	{
		OE_CORE_WARN("Cant SubTexture2D::SetVWrap");
	}

	TextureFormat SubTexture2D::GetFormat() const
	{
		return m_MasterTexture->GetFormat();
	}

	TextureType SubTexture2D::GetType() const
	{
		return TextureType::SubTexture;
	}
}
