#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Events/Event.h"

#include "OverEngine/Renderer/RendererContext.h"

namespace OverEngine {

	struct WindowProps
	{
		String Title;
		unsigned int Width;
		unsigned int Height;
		bool DoubleBuffered;

		WindowProps(const String& title = "OverEngine",
			        unsigned int width = 800,
			        unsigned int height = 600,
			        bool doubleBuffered = true)
			: Title(title), Width(width), Height(height), DoubleBuffered(doubleBuffered)
		{
		}
	};

	/**
	 * Interface representing a desktop system based Window
	 * Use Windows::Create to create a window
	 */
	class OVER_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;
		virtual bool IsDoubleBuffered() const = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual RendererContext* GetRendererContext() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};

}