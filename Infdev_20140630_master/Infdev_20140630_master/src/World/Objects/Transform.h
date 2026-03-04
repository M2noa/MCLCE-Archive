#pragma once

#include <atomic>
#include <vector>
#include <glm/gtc/quaternion.hpp>

#include "General/Vector3.h"
#include "General/Quaternion.h"
#include "./IWorldObject.h"

#include <cppu/cgc/pointers.h>

namespace Esteem
{
	class Transform : public IWorldObject
	{
	private:
		Vector3 position;
		Vector3 scale;
		Quaternion rotation;

		Vector3 localPosition;
		Vector3 localScale;
		Quaternion localRotation;

		glm::mat3 directions;
		glm::mat4 matrix;

		cgc::weak_ptr<Transform> parent;
		std::vector<cgc::strong_ptr<Transform>> childs;

	protected:
		World* world;
		std::atomic<bool> dirty;

		// TODO: Add listener delegates when dirty state has being processed
		
	public:
		Transform(World* world);
		virtual ~Transform();

		const cgc::strong_ptr<Transform> GetParent();
		static void SetParent(const cgc::strong_ptr<Transform>& parent, const cgc::strong_ptr<Transform>& child);
		const std::vector<cgc::strong_ptr<Transform>>& GetChilds() const;
		static void AddChild(const cgc::strong_ptr<Transform>& parent, const cgc::strong_ptr<Transform>& child);
		void RemoveChild(const cgc::strong_ptr<Transform>& transform);

		const Vector3& GetPosition() const;
		void SetPosition(const Vector3& position);
		const Quaternion& GetRotation() const;
		void SetRotation(const Quaternion& rotation);
		const Vector3& GetScale() const;
		void SetScale(const Vector3& scale);

		const Vector3& GetLocalPosition() const;
		void SetLocalPosition(const Vector3& position);
		const Quaternion& GetLocalRotation() const;
		void SetLocalRotation(const Quaternion& rotation);
		const Vector3& GetLocalScale() const;
		void SetLocalScale(const Vector3& scale);

		const glm::mat3& GetDirections() const;
		void SetDirections(const glm::mat3& directions);

		const Vector3& GetForward() const;
		void SetForward(const Vector3& forward);
		const Vector3& GetRight() const;
		void SetRight(const Vector3& right);
		const Vector3& GetUp() const;
		void SetUp(const Vector3& up);
		
		/// \brief get pointer to matrix
		const glm::mat4& GetMatrix();
		/// \brief overwrite values of transform's matrix
		void SetMatrix(const glm::mat4& matrix);
		/// \brief Calculates new matrix from RST
		void UpdateMatrix();

		World* GetWorld();

		void SetDirty();
		virtual void DirtyCleanUp();
	};
}

#include "./Transform.inl"