#include "stdafx.h"
#include "WeighedRandom.h"

int WeighedRandom::getTotalWeight(vector<WeighedRandomItem *> *items)
{
    int totalWeight = 0;
	for( AUTO_VAR(it, items->begin()); it != items->end(); it++ )
	{
		totalWeight += (*it)->randomWeight;
	}
    return totalWeight;
}

WeighedRandomItem *WeighedRandom::getRandomItem(Random *random, vector<WeighedRandomItem *> *items, int totalWeight)
{
    if (totalWeight <= 0)
	{
        __debugbreak();
    }

	int selection = random->nextInt(totalWeight);

	for( AUTO_VAR(it, items->begin()); it != items->end(); it++ )
	{
		selection -= (*it)->randomWeight;
        if (selection < 0)
		{
            return *it;
        }
	}
    return NULL;
}

WeighedRandomItem *WeighedRandom::getRandomItem(Random *random, vector<WeighedRandomItem *> *items)
{
	return getRandomItem(random, items, getTotalWeight(items));
}

int WeighedRandom::getTotalWeight(WeighedRandomItemArray items)
{
    int totalWeight = 0;
	for( unsigned int i = 0; i < items.length; i++ )
	{
        totalWeight += items[i]->randomWeight;
    }
    return totalWeight;
}

WeighedRandomItem *WeighedRandom::getRandomItem(Random *random, WeighedRandomItemArray items, int totalWeight)
{
    if (totalWeight <= 0)
	{
        __debugbreak();
    }

    int selection = random->nextInt(totalWeight);
	for( unsigned int i = 0; i < items.length; i++ )
	{
		selection -= items[i]->randomWeight;
        if (selection < 0)
		{
            return items[i];
        }
	}
    return NULL;
}


WeighedRandomItem *WeighedRandom::getRandomItem(Random *random, WeighedRandomItemArray items)
{
	return getRandomItem(random, items, getTotalWeight(items));
}