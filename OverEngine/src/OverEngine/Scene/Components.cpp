#include "pcheader.h"
#include "Components.h"

namespace OverEngine
{
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
