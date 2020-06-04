#pragma once

#include "OverEngine/Core/Core.h"
#include "Layer.h"

#include <vector>

namespace OverEngine {

	class OVER_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		Vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		Vector<Layer*>::iterator end() { return m_Layers.end(); }
		Vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		Vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

		Vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
		Vector<Layer*>::const_iterator end()	const { return m_Layers.end(); }
		Vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
		Vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }
	private:
		Vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};

}
