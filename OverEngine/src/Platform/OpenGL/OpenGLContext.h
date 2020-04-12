#pragma once

#include "OverEngine/Renderer/RendererContext.h"

struct GLFWwindow;

namespace OverEngine {

	class OpenGLContext : public RendererContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void SetViewport(int width, int height, int x = 0, int y = 0) override;
		virtual void BackupContext() override;
		virtual void ApplyBackupedContext() override;
		virtual const char* GetInfoVersion() override;
		virtual const char* GetInfoVendor() override;
		virtual const char* GetInfoRenderer() override;
	private:
		GLFWwindow* m_WindowHandle;
		GLFWwindow* m_WindowBackup;
	};

}