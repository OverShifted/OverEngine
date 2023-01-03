#include "NeuralNetwork.h"

#include <Platform/OpenGL/OpenGLTexture.h>

#include <imgui.h>

using namespace OverEngine;

#define X 800
#define Y 800

#define NX 100.0f
#define NY 100.0f

float Activation(float in)
{
//	return 1 / (1 + glm::exp(-in));
	return glm::max(0.0f, in);
//	return in;
}

NeuralNetwork::NeuralNetwork()
	: Layer("NeuralNetwork"), m_Tex(Texture2D::Create()), model({2, 2, 1}) // Random GUID
{
	OpenGLTexture2D* tex = (OpenGLTexture2D*)m_Tex.get();
	tex->Allocate(X, Y, 3);

	tex->SetFilter(TextureFilter::Nearest);
}

void NeuralNetwork::OnUpdate(TimeStep deltaTime)
{
	RenderCommand::SetClearColor(Color{ 0.0, 0.1, 0.0, 1.0 });
	RenderCommand::Clear(ClearFlags_ClearColor | ClearFlags_ClearDepth);

	uint8_t data[X * Y * 3];
	uint32_t data_idx = 0;

	for (uint32_t x = 0; x < X; x++)
	{
	   for (uint32_t y = 0; y < Y; y++)
	   {
			float xf = (float)x, yf = (float)y;
			float out = model.evaluate({ Remap(xf, 0.0f, (float)X, -1000.0f, 1000.0f) , Remap(yf, 0.0f, (float)Y, -1000.0f, 1000.0f) })[0];
//			OE_INFO("{}", out);
//			data[data_idx] = (uint32_t)(out * 100);

			if (out > 0.5)
			{
//				data[data_idx] = 255;
//				data[data_idx + 1] = 10;
//				data[data_idx + 2] = 40;

				data[data_idx] = (uint8_t)(Clamp(out, 0.0f, 1.0f) * 254.0);
				data[data_idx + 1] = data[data_idx];
				data[data_idx + 2] = data[data_idx];
			}
			else
			{
//				data[data_idx] = 30;
//				data[data_idx + 1] = 10;
//				data[data_idx + 2] = 255;
				data[data_idx] = (uint8_t)(out * 254.0);
				data[data_idx + 1] = data[data_idx];
				data[data_idx + 2] = data[data_idx];
			}


			data_idx += 3;
	   }

//	   data_idx += 3;
	}

	OpenGLTexture2D* tex = (OpenGLTexture2D*)m_Tex.get();
	tex->Upload(data);

//    Renderer2D::BeginScene(IDENTITY_MAT4X4, m_Camera);
//    Renderer2D::EndScene();
}

void NeuralNetwork::OnImGuiRender()
{
	ImGui::Begin("Neural Network Settings");
	ImGui::Text("%f", model.evaluate({ 0.0f , 0.0f })[0]);

	ImGui::SliderFloat("B00", &model.m_Layers[0].m_biases[0], -1.0, 1.0);
	ImGui::SliderFloat("B01", &model.m_Layers[0].m_biases[1], -1.0, 1.0);

	ImGui::SliderFloat("W00", &model.m_Layers[0].m_weights[0], -1.0, 1.0);
	ImGui::SliderFloat("W01", &model.m_Layers[0].m_weights[1], -1.0, 1.0);
	ImGui::SliderFloat("W02", &model.m_Layers[0].m_weights[2], -1.0, 1.0);
	ImGui::SliderFloat("W03", &model.m_Layers[0].m_weights[3], -1.0, 1.0);

	ImGui::Separator();

	ImGui::SliderFloat("B10", &model.m_Layers[1].m_biases[0], -1.0, 1.0);

	ImGui::SliderFloat("W10", &model.m_Layers[1].m_weights[0], -1.0, 1.0);
	ImGui::SliderFloat("W11", &model.m_Layers[1].m_weights[1], -1.0, 1.0);


	ImGui::Image(m_Tex, {X, Y});
	ImGui::End();
}

void NeuralNetwork::OnEvent(Event& event)
{
	OE_PROFILE_FUNCTION();

//	EventDispatcher dispatcher(event);
//	dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) {
//	    this->m_Camera.SetViewportSize(e.GetWidth(), e.GetHeight());
//        return false;
//    });
}
