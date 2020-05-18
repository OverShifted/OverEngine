#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Events/Event.h"

#include "OverEngine/Renderer/RendererContext.h"

namespace OverEngine {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		bool DoubleBuffered;

		WindowProps(const std::string& title = "OverEngine",
			        unsigned int width = 1280,
			        unsigned int height = 720,
			        bool doubleBuffered = true)
			: Title(title), Width(width), Height(height), DoubleBuffered(doubleBuffered)
		{
		}
	};

	// Interface representing a desktop system based Window
	class OVER_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

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