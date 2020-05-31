#pragma once

#include "OverEngine/Core/Core.h"
#include "OverEngine/Events/Event.h"

namespace OverEngine {

	class OVER_API Layer
	{
	public:
		Layer(const String& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const String& GetName() const { return m_DebugName; }
	protected:
		String m_DebugName;
	};

}
