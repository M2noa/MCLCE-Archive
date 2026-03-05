#include "stdafx.h"
#include "StatsCounter.h"
#include "..\Minecraft.World\Stat.h"
#include "..\Minecraft.World\Stats.h"
#include "..\Minecraft.World\Achievement.h"
#include "..\Minecraft.World\Achievements.h"
#include "..\Minecraft.Client\LocalPlayer.h"

#include "..\Minecraft.World\net.minecraft.world.level.tile.h"
#include "..\Minecraft.World\net.minecraft.world.item.h"

#include "..\Minecraft.Client\Common\Leaderboards\LeaderboardManager.h"

Stat** StatsCounter::LARGE_STATS[] = {
	&Stats::walkOneM,
	&Stats::swimOneM,
	&Stats::fallOneM,
	&Stats::climbOneM,
	&Stats::minecartOneM,
	&Stats::boatOneM,
	&Stats::pigOneM,
	&Stats::timePlayed
};

unordered_map<Stat*, int> StatsCounter::statBoards;

StatsCounter::StatsCounter()
{
	requiresSave = false;
	saveCounter = 0;
	modifiedBoards = 0;
	flushCounter = 0;
}

void StatsCounter::award(Stat* stat, unsigned int difficulty, unsigned int count)
{
	if( stat->isAchievement() )
		difficulty = 0;

	StatsMap::iterator val = stats.find(stat);
	if( val == stats.end() )
	{
		StatContainer newVal;
		newVal.stats[difficulty] = count;
		stats.insert( make_pair(stat, newVal) );
	}
	else
	{
		val->second.stats[difficulty] += count;

		if (stat != GenericStats::timePlayed())
			app.DebugPrintf("");

		//If value has wrapped, cap it to UINT_MAX
		if( val->second.stats[difficulty] < (val->second.stats[difficulty]-count) )
			val->second.stats[difficulty] = UINT_MAX;

		//If value is larger than USHRT_MAX and is not designated as large, cap it to USHRT_MAX
		if( val->second.stats[difficulty] > USHRT_MAX && !isLargeStat(stat) )
			val->second.stats[difficulty] = USHRT_MAX;
	}

	requiresSave = true;

	//If this stat is on a leaderboard, mark that leaderboard as needing updated
	unordered_map<Stat*, int>::iterator leaderboardEntry = statBoards.find(stat);
	if( leaderboardEntry != statBoards.end() )
	{
		app.DebugPrintf("[StatsCounter] award(): %X\n", leaderboardEntry->second << difficulty);
		modifiedBoards |= (leaderboardEntry->second << difficulty);
		if( flushCounter == 0 )
			flushCounter = FLUSH_DELAY;
	}
}

bool StatsCounter::hasTaken(Achievement *ach)
{
	return stats.find(ach) != stats.end();
}

bool StatsCounter::canTake(Achievement *ach)
{
	//4J Gordon: Remove achievement dependencies, always able to take
	return true;
}

unsigned int StatsCounter::getValue(Stat *stat, unsigned int difficulty)
{
	StatsMap::iterator val = stats.find(stat);
	if( val != stats.end() )
		return val->second.stats[difficulty];
	return 0;
}

unsigned int StatsCounter::getTotalValue(Stat *stat)
{
	StatsMap::iterator val = stats.find(stat);
	if( val != stats.end() )
		return val->second.stats[0] + val->second.stats[1] + val->second.stats[2] + val->second.stats[3];
	return 0;
}

void StatsCounter::tick(int player)
{
	if( saveCounter > 0 )
		--saveCounter;

	if( requiresSave && saveCounter == 0 )
		save(player);

	// 4J-JEV, we don't want to write leaderboards in the middle of a game.
	// EDIT: Yes we do, people were not ending their games properly and not updating scores.
// #ifndef __PS3__
	if( flushCounter > 0 )
	{
		--flushCounter;
		if( flushCounter == 0 )
			flushLeaderboards();
	}
// #endif
}

void StatsCounter::clear()
{
	// clear out the stats when someone signs out
	stats.clear();
}

