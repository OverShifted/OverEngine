#pragma once

#include "OverEngine/Core/Window.h"
#include "OverEngine/Renderer/RendererContext.h"

#include <GLFW/glfw3.h>

namespace OverEngine {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
		bool IsDoubleBuffered() const override;

		inline virtual void* GetNativeWindow() const { return m_Window; }
		inline virtual RendererContext* GetRendererContext() const { return m_Context; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		RendererContext* m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			bool DoubleBuffered;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}