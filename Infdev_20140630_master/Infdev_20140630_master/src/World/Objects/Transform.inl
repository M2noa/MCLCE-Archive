#pragma once

#include "./Transform.h"

#include "General/Matrix.h"

namespace Esteem
{
	inline const cgc::strong_ptr<Transform> Transform::GetParent()
	{
		return parent;
	}

	inline void Transform::SetParent(const cgc::strong_ptr<Transform>& parent, const cgc::strong_ptr<Transform>& child)
	{
		parent->AddChild(parent, child);
	}

	inline const std::vector<cgc::strong_ptr<Transform>>& Transform::GetChilds() const
	{
		return childs;
	}
	
	inline const Vector3& Transform::GetPosition() const
	{
		return position;
	}
	
	inline void Transform::SetPosition(const Vector3& position)
	{
		this->position = position;
	}

	inline const Quaternion& Transform::GetRotation() const
	{
		return rotation;
	}
	
	inline void Transform::SetRotation(const Quaternion& rotation)
	{
		this->rotation = rotation;
	}

	inline const Vector3& Transform::GetScale() const
	{
		return scale;
	}

	inline void Transform::SetScale(const Vector3& scale)
	{
		this->scale = scale;
	}

	inline const Vector3& Transform::GetLocalPosition() const
	{
		return localPosition;
	}

	inline void Transform::SetLocalPosition(const Vector3& position)
	{
		this->localPosition = position;
	}

	inline const Quaternion& Transform::GetLocalRotation() const
	{
		return localRotation;
	}

	inline void Transform::SetLocalRotation(const Quaternion& rotation)
	{
		this->localRotation = rotation;
	}

	inline const Vector3& Transform::GetLocalScale() const
	{
		return localScale;
	}

	inline void Transform::SetLocalScale(const Vector3& scale)
	{
		this->localScale = scale;
	}

	inline const glm::mat3& Transform::GetDirections() const
	{
		return directions;
	}

	inline void Transform::SetDirections(const glm::mat3& directions)
	{
		this->directions = directions;
	}

	inline const Vector3& Transform::GetRight() const
	{
		return const_cast<glm::mat3&>(directions)[0]; // glm::mat3 returns copy on const
	}

	inline void Transform::SetRight(const Vector3& right)
	{
		directions[0] = right;
	}

	inline const Vector3& Transform::GetUp() const
	{
		return const_cast<glm::mat3&>(directions)[1]; // glm::mat3 returns copy on const
	}

	inline void Transform::SetUp(const Vector3& up)
	{
		directions[1] = up;
	}

	inline const Vector3& Transform::GetForward() const
	{
		return const_cast<glm::mat3&>(directions)[2]; // glm::mat3 returns copy on const
	}

	inline void Transform::SetForward(const Vector3& forward)
	{
		directions[2] = forward;
	}

	inline const glm::mat4& Transform::GetMatrix()
	{
		return matrix;
	}

	inline void Transform::SetMatrix(const glm::mat4& matrix)
	{
		this->matrix = matrix;
	}

	inline void Transform::UpdateMatrix()
	{
		this->directions = Matrix3x3(rotation);
		this->matrix = Matrix4x4(this->directions, scale, position);
	}

	inline World* Transform::GetWorld()
	{
		return this->world;
	}
}