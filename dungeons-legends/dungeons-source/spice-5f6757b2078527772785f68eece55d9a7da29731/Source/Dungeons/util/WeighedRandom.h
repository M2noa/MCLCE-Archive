/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "util/Random.h"

class WeighedRandom {
public:

	class WeighedRandomItem {
	public:
		WeighedRandomItem()
			:   mRandomWeight(-128){
		}

		WeighedRandomItem(int randomWeight)
			:   mRandomWeight(randomWeight){
		}

		bool isValid() const {
			return mRandomWeight >= 0;
		}

		int getWeight() const {
			return mRandomWeight;
		}
	private:
		int mRandomWeight;
	};

	template<typename T>
	static int getTotalWeight(const T& items) {
		int totalWeight = 0;

		for (typename T::const_iterator it = items.begin(); it != items.end(); ++it) {
			totalWeight += it->getWeight();
		}

		return totalWeight;
	}

	template<typename T>
	static const T* getRandomItem(Random* random, const std::vector<T>& items, int totalWeight) {
		if (items.empty()) {
			return nullptr;
		}

		int selection = random->nextInt(totalWeight);

		for (typename std::vector<T>::const_iterator it = items.begin(); it != items.end(); ++it) {
			selection -= it->getWeight();
			if (selection < 0) {
				return &(*it);
			}
		}
		return nullptr;
	}

	template<typename T>
	static T* getRandomItem(Random* random, std::vector<T>& items, int totalWeight) {
		if (items.empty()) {
			return nullptr;
		}

		int selection = random->nextInt(totalWeight);

		for (typename std::vector<T>::iterator it = items.begin(); it != items.end(); ++it) {
			selection -= it->getWeight();
			if (selection < 0) {
				return &(*it);
			}
		}
		return nullptr;
	}

	template<typename T>
	static const T* getRandomItem(Random* random, const std::vector<T>& items) {
		return getRandomItem(random, items, getTotalWeight(items));
	}

	template<typename T>
	static T* getRandomItem(Random* random, std::vector<T>& items) {
		return getRandomItem(random, items, getTotalWeight(items));
	}

	template<typename T>
	static int getRandomItemIndex(Random* random, const T& items, int totalWeight) {
		int selection = random->nextInt(totalWeight);

		for (unsigned int i = 0; i < items.size(); ++i) {
			selection -= items[i].getWeight();
			if (selection < 0) {
				return i;
			}
		}
		return -1;
	}

	template<typename T>
	static int getRandomItemIndex(Random* random, const T& items) {
		return getRandomItemIndex(random, items, getTotalWeight(items));
	}

//     static int getTotalWeight(WeighedRandomItem items) {
//         int totalWeight = 0;
//
//         for (unsigned int i = 0; i < items.size(); ++i) {
//             WeighedRandomItem& item = items[i];
//             totalWeight += item.randomWeight;
//         }
//         return totalWeight;
//     }

//     static WeighedRandomItem getRandomItem(Random* random, WeighedRandomItem[] items, int totalWeight) {
//
//         if (totalWeight <= 0) {
//             throw /*new*/ IllegalArgumentException();
//         }
//
//         int selection = random->nextInt(totalWeight);
//         for (unsigned int i = 0; i < items.size(); ++i) {
//             WeighedRandomItem& item = items[i];
//         //for (auto&& item : items) {
//             selection -= item.randomWeight;
//             if (selection < 0) {
//                 return item;
//             }
//         }
//         return nullptr;
//     }

//     static WeighedRandomItem getRandomItem(Random* random, WeighedRandomItem[] items) {
//         return getRandomItem(random, items, getTotalWeight(items));
//     }
};
