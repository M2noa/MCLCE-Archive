#include "Transform.h"

#include "World/World.h"

#include "Utils/Debug.h"

namespace Esteem
{
	Transform::Transform(World* world)
		: world(world)
		, position(0.f)
		, rotation(1, 0, 0, 0)
		, scale(1.f)
		, localPosition(0.f)
		, localRotation(1, 0, 0, 0)
		, localScale(1.f)
		, directions(1.f)
		, matrix(1.f)
		, dirty(false)
	{

	}

	Transform::~Transform()
	{
		//world->GetData()->RemoveDirty(this);
	}

	void Transform::AddChild(const cgc::strong_ptr<Transform>& parent, const cgc::strong_ptr<Transform>& child)
	{
		auto& childs = parent->childs;

		auto found = std::find(childs.begin(), childs.end(), child);
		if (found == childs.end())
		{
			childs.push_back(child);
			child->parent = parent;

			child->localPosition = child->position - parent->position;
			child->localRotation = glm::inverse(parent->rotation) * child->rotation;
			child->localScale = child->scale / parent->scale;
		}
	}

	void Transform::RemoveChild(const cgc::strong_ptr<Transform>& child)
	{
		auto found = std::find(childs.begin(), childs.end(), child);
		if (found != childs.end())
		{
			childs.erase(found);
			child->parent = cgc::weak_ptr<Transform>();
		}
	}

	void Transform::DirtyCleanUp()
	{
		for (uint c = 0; c < childs.size(); ++c)
		{
			Transform* child = childs[c].ptr();
			child->scale = scale * child->localScale;
			child->position = position + (rotation * (scale * child->localPosition));
			child->rotation = rotation * child->localRotation;

			child->DirtyCleanUp();
		}

		dirty = false;
	}

	void Transform::SetDirty()
	{
		if (!dirty)
		{
			// TODO: enable dirty again
			dirty = world->AddDirty(this);
			if (!dirty)
				Debug::Log("set dirty not worked");
		}
	}
}