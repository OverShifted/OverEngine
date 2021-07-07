#include "pcheader.h"
#include "TransformComponent.h"

#include "OverEngine/Core/Runtime/Reflection/TypeInfo.h"

namespace OverEngine
{
	ObjectTypeInfo* TransformComponent::Reflect()
	{
		static ObjectTypeInfo* typeInfo = nullptr;

		if (!typeInfo)
		{
			typeInfo = new ObjectTypeInfo(GetStaticClassName(), sizeof(TransformComponent), dynamic_cast<ObjectTypeInfo*>(TypeResolver<Component>::Get()));

			ADD_GET_SET_PROPERTY_INSPECTOR_NAME(TransformComponent, LocalPosition, Position, GetLocalPosition, SetLocalPosition)
			ADD_GET_SET_PROPERTY_INSPECTOR_NAME(TransformComponent, LocalEulerAngles, Rotation, GetLocalEulerAngles, SetLocalEulerAngles)
			ADD_GET_SET_PROPERTY_INSPECTOR_NAME(TransformComponent, LocalScale, Scale, GetLocalScale, SetLocalScale)
		}

		return typeInfo;
	}

	#define ENTITY_FROM_HANDLE(handle) Entity{ handle, AttachedEntity.GetScene() }
	#define ENTITY_HANDLE_TRANSFORM(handle) ENTITY_FROM_HANDLE(handle).GetComponent<TransformComponent>()

	Vector3 TransformComponent::GetPosition() const
	{
		return m_LocalToWorld[3];
	}

	void TransformComponent::SetPosition(const Vector3& position)
	{
		if (m_Parent != entt::null)
		{
			auto parentTransform = ENTITY_HANDLE_TRANSFORM(m_Parent).GetLocalToWorld();

			parentTransform[0][0] = parentTransform[0][0] == 0.0f ? 1.0f : parentTransform[0][0];
			parentTransform[0][0] = parentTransform[0][0] == 0.0f ? 1.0f : parentTransform[0][0];
			parentTransform[0][0] = parentTransform[0][0] == 0.0f ? 1.0f : parentTransform[0][0];

			SetLocalPosition(Vector3(glm::inverse(parentTransform) * Vector4(position, 1.0f)));
		}
		else
		{
			SetLocalPosition(position);
		}
	}

	Vector3 TransformComponent::GetLocalPosition() const
	{
		return m_LocalPosition;
	}

	void TransformComponent::SetLocalPosition(const Vector3& position)
	{
		m_LocalPosition = position;
		Change();
	}

	Vector3 TransformComponent::GetEulerAngles()
	{
		if (m_Parent == entt::null)
			return GetLocalEulerAngles();

		return QuaternionToEulerAngles(GetRotation());
	}

	void TransformComponent::SetEulerAngles(const Vector3& rotation)
	{
		if (m_Parent != entt::null)
			SetLocalRotation(glm::quat_cast(glm::inverse(ENTITY_HANDLE_TRANSFORM(m_Parent).GetLocalToWorld())
				* glm::mat4_cast(EulerAnglesToQuaternion(rotation))));
		else
			SetLocalEulerAngles(rotation);
	}

	void TransformComponent::SetLocalEulerAngles(const Vector3& rotation)
	{
		m_LocalEulerAngles = rotation;
		m_LocalRotation = EulerAnglesToQuaternion(rotation);
		Change();
	}

	Quaternion TransformComponent::GetRotation()
	{
		if (m_Parent == entt::null)
			return GetLocalRotation();

		Mat3x3 rotationMat = {
			m_LocalToWorld[0].x, m_LocalToWorld[0].y, m_LocalToWorld[0].z,
			m_LocalToWorld[1].x, m_LocalToWorld[1].y, m_LocalToWorld[1].z,
			m_LocalToWorld[2].x, m_LocalToWorld[2].y, m_LocalToWorld[2].z,
		};

		Vector3 lossyScale = GetLossyScale();
		rotationMat[0] /= lossyScale.x;
		rotationMat[1] /= lossyScale.y;
		rotationMat[2] /= lossyScale.z;

		return glm::quat_cast(rotationMat);
	}

	void TransformComponent::SetRotation(const Quaternion& rotation)
	{
		if (m_Parent != entt::null)
			SetLocalRotation(glm::quat_cast(glm::inverse(ENTITY_HANDLE_TRANSFORM(m_Parent).GetLocalToWorld())
				* glm::mat4_cast(rotation)));
		else
			SetLocalRotation(rotation);
	}

	void TransformComponent::SetLocalRotation(const Quaternion& rotation)
	{
		m_LocalRotation = rotation;
		m_LocalEulerAngles = QuaternionToEulerAngles(rotation);
		Change();
	}

