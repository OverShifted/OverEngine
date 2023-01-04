#include "pcheader.h"

#ifdef __GNUC__
	#warning "Defining STBI_NO_SIMD because of spdlog (see https://github.com/gabime/spdlog/issues/2248)"
	#define STBI_NO_SIMD
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