void StatsCounter::parse(void* data)
{
	// 4J-PB - If this is the trial game, let's just make sure all the stats are empty
	// 4J-PB - removing - someone can have the full game, and then remove it and go back to the trial
// 	if(!ProfileManager.IsFullVersion())
// 	{
// 		stats.clear();
// 		return;
// 	}
	//Check that we don't already have any stats
	assert( stats.size() == 0 );

	//Pointer to current position in stat array
	PBYTE pbData=(PBYTE)data;
	pbData+=sizeof(GAME_SETTINGS);
	unsigned short* statData = (unsigned short*)pbData;//data + (STAT_DATA_OFFSET/sizeof(unsigned short));

	//Value being read
	StatContainer newVal;

	//For each stat
	vector<Stat *>::iterator end = Stats::all->end();
	for( vector<Stat *>::iterator iter = Stats::all->begin() ; iter != end ; ++iter )
	{
		if( !(*iter)->isAchievement() )
		{
			if( !isLargeStat(*iter) )
			{
				if( statData[0] != 0 || statData[1] != 0 || statData[2] != 0 || statData[3] != 0 )
				{
					newVal.stats[0] = statData[0];
					newVal.stats[1] = statData[1];
					newVal.stats[2] = statData[2];
					newVal.stats[3] = statData[3];
					stats.insert( make_pair(*iter, newVal) );
				}
				statData += 4;
			}
			else
			{
				unsigned int* largeStatData = (unsigned int*)statData;
				if( largeStatData[0] != 0 || largeStatData[1] != 0 || largeStatData[2] != 0 || largeStatData[3] != 0 )
				{
					newVal.stats[0] = largeStatData[0];
					newVal.stats[1] = largeStatData[1];
					newVal.stats[2] = largeStatData[2];
					newVal.stats[3] = largeStatData[3];
					stats.insert( make_pair(*iter, newVal) );
				}
				largeStatData += 4;
				statData = (unsigned short*)largeStatData;
			}
		}
		else
		{
			if( statData[0] != 0 )
			{
				newVal.stats[0] = statData[0];
				newVal.stats[1] = 0;
				newVal.stats[2] = 0;
				newVal.stats[3] = 0;
				stats.insert( make_pair(*iter, newVal) );
			}
			++statData;
		}
	}

	dumpStatsToTTY();
}

void StatsCounter::save(int player, bool force)
{
	// 4J-PB - If this is the trial game, don't save any stats
	if(!ProfileManager.IsFullVersion())
	{
		return;
	}

	//Check we're going to have enough room to store all possible stats
	unsigned int uiTotalStatsSize = (Stats::all->size() * 4 * sizeof(unsigned short)) - (Achievements::achievements->size() * 3 * sizeof(unsigned short)) + (LARGE_STATS_COUNT*4*(sizeof(unsigned int)-sizeof(unsigned short)));
	assert( uiTotalStatsSize <= (CConsoleMinecraftApp::GAME_DEFINED_PROFILE_DATA_BYTES-sizeof(GAME_SETTINGS)) );

	//Retrieve the data pointer from the profile
#if defined __PS3__
	PBYTE pbData = (PBYTE)StorageManager.GetGameDefinedProfileData(player);
#else
	PBYTE pbData = (PBYTE)ProfileManager.GetGameDefinedProfileData(player);
#endif
	pbData+=sizeof(GAME_SETTINGS);
	
	//Pointer to current position in stat array
	//unsigned short* statData = (unsigned short*)data + (STAT_DATA_OFFSET/sizeof(unsigned short));
	unsigned short* statData = (unsigned short*)pbData;

	//Reset all the data to 0 (we're going to replace it with the map data)
	memset(statData, 0, CConsoleMinecraftApp::GAME_DEFINED_PROFILE_DATA_BYTES-sizeof(GAME_SETTINGS));

	//For each stat
	StatsMap::iterator val;
	vector<Stat *>::iterator end = Stats::all->end();
	for( vector<Stat *>::iterator iter = Stats::all->begin() ; iter != end ; ++iter )
	{
		//If the stat is in the map write out it's value
		val = stats.find(*iter);
		if( !(*iter)->isAchievement() )
		{
			if( !isLargeStat(*iter) )
			{
				if( val != stats.end() )
				{
					statData[0] = val->second.stats[0];
					statData[1] = val->second.stats[1];
					statData[2] = val->second.stats[2];
					statData[3] = val->second.stats[3];
				}
				statData += 4;
			}
			else
			{
				unsigned int* largeStatData = (unsigned int*)statData;
				if( val != stats.end() )
				{
					largeStatData[0] = val->second.stats[0];
					largeStatData[1] = val->second.stats[1];
					largeStatData[2] = val->second.stats[2];
					largeStatData[3] = val->second.stats[3];
				}
				largeStatData += 4;
				statData = (unsigned short*)largeStatData;
			}
		}
		else
		{
			if( val != stats.end() )
			{
				statData[0] = val->second.stats[0];
			}
			++statData;
		}
	}

#if defined __PS3__
	StorageManager.WriteToProfile(player, true, force);
#else
	ProfileManager.WriteToProfile(player, true, force);
#endif

	saveCounter = SAVE_DELAY;
}


void StatsCounter::flushLeaderboards()
{
	if( LeaderboardManager::Instance()->OpenSession() )
	{
		writeStats();
		LeaderboardManager::Instance()->FlushStats();
	}
	else
	{
		app.DebugPrintf("Failed to open a session in order to write to leaderboard\n");

		// 4J-JEV: If user was not signed in it would hit this.
		//assert(false);// && "Failed to open a session in order to write to leaderboard");
	}

	modifiedBoards = 0;
}

void StatsCounter::saveLeaderboards()
{
	// 4J-PB - If this is the trial game, no writing leaderboards
	if(!ProfileManager.IsFullVersion())
	{
		return;
	}

	if( LeaderboardManager::Instance()->OpenSession() )
	{
		writeStats();
		LeaderboardManager::Instance()->CloseSession();
	}
	else
	{
		app.DebugPrintf("Failed to open a session in order to write to leaderboard\n");

		// 4J-JEV: If user was not signed in it would hit this.
		//assert(false);// && "Failed to open a session in order to write to leaderboard");
	}

	modifiedBoards = 0;
}

