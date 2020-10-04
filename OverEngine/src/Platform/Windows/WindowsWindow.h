#pragma once

#include "OverEngine/Core/Window.h"
#include "OverEngine/Renderer/RendererContext.h"

#include <GLFW/glfw3.h>

namespace OverEngine
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;

		inline virtual uint32_t GetWidth() const override { return m_Data.Width; }
		inline virtual uint32_t GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

		virtual void SetTitle(const char* title) override;

		virtual void SetMousePosition(Vector2 position) override;
		virtual Vector2 GetMousePosition() override;

		virtual void SetClipboardText(const char* text) override;
		virtual const char* GetClipboardText() override;

		inline virtual void* GetNativeWindow() const { return m_Window; }
		inline virtual RendererContext& GetRendererContext() const { return *m_Context; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

		static uint32_t s_WindowCount;
	private:
		GLFWwindow* m_Window;
		Scope<RendererContext> m_Context;

		struct WindowData
		{
			String Title;
			uint32_t Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}
