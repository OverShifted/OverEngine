#pragma once

#include <OverEngine.h>

using namespace OverEngine;

float Activation(float in);

class NNLayer {
public:
    NNLayer(uint32_t nodesIn, uint32_t nodesOut)
        : m_nodesIn(nodesIn), m_nodesOut(nodesOut)
    {
		m_weights.resize(m_nodesIn * m_nodesOut);
		m_biases.resize(m_nodesOut);

		for (float& w : m_weights) {
			w = Random::Range(-1.0, 1.0);
		}

		for (float& b : m_biases) {
			b = Random::Range(-1.0, 1.0);
		}
    }

    ~NNLayer() = default;
	Vector<float> evaluate(const Vector<float>& in) const
    {
		Vector<float> out = m_biases;

        uint32_t currentWIndex = 0;
        for (uint32_t i = 0; i < m_nodesOut; i++)
        {
            for (uint32_t j = 0; j < m_nodesIn; j++)
            {
                out[i] += in[j] * m_weights[currentWIndex];
                currentWIndex++;
            }
        }

		for (auto& v : out)
		{
			v = Activation(v);
		}

        return out;
    }
//private:
    uint32_t m_nodesIn, m_nodesOut;
	Vector<float> m_weights;
	Vector<float> m_biases;
};

class NN {
public:
	NN(Vector<uint32_t> shape)
	{
		m_Layers.reserve(shape.size() - 1);
		for (uint32_t i = 1; i < shape.size(); i++)
		{
			OE_INFO("Creating Layer {} x {}", shape[i - 1], shape[i]);
			m_Layers.push_back(NNLayer(shape[i - 1], shape[i]));
		}
	}

	Vector<float> evaluate(const Vector<float>& in) const
	{
		Vector<float> current = in;

		for (const auto& layer : m_Layers) {
			current = layer.evaluate(current);
		}

		return current;

//		return m_Layers[0].evaluate(in);
	}

	Vector<NNLayer> m_Layers;
};

class NeuralNetwork : public Layer
{
public:
    NeuralNetwork();

    void OnUpdate(TimeStep deltaTime) override;
    void OnImGuiRender() override;
    void OnEvent(Event& event) override;

private:
//    SceneCamera m_Camera;
	Ref<Texture2D> m_Tex;
	NN model;
};