void StatsCounter::writeStats()
{
	// 4J-PB - If this is the trial game, no writing
	if(!ProfileManager.IsFullVersion())
	{
		return;
	}
	//unsigned int locale = XGetLocale();

	int viewCount = 0;
	int iPad = ProfileManager.GetLockedProfile();

#if defined(__PS3__)
	LeaderboardManager::RegisterScore *scores = new LeaderboardManager::RegisterScore[24];

	if( modifiedBoards & LEADERBOARD_KILLS_EASY )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_KILLS_EASY\n");
		scores[viewCount].m_difficulty = 1;
		 
		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Kills;

		scores[viewCount].m_commentData.m_kills.m_zombie = getValue(Stats::killsZombie,				eDifficulty_Easy);
		scores[viewCount].m_commentData.m_kills.m_skeleton = getValue(Stats::killsSkeleton,			eDifficulty_Easy);
		scores[viewCount].m_commentData.m_kills.m_creeper = getValue(Stats::killsCreeper,			eDifficulty_Easy);
		scores[viewCount].m_commentData.m_kills.m_spider = getValue(Stats::killsSpider,				eDifficulty_Easy);
		scores[viewCount].m_commentData.m_kills.m_spiderJockey = getValue(Stats::killsSpiderJockey,	eDifficulty_Easy);
		//scores[viewCount].m_commentData.m_kills.m_zombiePigman = getValue(Stats::killsZombiePigman,		eDifficulty_Easy);
		scores[viewCount].m_commentData.m_kills.m_zombiePigman = getValue(Stats::killsNetherZombiePigman,	eDifficulty_Easy);
		scores[viewCount].m_commentData.m_kills.m_slime = getValue(Stats::killsSlime,						eDifficulty_Easy);

		scores[viewCount].m_score = scores[viewCount].m_commentData.m_kills.m_zombie
								  + scores[viewCount].m_commentData.m_kills.m_skeleton
								  + scores[viewCount].m_commentData.m_kills.m_creeper
								  + scores[viewCount].m_commentData.m_kills.m_spider
								  + scores[viewCount].m_commentData.m_kills.m_spiderJockey
									// + getValue(Stats::killsZombiePigman,			eDifficulty_Easy);
								  + scores[viewCount].m_commentData.m_kills.m_zombiePigman
								  + scores[viewCount].m_commentData.m_kills.m_slime;

		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_KILLS_NORMAL )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_KILLS_NORMAL\n");
		scores[viewCount].m_difficulty = 2;
		  
		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Kills;

		scores[viewCount].m_commentData.m_kills.m_zombie = getValue(Stats::killsZombie,				eDifficulty_Normal);
		scores[viewCount].m_commentData.m_kills.m_skeleton = getValue(Stats::killsSkeleton,			eDifficulty_Normal);
		scores[viewCount].m_commentData.m_kills.m_creeper = getValue(Stats::killsCreeper,			eDifficulty_Normal);
		scores[viewCount].m_commentData.m_kills.m_spider = getValue(Stats::killsSpider,				eDifficulty_Normal);
		scores[viewCount].m_commentData.m_kills.m_spiderJockey = getValue(Stats::killsSpiderJockey,	eDifficulty_Normal);
		//scores[viewCount].m_commentData.m_kills.m_zombiePigman = getValue(Stats::killsZombiePigman,		eDifficulty_Normal);
		scores[viewCount].m_commentData.m_kills.m_zombiePigman = getValue(Stats::killsNetherZombiePigman,	eDifficulty_Normal);
		scores[viewCount].m_commentData.m_kills.m_slime = getValue(Stats::killsSlime,						eDifficulty_Normal);

		scores[viewCount].m_score = scores[viewCount].m_commentData.m_kills.m_zombie
								  + scores[viewCount].m_commentData.m_kills.m_skeleton
								  + scores[viewCount].m_commentData.m_kills.m_creeper
								  + scores[viewCount].m_commentData.m_kills.m_spider
								  + scores[viewCount].m_commentData.m_kills.m_spiderJockey
									// + getValue(Stats::killsZombiePigman,			eDifficulty_Normal);
								  + scores[viewCount].m_commentData.m_kills.m_zombiePigman
								  + scores[viewCount].m_commentData.m_kills.m_slime;

		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_KILLS_HARD )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_KILLS_HARD\n");
		scores[viewCount].m_difficulty = 3;

		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Kills;

		scores[viewCount].m_commentData.m_kills.m_zombie = getValue(Stats::killsZombie,				eDifficulty_Hard);
		scores[viewCount].m_commentData.m_kills.m_skeleton = getValue(Stats::killsSkeleton,			eDifficulty_Hard);
		scores[viewCount].m_commentData.m_kills.m_creeper = getValue(Stats::killsCreeper,			eDifficulty_Hard);
		scores[viewCount].m_commentData.m_kills.m_spider = getValue(Stats::killsSpider,				eDifficulty_Hard);
		scores[viewCount].m_commentData.m_kills.m_spiderJockey = getValue(Stats::killsSpiderJockey,	eDifficulty_Hard);
		//scores[viewCount].m_commentData.m_kills.m_zombiePigman = getValue(Stats::killsZombiePigman,		eDifficulty_Hard);
		scores[viewCount].m_commentData.m_kills.m_zombiePigman = getValue(Stats::killsNetherZombiePigman,	eDifficulty_Hard);
		scores[viewCount].m_commentData.m_kills.m_slime = getValue(Stats::killsSlime,						eDifficulty_Hard);

		scores[viewCount].m_score = scores[viewCount].m_commentData.m_kills.m_zombie
								  + scores[viewCount].m_commentData.m_kills.m_skeleton
								  + scores[viewCount].m_commentData.m_kills.m_creeper
								  + scores[viewCount].m_commentData.m_kills.m_spider
								  + scores[viewCount].m_commentData.m_kills.m_spiderJockey
									// + getValue(Stats::killsZombiePigman,			eDifficulty_Hard);
								  + scores[viewCount].m_commentData.m_kills.m_zombiePigman
								  + scores[viewCount].m_commentData.m_kills.m_slime;

		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_MININGBLOCKS_PEACEFUL )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_MININGBLOCKS_PEACEFUL\n");
		scores[viewCount].m_difficulty = 0;

		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Mining;

		scores[viewCount].m_commentData.m_mining.m_dirt = getValue(Stats::blocksMined[Tile::dirt->id],				eDifficulty_Peaceful); 
		scores[viewCount].m_commentData.m_mining.m_cobblestone = getValue(Stats::blocksMined[Tile::stoneBrick->id],	eDifficulty_Peaceful); 
		scores[viewCount].m_commentData.m_mining.m_sand = getValue(Stats::blocksMined[Tile::sand->id],				eDifficulty_Peaceful); 
		scores[viewCount].m_commentData.m_mining.m_stone = getValue(Stats::blocksMined[Tile::rock->id],				eDifficulty_Peaceful); 
		scores[viewCount].m_commentData.m_mining.m_gravel = getValue(Stats::blocksMined[Tile::gravel->id],			eDifficulty_Peaceful); 
		scores[viewCount].m_commentData.m_mining.m_clay = getValue(Stats::blocksMined[Tile::clay->id],				eDifficulty_Peaceful); 
		scores[viewCount].m_commentData.m_mining.m_obsidian = getValue(Stats::blocksMined[Tile::obsidian->id],		eDifficulty_Peaceful);

		scores[viewCount].m_score = scores[viewCount].m_commentData.m_mining.m_dirt
								  + scores[viewCount].m_commentData.m_mining.m_cobblestone
								  + scores[viewCount].m_commentData.m_mining.m_sand
								  + scores[viewCount].m_commentData.m_mining.m_stone
								  + scores[viewCount].m_commentData.m_mining.m_gravel
								  + scores[viewCount].m_commentData.m_mining.m_clay
								  + scores[viewCount].m_commentData.m_mining.m_obsidian;

		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_MININGBLOCKS_EASY )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_MININGBLOCKS_EASY\n");
		scores[viewCount].m_difficulty = 1;

		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Mining;

		scores[viewCount].m_commentData.m_mining.m_dirt = getValue(Stats::blocksMined[Tile::dirt->id],				eDifficulty_Easy); 
		scores[viewCount].m_commentData.m_mining.m_cobblestone = getValue(Stats::blocksMined[Tile::stoneBrick->id],	eDifficulty_Easy); 
		scores[viewCount].m_commentData.m_mining.m_sand = getValue(Stats::blocksMined[Tile::sand->id],				eDifficulty_Easy); 
		scores[viewCount].m_commentData.m_mining.m_stone = getValue(Stats::blocksMined[Tile::rock->id],				eDifficulty_Easy); 
		scores[viewCount].m_commentData.m_mining.m_gravel = getValue(Stats::blocksMined[Tile::gravel->id],			eDifficulty_Easy); 
		scores[viewCount].m_commentData.m_mining.m_clay = getValue(Stats::blocksMined[Tile::clay->id],				eDifficulty_Easy); 
		scores[viewCount].m_commentData.m_mining.m_obsidian = getValue(Stats::blocksMined[Tile::obsidian->id],		eDifficulty_Easy);

		scores[viewCount].m_score = scores[viewCount].m_commentData.m_mining.m_dirt
								  + scores[viewCount].m_commentData.m_mining.m_cobblestone
								  + scores[viewCount].m_commentData.m_mining.m_sand
								  + scores[viewCount].m_commentData.m_mining.m_stone
								  + scores[viewCount].m_commentData.m_mining.m_gravel
								  + scores[viewCount].m_commentData.m_mining.m_clay
								  + scores[viewCount].m_commentData.m_mining.m_obsidian;

		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_MININGBLOCKS_NORMAL )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_MININGBLOCKS_NORMAL\n");
		scores[viewCount].m_difficulty = 2;

		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Mining;

		scores[viewCount].m_commentData.m_mining.m_dirt = getValue(Stats::blocksMined[Tile::dirt->id],				eDifficulty_Normal); 
		scores[viewCount].m_commentData.m_mining.m_cobblestone = getValue(Stats::blocksMined[Tile::stoneBrick->id],	eDifficulty_Normal); 
		scores[viewCount].m_commentData.m_mining.m_sand = getValue(Stats::blocksMined[Tile::sand->id],				eDifficulty_Normal); 
		scores[viewCount].m_commentData.m_mining.m_stone = getValue(Stats::blocksMined[Tile::rock->id],				eDifficulty_Normal); 
		scores[viewCount].m_commentData.m_mining.m_gravel = getValue(Stats::blocksMined[Tile::gravel->id],			eDifficulty_Normal); 
		scores[viewCount].m_commentData.m_mining.m_clay = getValue(Stats::blocksMined[Tile::clay->id],				eDifficulty_Normal); 
		scores[viewCount].m_commentData.m_mining.m_obsidian = getValue(Stats::blocksMined[Tile::obsidian->id],		eDifficulty_Normal);

		scores[viewCount].m_score = scores[viewCount].m_commentData.m_mining.m_dirt
								  + scores[viewCount].m_commentData.m_mining.m_cobblestone
								  + scores[viewCount].m_commentData.m_mining.m_sand
								  + scores[viewCount].m_commentData.m_mining.m_stone
								  + scores[viewCount].m_commentData.m_mining.m_gravel
								  + scores[viewCount].m_commentData.m_mining.m_clay
								  + scores[viewCount].m_commentData.m_mining.m_obsidian;

		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_MININGBLOCKS_HARD )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_MININGBLOCKS_HARD\n");
		scores[viewCount].m_difficulty = 3;

		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Mining;

		scores[viewCount].m_commentData.m_mining.m_dirt = getValue(Stats::blocksMined[Tile::dirt->id],				eDifficulty_Hard); 
		scores[viewCount].m_commentData.m_mining.m_cobblestone = getValue(Stats::blocksMined[Tile::stoneBrick->id],	eDifficulty_Hard); 
		scores[viewCount].m_commentData.m_mining.m_sand = getValue(Stats::blocksMined[Tile::sand->id],				eDifficulty_Hard); 
		scores[viewCount].m_commentData.m_mining.m_stone = getValue(Stats::blocksMined[Tile::rock->id],				eDifficulty_Hard); 
		scores[viewCount].m_commentData.m_mining.m_gravel = getValue(Stats::blocksMined[Tile::gravel->id],			eDifficulty_Hard); 
		scores[viewCount].m_commentData.m_mining.m_clay = getValue(Stats::blocksMined[Tile::clay->id],				eDifficulty_Hard); 
		scores[viewCount].m_commentData.m_mining.m_obsidian = getValue(Stats::blocksMined[Tile::obsidian->id],		eDifficulty_Hard);

		scores[viewCount].m_score = scores[viewCount].m_commentData.m_mining.m_dirt
								  + scores[viewCount].m_commentData.m_mining.m_cobblestone
								  + scores[viewCount].m_commentData.m_mining.m_sand
								  + scores[viewCount].m_commentData.m_mining.m_stone
								  + scores[viewCount].m_commentData.m_mining.m_gravel
								  + scores[viewCount].m_commentData.m_mining.m_clay
								  + scores[viewCount].m_commentData.m_mining.m_obsidian;

		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_FARMING_PEACEFUL )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_FARMING_PEACEFUL\n");
		scores[viewCount].m_difficulty = 0;

		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Farming;

		scores[viewCount].m_commentData.m_farming.m_eggs = getValue(Stats::itemsCollected[Item::egg->id],			eDifficulty_Peaceful);
		scores[viewCount].m_commentData.m_farming.m_wheat = getValue(Stats::blocksMined[Tile::crops_Id],			eDifficulty_Peaceful);
		scores[viewCount].m_commentData.m_farming.m_mushroom = getValue(Stats::blocksMined[Tile::mushroom1_Id],		eDifficulty_Peaceful); 
		scores[viewCount].m_commentData.m_farming.m_sugarcane = getValue(Stats::blocksMined[Tile::reeds_Id],		eDifficulty_Peaceful);
		scores[viewCount].m_commentData.m_farming.m_milk = getValue(Stats::cowsMilked,								eDifficulty_Peaceful);
		scores[viewCount].m_commentData.m_farming.m_pumpkin = getValue(Stats::itemsCollected[Tile::pumpkin->id],	eDifficulty_Peaceful);

		scores[viewCount].m_score = scores[viewCount].m_commentData.m_farming.m_eggs
								  + scores[viewCount].m_commentData.m_farming.m_wheat
								  + scores[viewCount].m_commentData.m_farming.m_mushroom 
								  + scores[viewCount].m_commentData.m_farming.m_sugarcane
								  + scores[viewCount].m_commentData.m_farming.m_milk 
								  + scores[viewCount].m_commentData.m_farming.m_pumpkin;
		
		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_FARMING_EASY )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_FARMING_EASY\n");
		scores[viewCount].m_difficulty = 1;

		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Farming;

		scores[viewCount].m_commentData.m_farming.m_eggs = getValue(Stats::itemsCollected[Item::egg->id],			eDifficulty_Easy);
		scores[viewCount].m_commentData.m_farming.m_wheat = getValue(Stats::blocksMined[Tile::crops_Id],			eDifficulty_Easy);
		scores[viewCount].m_commentData.m_farming.m_mushroom = getValue(Stats::blocksMined[Tile::mushroom1_Id],		eDifficulty_Easy); 
		scores[viewCount].m_commentData.m_farming.m_sugarcane = getValue(Stats::blocksMined[Tile::reeds_Id],		eDifficulty_Easy);
		scores[viewCount].m_commentData.m_farming.m_milk = getValue(Stats::cowsMilked,								eDifficulty_Easy);
		scores[viewCount].m_commentData.m_farming.m_pumpkin = getValue(Stats::itemsCollected[Tile::pumpkin->id],	eDifficulty_Easy);

		scores[viewCount].m_score = scores[viewCount].m_commentData.m_farming.m_eggs
								  + scores[viewCount].m_commentData.m_farming.m_wheat
								  + scores[viewCount].m_commentData.m_farming.m_mushroom 
								  + scores[viewCount].m_commentData.m_farming.m_sugarcane
								  + scores[viewCount].m_commentData.m_farming.m_milk 
								  + scores[viewCount].m_commentData.m_farming.m_pumpkin;
		
		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_FARMING_NORMAL )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_FARMING_NORMAL\n");
		scores[viewCount].m_difficulty = 2;

		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Farming;

		scores[viewCount].m_commentData.m_farming.m_eggs = getValue(Stats::itemsCollected[Item::egg->id],			eDifficulty_Normal);
		scores[viewCount].m_commentData.m_farming.m_wheat = getValue(Stats::blocksMined[Tile::crops_Id],			eDifficulty_Normal);
		scores[viewCount].m_commentData.m_farming.m_mushroom = getValue(Stats::blocksMined[Tile::mushroom1_Id],		eDifficulty_Normal); 
		scores[viewCount].m_commentData.m_farming.m_sugarcane = getValue(Stats::blocksMined[Tile::reeds_Id],		eDifficulty_Normal);
		scores[viewCount].m_commentData.m_farming.m_milk = getValue(Stats::cowsMilked,								eDifficulty_Normal);
		scores[viewCount].m_commentData.m_farming.m_pumpkin = getValue(Stats::itemsCollected[Tile::pumpkin->id],	eDifficulty_Normal);

		scores[viewCount].m_score = scores[viewCount].m_commentData.m_farming.m_eggs
								  + scores[viewCount].m_commentData.m_farming.m_wheat
								  + scores[viewCount].m_commentData.m_farming.m_mushroom 
								  + scores[viewCount].m_commentData.m_farming.m_sugarcane
								  + scores[viewCount].m_commentData.m_farming.m_milk 
								  + scores[viewCount].m_commentData.m_farming.m_pumpkin;
		
		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_FARMING_HARD )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_FARMING_HARD\n");
		scores[viewCount].m_difficulty = 3;

		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Farming;

		scores[viewCount].m_commentData.m_farming.m_eggs = getValue(Stats::itemsCollected[Item::egg->id],			eDifficulty_Hard);
		scores[viewCount].m_commentData.m_farming.m_wheat = getValue(Stats::blocksMined[Tile::crops_Id],			eDifficulty_Hard);
		scores[viewCount].m_commentData.m_farming.m_mushroom = getValue(Stats::blocksMined[Tile::mushroom1_Id],		eDifficulty_Hard); 
		scores[viewCount].m_commentData.m_farming.m_sugarcane = getValue(Stats::blocksMined[Tile::reeds_Id],		eDifficulty_Hard);
		scores[viewCount].m_commentData.m_farming.m_milk = getValue(Stats::cowsMilked,								eDifficulty_Hard);
		scores[viewCount].m_commentData.m_farming.m_pumpkin = getValue(Stats::itemsCollected[Tile::pumpkin->id],	eDifficulty_Hard);

		scores[viewCount].m_score = scores[viewCount].m_commentData.m_farming.m_eggs
								  + scores[viewCount].m_commentData.m_farming.m_wheat
								  + scores[viewCount].m_commentData.m_farming.m_mushroom 
								  + scores[viewCount].m_commentData.m_farming.m_sugarcane
								  + scores[viewCount].m_commentData.m_farming.m_milk 
								  + scores[viewCount].m_commentData.m_farming.m_pumpkin;
		
		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_TRAVELLING_PEACEFUL )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_TRAVELLING_PEACEFUL\n");
		scores[viewCount].m_difficulty = 0;
		
		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Travelling;

		scores[viewCount].m_commentData.m_travelling.m_walked = getValue(Stats::walkOneM,		eDifficulty_Peaceful);
		scores[viewCount].m_commentData.m_travelling.m_fallen = getValue(Stats::fallOneM,		eDifficulty_Peaceful);
		scores[viewCount].m_commentData.m_travelling.m_minecart = getValue(Stats::minecartOneM,	eDifficulty_Peaceful);
		scores[viewCount].m_commentData.m_travelling.m_boat = getValue(Stats::boatOneM,			eDifficulty_Peaceful);
		
		scores[viewCount].m_score = scores[viewCount].m_commentData.m_travelling.m_walked
								  + scores[viewCount].m_commentData.m_travelling.m_fallen
								  + scores[viewCount].m_commentData.m_travelling.m_minecart
								  + scores[viewCount].m_commentData.m_travelling.m_boat;

		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_TRAVELLING_EASY )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_TRAVELLING_EASY\n");
		scores[viewCount].m_difficulty = 1;

		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Travelling;

		scores[viewCount].m_commentData.m_travelling.m_walked = getValue(Stats::walkOneM,		eDifficulty_Easy);
		scores[viewCount].m_commentData.m_travelling.m_fallen = getValue(Stats::fallOneM,		eDifficulty_Easy);
		scores[viewCount].m_commentData.m_travelling.m_minecart = getValue(Stats::minecartOneM,	eDifficulty_Easy);
		scores[viewCount].m_commentData.m_travelling.m_boat = getValue(Stats::boatOneM,			eDifficulty_Easy);
		
		scores[viewCount].m_score = scores[viewCount].m_commentData.m_travelling.m_walked
								  + scores[viewCount].m_commentData.m_travelling.m_fallen
								  + scores[viewCount].m_commentData.m_travelling.m_minecart
								  + scores[viewCount].m_commentData.m_travelling.m_boat;

		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_TRAVELLING_NORMAL)
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_TRAVELLING_NORMAL\n");
		scores[viewCount].m_difficulty = 2;

		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Travelling;

		scores[viewCount].m_commentData.m_travelling.m_walked = getValue(Stats::walkOneM,		eDifficulty_Normal);
		scores[viewCount].m_commentData.m_travelling.m_fallen = getValue(Stats::fallOneM,		eDifficulty_Normal);
		scores[viewCount].m_commentData.m_travelling.m_minecart = getValue(Stats::minecartOneM,	eDifficulty_Normal);
		scores[viewCount].m_commentData.m_travelling.m_boat = getValue(Stats::boatOneM,			eDifficulty_Normal);
		
		scores[viewCount].m_score = scores[viewCount].m_commentData.m_travelling.m_walked
								  + scores[viewCount].m_commentData.m_travelling.m_fallen
								  + scores[viewCount].m_commentData.m_travelling.m_minecart
								  + scores[viewCount].m_commentData.m_travelling.m_boat;

		viewCount++;
	}

	if( modifiedBoards & LEADERBOARD_TRAVELLING_HARD )
	{
		scores[viewCount].m_iPad = iPad;

		app.DebugPrintf("Updating leaderboard view LEADERBOARD_TRAVELLING_HARD\n");
		scores[viewCount].m_difficulty = 3;

		scores[viewCount].m_commentData.m_statsType = LeaderboardManager::eStatsType_Travelling;

		scores[viewCount].m_commentData.m_travelling.m_walked = getValue(Stats::walkOneM,		eDifficulty_Hard);
		scores[viewCount].m_commentData.m_travelling.m_fallen = getValue(Stats::fallOneM,		eDifficulty_Hard);
		scores[viewCount].m_commentData.m_travelling.m_minecart = getValue(Stats::minecartOneM,	eDifficulty_Hard);
		scores[viewCount].m_commentData.m_travelling.m_boat = getValue(Stats::boatOneM,			eDifficulty_Hard);
		
		scores[viewCount].m_score = scores[viewCount].m_commentData.m_travelling.m_walked
								  + scores[viewCount].m_commentData.m_travelling.m_fallen
								  + scores[viewCount].m_commentData.m_travelling.m_minecart
								  + scores[viewCount].m_commentData.m_travelling.m_boat;

		viewCount++;
	}

	if( modifiedBoards & (LEADERBOARD_TRAVELLING_PEACEFUL | LEADERBOARD_TRAVELLING_EASY | LEADERBOARD_TRAVELLING_NORMAL | LEADERBOARD_TRAVELLING_HARD) )
	{
		app.DebugPrintf("Updating leaderboard view LEADERBOARD_TRAVELLING_PEACEFUL | LEADERBOARD_TRAVELLING_EASY | LEADERBOARD_TRAVELLING_NORMAL | LEADERBOARD_TRAVELLING_HARD\n");
		
		//viewCount++;
	}

	if( viewCount > 0 )
	{
		if( !LeaderboardManager::Instance()->WriteStats(viewCount, scores) )
		{
			app.DebugPrintf("Failed to write to leaderboard\n");
			assert(false);// && "Failed to write to leaderboard");
			//printf("Failed to write to leaderboard");
		}
		else
		{
			app.DebugPrintf("Successfully wrote %d leadeboard views\n", viewCount);
		}
	}

