#pragma once

// Forward Declarations.
class LeaderboardManager;

// READ LISTENTER //
class LeaderboardReadListener;

// ABSTRACT CLASS //
class LeaderboardManager
{
public:
	enum eStatsReturn
	{
		eStatsReturn_Success=0,
		eStatsReturn_NoResults,
		eStatsReturn_NetworkError
	};

	enum eProperty_Kills
	{
		eProperty_Kills_Zombie=0,
		eProperty_Kills_Skeleton,
		eProperty_Kills_Creeper,
		eProperty_Kills_Spider,
		eProperty_Kills_SpiderJockey,
		eProperty_Kills_ZombiePigman,
		eProperty_Kills_Slime,
		eProperty_Kills_Rating,
		eProperty_Kills_Max,
	};

	enum eProperty_Mining
	{
		eProperty_Mining_Dirt=0,
		eProperty_Mining_Stone,
		eProperty_Mining_Sand,
		eProperty_Mining_Cobblestone,
		eProperty_Mining_Gravel,
		eProperty_Mining_Clay,
		eProperty_Mining_Obsidian,
		eProperty_Mining_Rating,
		eProperty_Mining_Max,
	};

	enum eProperty_Farming
	{
		eProperty_Farming_Egg=0,
		eProperty_Farming_Wheat,
		eProperty_Farming_Mushroom,
		eProperty_Farming_Sugarcane,
		eProperty_Farming_Milk,
		eProperty_Farming_Pumpkin,
		eProperty_Farming_Rating,
		eProperty_Farming_Max,
	};

	enum eProperty_Travelling
	{
		eProperty_Travelling_Walked=0,
		eProperty_Travelling_Fallen,
		eProperty_Travelling_Minecart,
		eProperty_Travelling_Boat,
		eProperty_Travelling_Rating,
		eProperty_Travelling_Max,
	};

	enum EStatsType
	{
		eStatsType_Travelling=0,
		eStatsType_Mining,
		eStatsType_Farming,
		eStatsType_Kills,
		eStatsType_MAX,
		eStatsType_UNDEFINED
	};

	enum EFilterMode
	{
		eFM_Friends=0,		//Stats belonging to current user's friends
		eFM_MyScore,		//Stats around the current user's rank
		eFM_TopRank,		//Stats at the top of the leaderboard
		eNumFilterModes,
		eFM_UNDEFINED
	};	

	static const wstring filterNames[eNumFilterModes];

	typedef struct KillsRecord
	{
	public:
		unsigned short m_zombie;
		unsigned short m_skeleton;
		unsigned short m_creeper;
		unsigned short m_spider;
		unsigned short m_spiderJockey;
		unsigned short m_zombiePigman;
		unsigned short m_slime;
	} KillsRecord;

	typedef struct MiningRecord
	{
	public:
		unsigned short m_dirt;
		unsigned short m_stone;
		unsigned short m_sand;
		unsigned short m_cobblestone;
		unsigned short m_gravel;
		unsigned short m_clay;
		unsigned short m_obsidian;
	} MiningRecord;

	typedef struct FarmingRecord
	{
	public:
		unsigned short m_eggs;
		unsigned short m_wheat;
		unsigned short m_mushroom;
		unsigned short m_sugarcane;
		unsigned short m_milk;
		unsigned short m_pumpkin;
	} FarmingRecord;

	typedef struct TravellingRecord
	{
	public:
		unsigned int m_walked;
		unsigned int m_fallen;
		unsigned int m_minecart;
		unsigned int m_boat;
	} TravellingRecord;

	

public:
#ifdef _XBOX
	typedef XSESSION_VIEW_PROPERTIES *ViewIn;
	typedef PXUSER_STATS_READ_RESULTS ViewOut;
#else
	static const int RECORD_SIZE = 40; //base32

