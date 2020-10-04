#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Events/Event.h"

#include "OverEngine/Renderer/RendererContext.h"

namespace OverEngine {

	struct WindowProps
	{
		String Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const String& title = "OverEngine",
					uint32_t width = 1280,
					uint32_t height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	/*
	 * Interface representing a desktop system based Window
	 * Use Windows::Create to create a window
	 */

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void SetTitle(const char* title) = 0;

		// Mouse position
		virtual void SetMousePosition(Vector2 position) = 0;
		virtual Vector2 GetMousePosition() = 0;

		// Clipboard
		virtual void SetClipboardText(const char* text) = 0;
		virtual const char* GetClipboardText() = 0;

		// Internal stuff
		virtual void* GetNativeWindow() const = 0;
		virtual RendererContext& GetRendererContext() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};
}
