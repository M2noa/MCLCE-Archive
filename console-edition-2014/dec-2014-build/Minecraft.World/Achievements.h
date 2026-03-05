#pragma once
using namespace std;


class Achievement;

class Achievements 
{
	friend class Achievement;

protected:
	static const int ACHIEVEMENT_OFFSET;

    // maximum position of achievements (min and max)
public:
	static const int ACHIEVEMENT_WIDTH_POSITION = 12;
    static const int ACHIEVEMENT_HEIGHT_POSITION = 12;

    static int xMin, yMin, xMax, yMax;

    static vector<Achievement *> *achievements;

    static Achievement *openInventory;
    static Achievement *mineWood;
    static Achievement *buildWorkbench;
    static Achievement *buildPickaxe;
    static Achievement *buildFurnace;
    static Achievement *acquireIron;
    static Achievement *buildHoe;
    static Achievement *makeBread;
    static Achievement *bakeCake;
    static Achievement *buildBetterPickaxe;
    static Achievement *cookFish;
    static Achievement *onARail;
    static Achievement *buildSword;
    static Achievement *killEnemy;
    static Achievement *killCow;
    static Achievement *flyPig;

	// 4J-JEV: Present on the PS3.
	static Achievement *snipeSkeleton;
	static Achievement *diamonds;
	//static Achievement *portal; //4J-JEV: Whats this?
	static Achievement *ghast;
	static Achievement *blazeRod;
	static Achievement *potion;
	static Achievement *theEnd;
	static Achievement *winGame;
	static Achievement *enchantments;

	// 4J : WESTY : Added new acheivements. 
	static Achievement *leaderOfThePack;
	static Achievement *MOARTools;
	static Achievement *dispenseWithThis;
	static Achievement *InToTheNether;

	// 4J : WESTY : Added other awards. 
	static Achievement *socialPost;
	static Achievement *eatPorkChop;
	static Achievement *play100Days;
	static Achievement *arrowKillCreeper;
	static Achievement *mine100Blocks;
	static Achievement *kill10Creepers;

#ifdef _EXTENDED_ACHIEVEMENTS
	static Achievement *overkill; // Old achievements;
	static Achievement *bookcase; // Old achievements;
	static Achievement *adventuringTime;
	static Achievement *repopulation;
	static Achievement *diamondsToYou;
	static Achievement *passingTheTime;
	static Achievement *archer;
	static Achievement *theHaggler;
	static Achievement *potPlanter;
	static Achievement *itsASign;
	static Achievement *ironBelly;
	static Achievement *haveAShearfulDay;
	static Achievement *rainbowCollection;
	static Achievement *stayinFrosty;
	static Achievement *chestfulOfCobblestone;
	static Achievement *renewableEnergy;
	static Achievement *musicToMyEars;
	static Achievement *bodyGuard;
	static Achievement *ironMan;
	static Achievement *zombieDoctor;
	static Achievement *lionTamer;
#endif

	static void staticCtor();

    static void init();
};
