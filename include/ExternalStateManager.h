/*
Max A. Allen
Void Bound
A Sci-Fi terminal game aiming to provide a more dynamic experience by
using relationships to determine a middle-ground event
*/

#ifndef EXTERNALSTATEMANAGER_H
#define EXTERNALSTATEMANAGER_H

#include <vector>
#include <string>

struct LeaderboardEntry {
	int score = -1;
	std::string playerName;
	std::string vehicleName;
};

struct SaveData {
	std::string playerName;
	std::string vesselName;
	int currentDay = 0;
	unsigned int gameSeed = 0;
	int resources[4] = { 0 };
	int relationships[4] = { 0 };
	std::string previousEvents[5];
};

class ExternalStateManager {
	void createDefaultSettings();
	void checkDefault(std::string& fileName, bool isEventFile);
	void createDefaultLeaderboard();
	bool checkEntry(const LeaderboardEntry& entry);
	void sortLeaderboardEntries(std::vector<LeaderboardEntry>& entries);

public:
	ExternalStateManager() {};

	void loadSettings(std::string& eventsFile, std::string& leaderboardFile);

	static bool checkFileExists(const std::string& fileName);

	std::vector<LeaderboardEntry> loadLeaderboard(const std::string& leaderboardFile);

	void writeSettignsFile(std::string& eventsFile, std::string& leaderboardFile);

	bool loadSaveFile(const std::string& name, SaveData& save);
	void setSaveDataByKey(SaveData& data, const std::string& key, const std::string& value);
	bool checkSaveGame(const SaveData& data);

	std::string sanitizeFileName(const std::string& fileName);
	void deleteFile(const std::string& fileName);

	void addLeaderboardEntry(const std::string& leaderboard, const std::string& playerName, const std::string& vesselName, const int& score);
	void saveGame(const SaveData& data);
};

#endif // !EXTERNALSTATEMANAGER_H
