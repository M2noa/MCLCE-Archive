/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

template <typename EntityType, typename ConditionType, typename RuntimeType>
inline bool _tryInitializeComponent(EntityType *e, ConditionType && condition, Unique<RuntimeType> &component) {
	// Check whether the component should be initialized
	if (bool(condition)) {
		if (!component) {
			component = make_unique<RuntimeType>(*e);
		}
		// Run initialization code
		component->initFromDefinition();
		return true;
	}
	else {
		component.reset();
		return false;
	}
}

template<typename EntityType, typename ConditionType, typename RuntimeType, typename PredicateType> 
inline void _tryInitializeComponent(EntityType* e, ConditionType&& condition, Unique<RuntimeType> &component, PredicateType&& predicate) {
	if (!bool(predicate)) {
		return;
	}
	_tryInitializeComponent(e, condition, component);
}

template<typename EntityType, typename ConditionType, typename RuntimeType, typename PredicateType>
inline void _tryReloadComponent(EntityType* e, ConditionType&& condition, Unique<RuntimeType> &component, PredicateType&& predicate) {
	if (!bool(predicate) || !bool(condition)) {
		return;
	}	
	component->reloadComponent();
}

class CompoundTag;

template <typename EntityType, typename ConditionType, typename RuntimeType>
inline bool _tryLoadComponent(EntityType *e, ConditionType && condition, Unique<RuntimeType> &component, const CompoundTag &t) {
	// Check whether the component should be loaded
	if (bool(condition)) {
		if (!component) {
			component = make_unique<RuntimeType>(*e);
		}
		// Run initialization code
		component->initFromDefinition();
		component->readAdditionalSaveData(t);
		return true;
	}
	else {
		component.reset();
		return false;
	}
}
