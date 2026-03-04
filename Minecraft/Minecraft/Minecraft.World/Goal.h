#pragma once

class Goal
{
private:
	int _requiredControlFlags;

protected:
	Goal();
public:
	virtual ~Goal() {}
	virtual bool canUse() = 0;
	virtual bool canContinueToUse();
	virtual bool canInterrupt();
	virtual void start();
	virtual void stop();
	virtual void tick();
	virtual void setRequiredControlFlags(int requiredControlFlags);
	virtual int getRequiredControlFlags();
	
	// 4J Added override to update ai elements when loading entity from schematics
	virtual void setLevel(Level *level) {};
};