#endif
}

void StatsCounter::setupStatBoards()
{
	statBoards.insert( make_pair(Stats::killsZombie, LEADERBOARD_KILLS_PEACEFUL) );
	statBoards.insert( make_pair(Stats::killsSkeleton, LEADERBOARD_KILLS_PEACEFUL) );
	statBoards.insert( make_pair(Stats::killsCreeper, LEADERBOARD_KILLS_PEACEFUL) );
	statBoards.insert( make_pair(Stats::killsSpider, LEADERBOARD_KILLS_PEACEFUL) );
	statBoards.insert( make_pair(Stats::killsSpiderJockey, LEADERBOARD_KILLS_PEACEFUL) );
	statBoards.insert( make_pair(Stats::killsZombiePigman, LEADERBOARD_KILLS_PEACEFUL) );
	statBoards.insert( make_pair(Stats::killsNetherZombiePigman, LEADERBOARD_KILLS_PEACEFUL) );
	statBoards.insert( make_pair(Stats::killsSlime, LEADERBOARD_KILLS_PEACEFUL) );

	statBoards.insert( make_pair(Stats::blocksMined[Tile::dirt->id], LEADERBOARD_MININGBLOCKS_PEACEFUL) );
	statBoards.insert( make_pair(Stats::blocksMined[Tile::stoneBrick->id], LEADERBOARD_MININGBLOCKS_PEACEFUL) );
	statBoards.insert( make_pair(Stats::blocksMined[Tile::sand->id], LEADERBOARD_MININGBLOCKS_PEACEFUL) );
	statBoards.insert( make_pair(Stats::blocksMined[Tile::rock->id], LEADERBOARD_MININGBLOCKS_PEACEFUL) );
	statBoards.insert( make_pair(Stats::blocksMined[Tile::gravel->id], LEADERBOARD_MININGBLOCKS_PEACEFUL) );
	statBoards.insert( make_pair(Stats::blocksMined[Tile::clay->id], LEADERBOARD_MININGBLOCKS_PEACEFUL) );
	statBoards.insert( make_pair(Stats::blocksMined[Tile::obsidian->id], LEADERBOARD_MININGBLOCKS_PEACEFUL) );
 
	statBoards.insert( make_pair(Stats::itemsCollected[Item::egg->id], LEADERBOARD_FARMING_PEACEFUL) );
	statBoards.insert( make_pair(Stats::blocksMined[Tile::crops_Id], LEADERBOARD_FARMING_PEACEFUL) );
	statBoards.insert( make_pair(Stats::blocksMined[Tile::mushroom1_Id], LEADERBOARD_FARMING_PEACEFUL) );
	statBoards.insert( make_pair(Stats::blocksMined[Tile::reeds_Id], LEADERBOARD_FARMING_PEACEFUL) );
	statBoards.insert( make_pair(Stats::cowsMilked, LEADERBOARD_FARMING_PEACEFUL) );
	statBoards.insert( make_pair(Stats::itemsCollected[Tile::pumpkin->id], LEADERBOARD_FARMING_PEACEFUL) );

	statBoards.insert( make_pair(Stats::walkOneM, LEADERBOARD_TRAVELLING_PEACEFUL) );
	statBoards.insert( make_pair(Stats::fallOneM, LEADERBOARD_TRAVELLING_PEACEFUL) );
	statBoards.insert( make_pair(Stats::minecartOneM, LEADERBOARD_TRAVELLING_PEACEFUL) );
	statBoards.insert( make_pair(Stats::boatOneM, LEADERBOARD_TRAVELLING_PEACEFUL) );
}

