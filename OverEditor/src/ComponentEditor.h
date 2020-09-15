#pragma once

#include <OverEngine/Scene/Components.h>
#include <imgui/imgui.h>

static const char* RemoveComponentButtonText = "X";

// Base
template <class T>
void ComponentEditor(Entity entity, uint32_t typeID)
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 10, ImGui::GetStyle().FramePadding.y });

	char txt[100];
	sprintf_s(txt, sizeof(txt) / sizeof(char), "%s##%i", RemoveComponentButtonText, typeID);

	bool componentRemoved = false;
	if (ImGui::Button(txt))
	{
		entity.RemoveComponent<T>();
		componentRemoved = true;
	}

	sprintf_s(txt, sizeof(txt) / sizeof(char), "##%i", typeID);
	if (!componentRemoved)
	{
		ImGui::SameLine();
		ImGui::Checkbox(txt, &entity.GetComponent<T>().Enabled);
	}

	ImGui::SameLine();

	sprintf_s(txt, sizeof(txt) / sizeof(char), "##MOVE_COMPONENT_UP%i", typeID);

	if (ImGui::ArrowButton(txt, ImGuiDir_Up))
	{
		auto& componentList = entity.GetComponentsTypeIDList();
		auto it = std::find(componentList.begin(), componentList.end(), typeID);
		if (it != componentList.begin())
		{
			componentList[it - componentList.begin()] = componentList[it - componentList.begin() - 1];
			componentList[it - componentList.begin() - 1] = typeID;
		}
	}

	ImGui::SameLine();

	sprintf_s(txt, sizeof(txt) / sizeof(char), "##MOVE_COMPONENT_DOWN%i", typeID);

	if (ImGui::ArrowButton(txt, ImGuiDir_Down))
	{
		auto& componentList = entity.GetComponentsTypeIDList();
		auto it = std::find(componentList.begin(), componentList.end(), typeID);
		if (it != componentList.end() - 1)
		{
			componentList[it - componentList.begin()] = componentList[it - componentList.begin() + 1];
			componentList[it - componentList.begin() + 1] = typeID;
		}
	}

	ImGui::PopStyleVar();

	ImGui::SameLine();

	if (!componentRemoved)
	{
		if (ImGui::CollapsingHeader(entity.GetComponent<T>().GetName()))
		{
			ImGui::Indent(30.f);
			ImGui::TextUnformatted("No Overloaded function for this component found!");
			ImGui::Unindent(30.f);
		}
	}
}

// Overloads
template <>
void ComponentEditor<TransformComponent>(Entity entity, uint32_t typeID)
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 10, ImGui::GetStyle().FramePadding.y });

	char txt[100];
	sprintf_s(txt, sizeof(txt) / sizeof(char), "%s##%i", RemoveComponentButtonText, typeID);

	bool componentRemoved = false;
	if (ImGui::Button(txt))
	{
		entity.RemoveComponent<TransformComponent>();
		componentRemoved = true;
	}

	sprintf_s(txt, sizeof(txt) / sizeof(char), "##%i", typeID);
	if (!componentRemoved)
	{
		ImGui::SameLine();
		ImGui::Checkbox(txt, &entity.GetComponent<TransformComponent>().Enabled);
	}

	ImGui::SameLine();

	sprintf_s(txt, sizeof(txt) / sizeof(char), "##MOVE_COMPONENT_UP%i", typeID);

	if (ImGui::ArrowButton(txt, ImGuiDir_Up))
	{
		auto& componentList = entity.GetComponentsTypeIDList();
		auto it = std::find(componentList.begin(), componentList.end(), typeID);
		if (it != componentList.begin())
		{
			componentList[it - componentList.begin()] = componentList[it - componentList.begin() - 1];
			componentList[it - componentList.begin() - 1] = typeID;
		}
	}

	ImGui::SameLine();

	sprintf_s(txt, sizeof(txt) / sizeof(char), "##MOVE_COMPONENT_DOWN%i", typeID);

	if (ImGui::ArrowButton(txt, ImGuiDir_Down))
	{
		auto& componentList = entity.GetComponentsTypeIDList();
		auto it = std::find(componentList.begin(), componentList.end(), typeID);
		if (it != componentList.end() - 1)
		{
			componentList[it - componentList.begin()] = componentList[it - componentList.begin() + 1];
			componentList[it - componentList.begin() + 1] = typeID;
		}
	}

	ImGui::PopStyleVar();

	ImGui::SameLine();

	if (!componentRemoved)
	{
		if (ImGui::CollapsingHeader("Transform Component"))
		{
			ImGui::Columns(2);

			auto& transform = entity.GetComponent<TransformComponent>();

			Vector3 position = transform.GetPosition();
			Vector3 rotation = transform.GetEulerAngles();
			Vector3 scale = transform.GetScale();

			ImGui::TextUnformatted("Position");
			ImGui::NextColumn();

			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat3("##Position", glm::value_ptr(position), 0.2f))
				transform.SetPosition(position);
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::TextUnformatted("Rotation");
			ImGui::NextColumn();

			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat3("##Rotation", glm::value_ptr(rotation), 0.2f))
				transform.SetEulerAngles(rotation);
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::TextUnformatted("Scale");
			ImGui::NextColumn();

			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat3("##Scale", glm::value_ptr(scale), 0.2f))
				transform.SetScale(scale);
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Columns(1);
		}
	}
}

