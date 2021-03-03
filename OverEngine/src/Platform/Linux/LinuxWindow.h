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

		virtual void SetTitle(const char* title) override;

		virtual void SetMousePosition(Vector2 position) override;
		virtual Vector2 GetMousePosition() override;

		virtual void SetClipboardText(const char* text) override;
		virtual const char* GetClipboardText() override;

		inline virtual void* GetNativeWindow() const override { return m_Window; }
		inline virtual RendererContext& GetRendererContext() const override { return *m_Context; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

		static uint32_t s_WindowCount;
	private:
		GLFWwindow* m_Window;
		Scope<RendererContext> m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}
