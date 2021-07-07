#include "pcheader.h"
#include "Object.h"

#include "OverEngine/Core/Runtime/Reflection/TypeInfo.h"

namespace OverEngine
{
	ObjectTypeInfo* Object::Reflect()
	{
		static ObjectTypeInfo typeInfo(GetStaticClassName(), sizeof(Object));
		return &typeInfo;
	};

	bool Object::GetProperty(const String& name, Variant* out)
	{
		ObjectTypeInfo* klass = DynamicReflect();
		ObjectTypeInfo* check = klass;

		while (check)
		{
			if (STD_MAP_HAS(check->Properties, name))
			{
				(*out) = check->Properties.at(name)->Getter(this);
				return true;
			}

			check = check->ParentClass;
		}

		// OE_CORE_ASSERT(false, "Bad property access! Cannot find property `{}` on Object `{}` of type `{}`.", name, (void*)this, klass->Name);
		return false;
	}

	bool Object::SetProperty(const String& name, const Variant& value)
	{
		ObjectTypeInfo* klass = DynamicReflect();
		ObjectTypeInfo* check = klass;

		while (check)
		{
			if (STD_MAP_HAS(check->Properties, name))
			{
				check->Properties.at(name)->Setter(this, value);
				return true;
			}

			check = check->ParentClass;
		}

		// OE_CORE_ASSERT(false, "Bad property access! Cannot find property `{}` on Object `{}` of type `{}`.", name, (void*)this, klass->Name);
		return false;
	}
}
