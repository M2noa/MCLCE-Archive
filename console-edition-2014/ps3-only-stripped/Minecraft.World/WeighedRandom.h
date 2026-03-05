#pragma once
// 4J - this WeighedRandomItem class was a nested static class within WeighedRandom, but we need to be able to refer to it externally

class WeighedRandomItem
{
	friend class WeighedRandom;
protected:
	int randomWeight;

public:
	WeighedRandomItem(int randomWeight)
	{
        this->randomWeight = randomWeight;
    }
};

class WeighedRandom
{
public:
	// 4J - vectors here were Collection<? extends WeighedRandomItem>
	static int getTotalWeight(vector<WeighedRandomItem *> *items);	
	static WeighedRandomItem *getRandomItem(Random *random, vector<WeighedRandomItem *> *items, int totalWeight);
	static WeighedRandomItem *getRandomItem(Random *random, vector<WeighedRandomItem *> *items);
	static int getTotalWeight(WeighedRandomItemArray items);
	static WeighedRandomItem *getRandomItem(Random *random, WeighedRandomItemArray items, int totalWeight);
	static WeighedRandomItem *getRandomItem(Random *random, WeighedRandomItemArray items);

};
