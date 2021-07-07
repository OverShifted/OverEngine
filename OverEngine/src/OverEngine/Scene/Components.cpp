#include "pcheader.h"
#include "Components.h"

#include "OverEngine/Core/Runtime/Reflection/TypeInfo.h"

namespace OverEngine
{
	ObjectTypeInfo* SpriteRendererComponent::Reflect()
	{
		static ObjectTypeInfo* typeInfo = nullptr;

		if (!typeInfo)
		{
			typeInfo = new ObjectTypeInfo(GetStaticClassName(), sizeof(SpriteRendererComponent), dynamic_cast<ObjectTypeInfo*>(TypeResolver<Component>::Get()));

			ADD_PUBLIC_MEMBER_PROPERTY(SpriteRendererComponent, Tint)
			ADD_PUBLIC_MEMBER_PROPERTY(SpriteRendererComponent, Sprite)
			ADD_PUBLIC_MEMBER_PROPERTY(SpriteRendererComponent, Tiling)
			ADD_PUBLIC_MEMBER_PROPERTY(SpriteRendererComponent, Offset)
			ADD_PUBLIC_MEMBER_ENUM_PROPERTY(SpriteRendererComponent, Flip, PropertyHint(PropertyHintType::Flags, { { 0, "None" }, { BIT(0), "X" }, { BIT(1), "Y" } }))
			ADD_PUBLIC_MEMBER_PROPERTY(SpriteRendererComponent, ForceTile)
		}

		return typeInfo;
	}

	void NativeScriptsComponent::RemoveScript(size_t hash)
	{
		if (!HasScript(hash))
		{
			OE_CORE_ASSERT(false, "Script of `typeid().hash_code() = {}` is not attached to Entity {0:x} (named `{}`)!", hash, AttachedEntity.GetComponent<IDComponent>().ID, AttachedEntity.GetComponent<NameComponent>().Name);
			return;
		}

		if (Runtime)
		{
			auto& script = Scripts.at(hash);
			script.DestroyScript(&script);
		}

		Scripts.erase(hash);
	}
}
