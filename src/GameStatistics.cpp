#include "GameStatistics.hpp"
#include "ExternalStateManager.hpp"

int GameStatistics::checkReltaionshipUltimatum() {
	for (int i = 0; i < 4; ++i) {
		if (relationships[i] <= 0) return i;
	}
	return -1;
}

void GameStatistics::setDefaults() {
	setRelationship(0, 50);
	setRelationship(1, 50);
	setRelationship(2, 50);
	setRelationship(3, 50);

	setResource(0, 10);
	setResource(1, 10);
	setResource(2, 10);
	setResource(3, 25);

	currentDay = 0;
}

void GameStatistics::progressDay() {
	currentDay++;

	setResource(0, getResource(0) - 1);
	setResource(1, getResource(1) - 1);
	setResource(2, getResource(2) - 1);
}

SaveData GameStatistics::toSaveGame() {
	SaveData me;

	me.playerName = playerName;
	me.vesselName = vesselName;
	me.currentDay = currentDay;
	
	me.gameSeed = gameSeed;

	for (int i = 0; i < 4; i++) {
		me.resources[i] = resources[i];
		me.relationships[i] = relationships[i];
	}

	return me;
}