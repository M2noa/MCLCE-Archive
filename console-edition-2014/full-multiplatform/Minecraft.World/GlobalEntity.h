#pragma once
#include "Entity.h"

class Level;

//class GlobalEntity : public Entity
class GlobalEntity : public Entity
{
public:
	GlobalEntity(Level *level) : Entity( level ) {};
};