bool StatsCounter::isLargeStat(Stat* stat)
{
	Stat*** end = &LARGE_STATS[LARGE_STATS_COUNT];
	for( Stat*** iter = LARGE_STATS ; iter != end ; ++iter )
		if( (*(*iter))->id == stat->id )
			return true;
	return false;
}

void StatsCounter::dumpStatsToTTY()
{
	vector<Stat*>::iterator statsEnd = Stats::all->end();
	for( vector<Stat*>::iterator statsIter = Stats::all->begin() ; statsIter!=statsEnd ; ++statsIter )
	{
		app.DebugPrintf("%ls\t\t%u\t%u\t%u\t%u\n",
			(*statsIter)->name.c_str(),
			getValue(*statsIter, 0),
			getValue(*statsIter, 1),
			getValue(*statsIter, 2),
			getValue(*statsIter, 3)
			);
	}
}

#ifdef _DEBUG

//To clear leaderboards set DEBUG_ENABLE_CLEAR_LEADERBOARDS to 1 and set DEBUG_CLEAR_LEADERBOARDS to be the bitmask of what you want to clear
//Leaderboards are updated on game exit so enter and exit a level to trigger the clear

//#define DEBUG_CLEAR_LEADERBOARDS			(LEADERBOARD_KILLS_EASY | LEADERBOARD_KILLS_NORMAL | LEADERBOARD_KILLS_HARD)
#define DEBUG_CLEAR_LEADERBOARDS			(0xFFFFFFFF)
#define DEBUG_ENABLE_CLEAR_LEADERBOARDS

void StatsCounter::WipeLeaderboards()
{

}
#endif
