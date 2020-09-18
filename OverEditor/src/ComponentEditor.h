#pragma once

#include "UIElements.h"

#include <OverEngine/Scene/Components.h>
#include <imgui/imgui.h>

// Base
template <class T>
void ComponentEditor(Entity entity, uint32_t typeID)
{
	if (OverEditor::UIElements::BeginComponentEditor<T>(entity, entity.GetComponent<T>().GetName(), typeID))
		ImGui::TextUnformatted("No Overloaded function for this component found!");
}

// Overloads
template <>
void ComponentEditor<TransformComponent>(Entity entity, uint32_t typeID)
{
	if (OverEditor::UIElements::BeginComponentEditor<TransformComponent>(entity, "Transform Component", typeID))
	{
		OverEditor::UIElements::BeginFieldGroup();

		auto& transform = entity.GetComponent<TransformComponent>();
		Vector3 position = transform.GetPosition();
		Vector3 rotation = transform.GetEulerAngles();
		Vector3 scale = transform.GetScale();

		if (OverEditor::UIElements::DragFloat3Field("Position", "##Position", glm::value_ptr(position), 0.2f))
			transform.SetPosition(position);

		if (OverEditor::UIElements::DragFloat3Field("Rotation", "##Rotation", glm::value_ptr(rotation), 0.2f))
			transform.SetEulerAngles(rotation);

		if (OverEditor::UIElements::DragFloat3Field("Scale", "##Scale", glm::value_ptr(scale), 0.2f))
			transform.SetScale(scale);

		OverEditor::UIElements::EndFieldGroup();
	}
}

template <>
void ComponentEditor<SpriteRendererComponent>(Entity entity, uint32_t typeID)
{
	if (OverEditor::UIElements::BeginComponentEditor<SpriteRendererComponent>(entity, "SpriteRenderer Component", typeID))
	{
		OverEditor::UIElements::BeginFieldGroup();

		auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();

		OverEditor::UIElements::Color4Field("Tint", "##Tint", glm::value_ptr(spriteRenderer.Tint));
		OverEditor::UIElements::DragFloatField("AlphaClippingThreshold",
			"##AlphaClippingThreshold", &spriteRenderer.AlphaClippingThreshold, 1.0f, 0.0f, 1.0f);
		OverEditor::UIElements::Texture2DAssetField("Sprite", "##Sprite", spriteRenderer.Sprite);

		if (spriteRenderer.Sprite)
		{
			OverEditor::UIElements::CheckboxField("FlipX", "##FlipX", &spriteRenderer.FlipX);
			OverEditor::UIElements::CheckboxField("FlipY", "##FlipY", &spriteRenderer.FlipY);

			static OverEditor::UIElements::EnumValues wrappingValues = {
				{ 0, "None (Use texture default value)" }, { 1, "Repeat" },
				{ 2, "MirroredRepeat" }, { 3, "ClampToEdge" },{ 4, "ClampToBorder" }
			};
			OverEditor::UIElements::BasicEnum("SWrapping", "##SWrapping", wrappingValues, (int*)&spriteRenderer.OverrideSWrapping);
			OverEditor::UIElements::BasicEnum("TWrapping", "##TWrapping", wrappingValues, (int*)&spriteRenderer.OverrideTWrapping);

			static OverEditor::UIElements::EnumValues filteringValues = {
				{ 0, "None (Use texture default value)" }, { 1, "Point" }, { 2, "Linear" }
			};
			OverEditor::UIElements::BasicEnum("Filtering", "##Filtering", filteringValues, (int*)&spriteRenderer.OverrideFiltering);

			OverEditor::UIElements::DragFloatField("TilingFactorX", "##TilingFactorX", &spriteRenderer.TilingFactorX);
			OverEditor::UIElements::DragFloatField("TilingFactorY", "##TilingFactorY", &spriteRenderer.TilingFactorY);

			OverEditor::UIElements::CheckboxField("OverrideTextureBorderColor (for ClampToBorder wrapping)",
				"##OverrideTextureBorderColor", &spriteRenderer.IsOverrideTextureBorderColor);

			if (spriteRenderer.IsOverrideTextureBorderColor)
				OverEditor::UIElements::Color4Field("BorderColor (for ClampToBorder wrapping)",
					"##BorderColor", glm::value_ptr(spriteRenderer.OverrideTextureBorderColor));
		}

		OverEditor::UIElements::EndFieldGroup();
	}
}

template <>
void ComponentEditor<CameraComponent>(Entity entity, uint32_t typeID)
{
	if (OverEditor::UIElements::BeginComponentEditor<CameraComponent>(entity, "Camera Component", typeID))
	{
		OverEditor::UIElements::BeginFieldGroup();

		auto& camera = entity.GetComponent<CameraComponent>();

		float orthoSize = camera.Camera.GetOrthographicSize();

		bool isClearingColor = camera.Camera.GetIsClearingColor();
		bool isClearingDepth = camera.Camera.GetIsClearingDepth();
		Color clearColor = camera.Camera.GetClearColor();

		float ZNear = camera.Camera.GetZNear();
		float ZFar = camera.Camera.GetZFar();

		// TODO: Perspective Camera
		if (OverEditor::UIElements::DragFloatField("Orthographic Size", "##Orthographic Size", &orthoSize, 0.5f, 0.0001f, FLT_MAX))
			camera.Camera.SetOrthographicSize(orthoSize);

		if (OverEditor::UIElements::DragFloatField("ZNear", "##ZNear", &ZNear, 0.5f))
			camera.Camera.SetZNear(ZNear);
		if (OverEditor::UIElements::DragFloatField("ZFar", "##ZFar", &ZFar, 0.5f))
			camera.Camera.SetZFar(ZFar);

		if (OverEditor::UIElements::Color4Field("Clear Color", "##Clear Color", glm::value_ptr(clearColor)))
			camera.Camera.SetClearColor(clearColor);

		if (OverEditor::UIElements::CheckboxField("Is Clearing Color", "##Is Clearing Color", &isClearingColor))
			camera.Camera.SetIsClearingColor(isClearingColor);
		if (OverEditor::UIElements::CheckboxField("Is Clearing Depth", "##Is Clearing Depth", &isClearingDepth))
			camera.Camera.SetIsClearingDepth(isClearingDepth);

		OverEditor::UIElements::EndFieldGroup();
	}
}
