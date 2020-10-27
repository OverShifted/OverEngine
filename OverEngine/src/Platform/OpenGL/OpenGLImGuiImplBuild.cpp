#include "pcheader.h"

// For Glad 1 :
// #define IMGUI_IMPL_OPENGL_LOADER_GLAD 

// For Glad2 :
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM <glad/gl.h>

#ifdef _MSC_VER
	#pragma warning(disable:4996)
#endif

#include "imgui/backends/imgui_impl_opengl3.cpp"
