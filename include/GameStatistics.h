/*
Max A. Allen
Void Bound
A Sci-Fi terminal game aiming to provide a more dynamic experience by
using relationships to determine a middle-ground event
*/

#ifndef GAMESTATISTICS_H
#define GAMESTATISTICS_H

#include <cstdlib>
#include <string>
#include "ExternalStateManager.h"

class GameStatistics {

	const static int MAX_RELATIONSHIP_SCORE = 100;
	const static int MIN_RELATIONSHIP_SCORE = 100;
	const static int RESOURCE_COUNT = 4;
	const static int NPC_COUNT = 4;

	// oxygen, water, food, scrap
	double resources[RESOURCE_COUNT] = { 0 };
	//engineer, pilot, adventurer, Combat Warden
	double relationships[NPC_COUNT] = { 0 };

	int currentDay = -1;

	unsigned int gameSeed;

	std::string vesselName;
	std::string playerName;


public:

	GameStatistics() {};

	int getCurrentDay() const { return currentDay; }
	void setCurrentDay(int dayNumber) { currentDay = dayNumber; }

	bool checkDefeat() {
		return resources[0] <= 0 || resources[1] <= 0 || resources[2] <= 0 || resources[3] <= 0;
	}
	void progressDay();

	int checkReltaionshipUltimatum();

	void setResource(int index, int value) { resources[index] = value; }
	void setRelationship(int index, int value) { 
		if (value > MAX_RELATIONSHIP_SCORE) {
			value = MAX_RELATIONSHIP_SCORE;
		}
		else if (value < MIN_RELATIONSHIP_SCORE) {
			value = MIN_RELATIONSHIP_SCORE;
		}
		relationships[index] = value;
	}

	double getResource(int index) { return resources[index]; }
	double* getResources() { return resources; }

	double getRelationship(int index) { return relationships[index]; }
	double* getRelationships() { return relationships; }


	unsigned int getGameSeed() { return gameSeed; }
	void setGameSeed(unsigned int seed) { gameSeed = seed; }
	void processSeed() { srand(gameSeed); }

	std::string getVesselName() { return vesselName; }
	std::string getPlayerName() { return playerName; }

	void setVesselName(const std::string& name) { vesselName = name; }
	void setPlayerName(const std::string& name) { playerName = name; }

	void setDefaults();
	SaveData toSaveGame();
};

#endif