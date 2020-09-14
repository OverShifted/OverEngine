#pragma once

#include "OverEngine/Core/Window.h"
#include "OverEngine/Renderer/RendererContext.h"

#include <GLFW/glfw3.h>

namespace OverEngine
{
	class LinuxWindow : public Window
	{
	public:
		LinuxWindow(const WindowProps& props);
		virtual ~LinuxWindow();

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

		static uint32_t s_WindowCount;
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
