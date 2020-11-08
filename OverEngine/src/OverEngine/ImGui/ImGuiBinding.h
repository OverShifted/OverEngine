#pragma once

struct ImDrawData;

namespace OverEngine
{
	class Window;

	class ImGuiBinding
	{
	public:
		static void Init(Window* window);

		inline static void Shutdown()
		{
			s_RendererShutdownFunction();
			s_PlatformShutdownFunction();
		}

		inline static void NewFrame()
		{
			s_RendererNewFrameFunction();
			s_PlatformNewFrameFunction();
		}

		inline static void Render(ImDrawData* drawData)
		{
			s_RenderFunction(drawData);
		}
	private:
		static void(*s_RendererShutdownFunction)();
		static void(*s_PlatformShutdownFunction)();

		static void(*s_RendererNewFrameFunction)();
		static void(*s_PlatformNewFrameFunction)();

		static void(*s_RenderFunction)(ImDrawData*);
	};
}
