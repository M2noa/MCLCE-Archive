/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include <string>
#include <map>
#include <iostream>
#include "world/level/levelgen/structure/StructureTemplate.h"
#include "util/SharedLock.h"

class StructureManager {
public:
	StructureManager();
	StructureTemplate& getOrCreate(const std::string& structureName);
	bool load(StructureTemplate &structure, std::string& data);
	bool save(const std::string& structureName);

private:
	StructureTemplate* get(const std::string& structureName);
	StructureTemplate* _readStructure(const std::string& name);

	SharedMutex mRepositoryMutex;
	std::unordered_map<std::string, Unique<StructureTemplate>> mStructureRepository;
};
