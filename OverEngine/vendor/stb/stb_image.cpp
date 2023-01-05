#include "pcheader.h"

#if defined(__GNUC__) && defined(OE_DEBUG) && defined(__OPTIMIZE__)
	#warning "Undefing __OPTIMIZE__ because of fmt (see https://github.com/nothings/stb/discussions/1432)"
	#undef __OPTIMIZE__
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