	typedef struct StatsData
	{
		EStatsType m_statsType;
		union
		{
			LeaderboardManager::KillsRecord m_kills;
			LeaderboardManager::MiningRecord m_mining;
			LeaderboardManager::FarmingRecord m_farming;
			LeaderboardManager::TravellingRecord m_travelling;
			unsigned char m_padding[RECORD_SIZE];
		};
	} StatsData;

	typedef struct RegisterScore
	{
		int m_iPad;
		int m_score;
		int m_difficulty;
		StatsData m_commentData;
	} RegisterScore;

	typedef struct ReadScore
	{
		// Maximum number of columns in a scoreboard.
		static const unsigned int STATSDATA_MAX = 8;

		PlayerUID		m_uid;						// Player's unique identifier.
		unsigned long	m_rank;						// Rank of the player on this scoreboard.
		wstring			m_name;						// Player's display name.
		
		unsigned long	m_totalScore;				// Sum of all the player's scores on this leaderboard.

		unsigned short	m_statsSize;				// Iff (m_hasResults): Number of columns on this leaderboard.
		unsigned long	m_statsData[STATSDATA_MAX];	// Iff (m_hasResults): Player's score for each appropriate column.

		int				m_idsErrorMessage;			// Iff (not m_hasResults): error message explaining what went wrong.

	} ReadScore;

	typedef struct ReadView
	{
		unsigned int m_numQueries;
		ReadScore *m_queries;

	} ReadView;
	
	typedef ReadView ViewOut;
	typedef RegisterScore *ViewIn;
#endif


public:
	LeaderboardManager();
	virtual ~LeaderboardManager() {}

protected:
	virtual void zeroReadParameters();

	EFilterMode m_eFilterMode;

	// Parameters for reading.
	int m_difficulty;
	EStatsType m_statsType;
	LeaderboardReadListener *m_readListener;
	PlayerUID m_myXUID; 
	unsigned int m_startIndex, m_readCount;

private:
	static LeaderboardManager *m_instance; //Singleton instance of the LeaderboardManager

public:
	static LeaderboardManager *Instance() { return m_instance; }
	static void DeleteInstance();

	virtual void Tick() = 0;

	//Open a session
	virtual bool OpenSession() = 0;

	//Close a session
	virtual void CloseSession() = 0;

	//Delete a session
	virtual void DeleteSession() = 0;

	//Write the given stats
	//This is called synchronously and will not free any memory allocated for views when it is done

	virtual bool WriteStats(unsigned int viewCount, ViewIn views) = 0;

	virtual bool ReadStats_Friends(LeaderboardReadListener *callback, int difficulty, EStatsType type, PlayerUID myUID, unsigned int startIndex, unsigned int readCount);
	virtual bool ReadStats_MyScore(LeaderboardReadListener *callback, int difficulty, EStatsType type, PlayerUID myUID, unsigned int readCount);
	virtual bool ReadStats_TopRank(LeaderboardReadListener *callback, int difficulty, EStatsType type, unsigned int startIndex, unsigned int readCount);

	//Perform a flush of the stats
	virtual void FlushStats() = 0;

	//Cancel the current operation
	virtual void CancelOperation() = 0;

	//Is the leaderboard manager idle.
	virtual bool isIdle() = 0;

public:
#ifndef _XBOX
	static void printStats(ReadView &view);
#endif
};

class LeaderboardReadListener
{
public:
#ifdef _XBOX
	virtual bool OnStatsReadComplete(bool success, int numResults, LeaderboardManager::ViewOut results) = 0;
#else
	virtual bool OnStatsReadComplete(LeaderboardManager::eStatsReturn ret, int numResults, LeaderboardManager::ViewOut results) = 0;
#endif
};

#ifndef _XBOX
class DebugReadListener : public LeaderboardReadListener
{
public:
	virtual bool OnStatsReadComplete(LeaderboardManager::eStatsReturn ret, int numResults, LeaderboardManager::ViewOut results);
	static DebugReadListener *m_instance;
};
#endif
