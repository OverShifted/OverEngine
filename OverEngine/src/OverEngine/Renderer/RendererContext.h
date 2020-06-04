#pragma once

namespace OverEngine
{
	class Window; // Forward declaration

	namespace Renderer
	{

		class RendererContext
		{
		public:
			virtual void Init() = 0;
			virtual void SwapBuffers() = 0;
			virtual void SetViewport(int width, int height, int x = 0, int y = 0) = 0;

			virtual void Current() = 0;

			virtual const char* GetInfoVersion() = 0;
			virtual const char* GetInfoVendor() = 0;
			virtual const char* GetInfoRenderer() = 0;

			static RendererContext* Create(Window* handle);
		};
	}
}