template <>
void ComponentEditor<SpriteRendererComponent>(Entity entity, uint32_t typeID)
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 10, ImGui::GetStyle().FramePadding.y });

	char txt[100];
	sprintf_s(txt, sizeof(txt) / sizeof(char), "%s##%i", RemoveComponentButtonText, typeID);

	bool componentRemoved = false;
	if (ImGui::Button(txt))
	{
		entity.RemoveComponent<SpriteRendererComponent>();
		componentRemoved = true;
	}

	sprintf_s(txt, sizeof(txt) / sizeof(char), "##%i", typeID);
	if (!componentRemoved)
	{
		ImGui::SameLine();
		ImGui::Checkbox(txt, &entity.GetComponent<SpriteRendererComponent>().Enabled);
	}

	ImGui::SameLine();

	sprintf_s(txt, sizeof(txt) / sizeof(char), "##MOVE_COMPONENT_UP%i", typeID);

	if (ImGui::ArrowButton(txt, ImGuiDir_Up))
	{
		auto& componentList = entity.GetComponentsTypeIDList();
		auto it = std::find(componentList.begin(), componentList.end(), typeID);
		if (it != componentList.begin())
		{
			componentList[it - componentList.begin()] = componentList[it - componentList.begin() - 1];
			componentList[it - componentList.begin() - 1] = typeID;
		}
	}

	ImGui::SameLine();

	sprintf_s(txt, sizeof(txt) / sizeof(char), "##MOVE_COMPONENT_DOWN%i", typeID);

	if (ImGui::ArrowButton(txt, ImGuiDir_Down))
	{
		auto& componentList = entity.GetComponentsTypeIDList();
		auto it = std::find(componentList.begin(), componentList.end(), typeID);
		if (it != componentList.end() - 1)
		{
			componentList[it - componentList.begin()] = componentList[it - componentList.begin() + 1];
			componentList[it - componentList.begin() + 1] = typeID;
		}
	}

	ImGui::PopStyleVar();

	ImGui::SameLine();

	if (!componentRemoved)
	{
		if (ImGui::CollapsingHeader("SpriteRenderer Component"))
		{
			ImGui::Columns(2);
			auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();

			ImGui::TextUnformatted("Tint");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			ImGui::ColorEdit4("##Tint", &spriteRenderer.Tint.r);
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::TextUnformatted("Sprite");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);

			char spriteText[50];
			auto spriteTextSize = sizeof(spriteText) / sizeof(char);

			if (!spriteRenderer.Sprite)
				sprintf_s(spriteText, spriteTextSize, "None (Texture2DAsset)");
			else if (!spriteRenderer.Sprite->GetResource())
				sprintf_s(spriteText, spriteTextSize, "%s.%s",
					spriteRenderer.Sprite->GetResource()->GetName().c_str(),
					spriteRenderer.Sprite->GetName().c_str());
			else
				sprintf_s(spriteText, spriteTextSize, "%s", spriteRenderer.Sprite->GetName().c_str());

			ImGui::InputText("", spriteText, spriteTextSize, ImGuiInputTextFlags_ReadOnly);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_ASSET_DRAG"))
				{
					Ref<Texture2DAsset>* asset = static_cast<Ref<Texture2DAsset>*>(payload->Data);
					spriteRenderer.Sprite = *asset;
				}
				ImGui::EndDragDropTarget();
			}

			if (spriteRenderer.Sprite)
			{
				ImGui::NextColumn();

				ImGui::TextUnformatted("FlipX");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::Checkbox("##FlipX", &spriteRenderer.FlipX);
				ImGui::PopItemWidth();
				ImGui::NextColumn();

				ImGui::TextUnformatted("FlipY");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::Checkbox("##FlipY", &spriteRenderer.FlipY);
				ImGui::PopItemWidth();
				ImGui::NextColumn();

				static constexpr char* textureWrappingLabels[] = { "None (Use texture default value)", "Repeat", "MirroredRepeat", "ClampToEdge", "ClampToBorder" };
				static constexpr size_t textureWrappingLabelsSize = OE_ARRAY_SIZE(textureWrappingLabels);

				ImGui::TextUnformatted("SWrapping");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::Combo("##SWrapping", (int*)&spriteRenderer.OverrideSWrapping, textureWrappingLabels, textureWrappingLabelsSize);
				ImGui::PopItemWidth();
				ImGui::NextColumn();

				ImGui::TextUnformatted("TWrapping");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::Combo("##TWrapping", (int*)&spriteRenderer.OverrideTWrapping, textureWrappingLabels, textureWrappingLabelsSize);
				ImGui::PopItemWidth();
				ImGui::NextColumn();

				static constexpr char* textureFilteringLabels[] = { "None (Use texture default value)", "Point", "Linear" };
				static constexpr size_t textureFilteringLabelsSize = OE_ARRAY_SIZE(textureFilteringLabels);

				ImGui::TextUnformatted("Filtering");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::Combo("##Filtering", (int*)&spriteRenderer.OverrideFiltering, textureFilteringLabels, textureFilteringLabelsSize);
				ImGui::PopItemWidth();
				ImGui::NextColumn();

				ImGui::TextUnformatted("TilingFactorX");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::DragFloat("##TilingFactorX", &spriteRenderer.TilingFactorX);
				ImGui::PopItemWidth();
				ImGui::NextColumn();

				ImGui::TextUnformatted("TilingFactorY");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::DragFloat("##TilingFactorY", &spriteRenderer.TilingFactorY);
				ImGui::PopItemWidth();
			}

			ImGui::PopItemWidth();

			ImGui::Columns(1);
		}
	}
}

