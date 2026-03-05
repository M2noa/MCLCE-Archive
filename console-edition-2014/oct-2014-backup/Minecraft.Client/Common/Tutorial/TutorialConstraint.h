#pragma once

// 4J Stu - An abstract class that represents a constraint on what the user is able to do
class TutorialConstraint
{
private:
	int descriptionId;
	bool m_deleteOnDeactivate;
	bool m_queuedForRemoval;
public:
	enum ConstraintType
	{
		e_ConstraintInput = 0, // Constraint on controller input
		e_ConstraintArea,
		e_ConstraintAllInput,
		e_ConstraintXuiInput,
		e_ConstraintChangeState,
	};

	TutorialConstraint(int descriptionId) : descriptionId( descriptionId ), m_deleteOnDeactivate( false ), m_queuedForRemoval( false ) {}
	virtual ~TutorialConstraint() {}

	int getDescriptionId() { return descriptionId; }

	virtual ConstraintType getType() = 0;

	virtual void tick(int iPad) {}
	virtual bool isConstraintSatisfied(int iPad) { return true; }
	virtual bool isConstraintRestrictive(int iPad) { return true; }

	virtual bool isMappingConstrained(int iPad, int mapping) { return false;}
	virtual bool isXuiInputConstrained(int vk) { return false;}

	void setDeleteOnDeactivate(bool deleteOnDeactivated) { m_deleteOnDeactivate = deleteOnDeactivated; }
	bool getDeleteOnDeactivate() { return m_deleteOnDeactivate; }

	void setQueuedForRemoval(bool queued) { m_queuedForRemoval = queued; }
	bool getQueuedForRemoval() { return m_queuedForRemoval; }

	virtual bool canMoveToPosition(double xo, double yo, double zo, double xt, double yt, double zt) { return true; }
};