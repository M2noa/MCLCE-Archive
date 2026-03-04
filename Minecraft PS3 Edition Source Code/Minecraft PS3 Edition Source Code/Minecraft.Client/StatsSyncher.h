#pragma once
class File;
class StatsCounter;
class User;
class File;
class Stat;
using namespace std;

class StatsSyncher
{
private:
	static const int SAVE_INTERVAL = 20 * 5;
    static const int SEND_INTERVAL = 20 * 60;

    volatile bool busy;

    volatile unordered_map<Stat *, int> *serverStats;
    volatile unordered_map<Stat *, int> *failedSentStats;

    StatsCounter *statsCounter;
    File *unsentFile, *lastServerFile;
    File *unsentFileTmp, *lastServerFileTmp;
    File *unsentFileOld, *lastServerFileOld;
    User *user;

    int noSaveIn, noSendIn;

public:
	StatsSyncher(User *user, StatsCounter *statsCounter, File *dir);
private:
	void attemptRename(File *dir, const wstring& name, File *to);
    unordered_map<Stat *, int> *loadStatsFromDisk(File *file, File *tmp, File *old);
    unordered_map<Stat *, int> *loadStatsFromDisk(File *file);
    void doSend(unordered_map<Stat *, int> *stats);
    void doSave(unordered_map<Stat *, int> *stats, File *file, File *tmp, File *old);
protected:
	unordered_map<Stat *, int> *doGetStats();
public:
	void getStatsFromServer();
    void saveUnsent(unordered_map<Stat *, int> *stats);
    void sendUnsent(unordered_map<Stat *, int> *stats, unordered_map<Stat *, int> *fullStats);
    void forceSendUnsent(unordered_map<Stat *, int> *stats);
    void forceSaveUnsent(unordered_map<Stat *, int> *stats);
    bool maySave();
    bool maySend();
    void tick();
};