template <>
void ComponentEditor<CameraComponent>(Entity entity, uint32_t typeID)
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 10, ImGui::GetStyle().FramePadding.y });

	char txt[100];
	sprintf_s(txt, sizeof(txt) / sizeof(char), "%s##%i", RemoveComponentButtonText, typeID);

	bool componentRemoved = false;
	if (ImGui::Button(txt))
	{
		entity.RemoveComponent<CameraComponent>();
		componentRemoved = true;
	}

	sprintf_s(txt, sizeof(txt) / sizeof(char), "##%i", typeID);
	if (!componentRemoved)
	{
		ImGui::SameLine();
		ImGui::Checkbox(txt, &entity.GetComponent<CameraComponent>().Enabled);
	}

	ImGui::SameLine();

	sprintf_s(txt, sizeof(txt) / sizeof(char), "##MOVE_COMPONENT_UP%i", typeID);

	if (ImGui::ArrowButton(txt, ImGuiDir_Up))
	{
		auto& componentList = entity.GetComponentsTypeIDList();
		auto it = std::find(componentList.begin(), componentList.end(), typeID);
		if (it != componentList.begin())
		{
			componentList[it-componentList.begin()] = componentList[it-componentList.begin()-1];
			componentList[it-componentList.begin()-1] = typeID;
		}
	}

	ImGui::SameLine();

	sprintf_s(txt, sizeof(txt) / sizeof(char), "##MOVE_COMPONENT_DOWN%i", typeID);

	if (ImGui::ArrowButton(txt, ImGuiDir_Down))
	{
		auto& componentList = entity.GetComponentsTypeIDList();
		auto it = std::find(componentList.begin(), componentList.end(), typeID);
		if (it != componentList.end() - 1)
		{
			componentList[it-componentList.begin()] = componentList[it-componentList.begin()+1];
			componentList[it-componentList.begin()+1] = typeID;
		}
	}

	ImGui::PopStyleVar();

	ImGui::SameLine();

	if (!componentRemoved)
	{
		if (ImGui::CollapsingHeader("Camera Component"))
		{
			ImGui::Columns(2);

			auto& camera = entity.GetComponent<CameraComponent>();

			float orthoSize = camera.Camera.GetOrthographicSize();

			bool isClearingColor = camera.Camera.GetIsClearingColor();
			bool isClearingDepth = camera.Camera.GetIsClearingDepth();
			Color clearColor = camera.Camera.GetClearColor();

			float ZNear = camera.Camera.GetZNear();
			float ZFar = camera.Camera.GetZFar();

			// TODO: Perspective Camera
			ImGui::Text("Orthographic Size");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat("##Orthographic Size", &orthoSize, 0.5f, 0.0001f, FLT_MAX))
				camera.Camera.SetOrthographicSize(orthoSize);
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Text("ZNear");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat("##ZNear", &ZNear, 0.5f))
				camera.Camera.SetZNear(ZNear);
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Text("ZFar");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::DragFloat("##ZFar", &ZFar, 0.5f))
				camera.Camera.SetZFar(ZFar);
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Text("Clear Color");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::ColorEdit4("##Clear Color", &clearColor.r))
				camera.Camera.SetClearColor(clearColor);
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Text("Is Clearing Color");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::Checkbox("##Is Clearing Color", &isClearingColor))
				camera.Camera.SetIsClearingColor(isClearingColor);
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Text("Is Clearing Depth");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::Checkbox("##Is Clearing Depth", &isClearingDepth))
				camera.Camera.SetIsClearingDepth(isClearingDepth);
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Columns(1);
		}
	}
}
