#ifndef GAMESTATISTICS_H
#define GAMESTATISTICS_H

#include <cstdlib>
#include <string>
#include "ExternalStateManager.hpp"

class GameStatistics {

	// oxygen, water, food, scrap
	double resources[4] = { 0 };
	//engineer, pilot, adventurer, Combat Warden
	double relationships[4] = { 0 };

	int currentDay = -1;

	unsigned int gameSeed;

	std::string vesselName;
	std::string playerName;


public: 

	GameStatistics() {};

	int getCurrentDay() const { return currentDay; }
	void setCurrentDay(const int& dayNumber) { currentDay = dayNumber; }

	bool checkDefeat() {
		return resources[0] <= 0 || resources[1] <= 0 || resources[2] <= 0 || resources[3] <= 0;
	}
	void progressDay();

	int checkReltaionshipUltimatum();

	void setResource(const int& index, const int& value) { resources[index] = value; }
	void setRelationship(const int& index, const int& value) { relationships[index] = value; }

	double getResource(const int& index) { return resources[index]; }
	double* getResources() { return resources; }

	double getRelationship(const int& index) { return relationships[index]; }
	double* getRelationships() { return relationships; }


	unsigned int getGameSeed() { return gameSeed; }
	void setGameSeed(const unsigned int& seed) { gameSeed = seed; }
	void processSeed() { srand(gameSeed); }

	std::string getVesselName() { return vesselName; }
	std::string getPlayerName() { return playerName; }

	void setVesselName(const std::string& name) { vesselName = name; }
	void setPlayerName(const std::string& name) { playerName = name; }

	void setDefaults();
	SaveData toSaveGame();
};

#endif