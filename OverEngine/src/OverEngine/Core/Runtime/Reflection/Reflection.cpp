#include "pcheader.h"
#include "Reflection.h"

namespace OverEngine::Reflection
{
	Scope<UnorderedMap<size_t, TypeDescriptor*>> EnumClassResolver::Descriptors = nullptr;
}