	void TransformComponent::SetLocalScale(const Vector3& scale)
	{
		m_LocalScale = scale;
		Change();
	}

	Vector3 TransformComponent::GetLossyScale() const
	{
		return {
			glm::fastLength(Vector3(m_LocalToWorld[0])),
			glm::fastLength(Vector3(m_LocalToWorld[1])),
			glm::fastLength(Vector3(m_LocalToWorld[2]))
		};
	}

	void TransformComponent::DetachFromParent()
	{
		if (m_Parent != entt::null)
		{
			auto& parentChildren = ENTITY_HANDLE_TRANSFORM(m_Parent).m_Children;
			auto it = STD_CONTAINER_FIND(parentChildren, AttachedEntity.GetRuntimeID());
			if (it != parentChildren.end())
				parentChildren.erase(it);

			m_Parent = entt::null;

			AttachedEntity.GetScene()->GetRootHandles().push_back(AttachedEntity.GetRuntimeID());

			Change();
		}
	}

	void TransformComponent::DetachChildren()
	{
		for (const auto& child : m_Children)
			ENTITY_HANDLE_TRANSFORM(child).DetachFromParent();
	}

	Entity TransformComponent::GetParent() const
	{
		return ENTITY_FROM_HANDLE(m_Parent);
	}

	void TransformComponent::SetParent(Entity parent)
	{
		if (parent)
		{
			// Add to parent children list
			ENTITY_HANDLE_TRANSFORM(parent).m_Children.push_back(AttachedEntity.GetRuntimeID());

			if (m_Parent == entt::null)
			{
				auto& sceneRootHandles = AttachedEntity.GetScene()->GetRootHandles();
				auto it = STD_CONTAINER_FIND(sceneRootHandles, AttachedEntity.GetRuntimeID());
				OE_CORE_ASSERT(it != sceneRootHandles.end(), "Cannot find Entity in scene root list!");
				sceneRootHandles.erase(it);
			}
			else
			{
				auto& parentChildren = ENTITY_HANDLE_TRANSFORM(m_Parent).m_Children;
				auto it = STD_CONTAINER_FIND(parentChildren, AttachedEntity.GetRuntimeID());
				OE_CORE_ASSERT(it != parentChildren.end(), "Cannot find Entity in parent's children list!");
				parentChildren.erase(it);
			}

			m_Parent = parent.GetRuntimeID();

			Change();
		}
		else
		{
			DetachFromParent();
		}
	}

	uint32_t TransformComponent::GetSiblingIndex() const
	{
		if (m_Parent == entt::null)
		{
			const auto& rootEntities = AttachedEntity.GetScene()->GetRootHandles();
			auto it = STD_CONTAINER_FIND(rootEntities, AttachedEntity.GetRuntimeID());
			return (uint32_t)(it - rootEntities.begin());
		}

		const auto& parentChildren = ENTITY_HANDLE_TRANSFORM(m_Parent).GetChildrenHandles();
		auto it = STD_CONTAINER_FIND(parentChildren, AttachedEntity.GetRuntimeID());
		return (uint32_t)(it - parentChildren.begin());
	}

	void TransformComponent::SetSiblingIndex(uint32_t index)
	{
		if (m_Parent == entt::null)
		{
			auto& rootEntities = AttachedEntity.GetScene()->GetRootHandles();
			auto it = STD_CONTAINER_FIND(rootEntities, AttachedEntity.GetRuntimeID());
			Move(rootEntities, it - rootEntities.begin(), index);

			return;
		}

		auto& parentChildren = ENTITY_HANDLE_TRANSFORM(m_Parent).GetChildrenHandles();
		auto it = STD_CONTAINER_FIND(parentChildren, AttachedEntity.GetRuntimeID());
		Move(parentChildren, it - parentChildren.begin(), index);
	}

	void TransformComponent::Invalidate()
	{
		if (m_ChangedFlags & ChangedFlags_Changed)
		{
			Mat4x4 localToParent = glm::mat4_cast(m_LocalRotation) * SCALE_MAT4X4(m_LocalScale);
			localToParent[3].x = m_LocalPosition.x;
			localToParent[3].y = m_LocalPosition.y;
			localToParent[3].z = m_LocalPosition.z;

			if (m_Parent != entt::null)
				m_LocalToWorld = ENTITY_HANDLE_TRANSFORM(m_Parent).GetLocalToWorld() * localToParent;
			else
				m_LocalToWorld = localToParent;
		}
	}

	// Add changed flag and force all children to update
	void TransformComponent::Change()
	{
		m_ChangedFlags |= ChangedFlags_Changed | ChangedFlags_ChangedForPhysics;
		Invalidate();

		for (const auto& child : m_Children)
			ENTITY_HANDLE_TRANSFORM(child).Change();
	}
}
