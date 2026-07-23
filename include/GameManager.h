/*
Max A. Allen
Void Bound
A Sci-Fi terminal game aiming to provide a more dynamic experience by
using relationships to determine a middle-ground event
*/

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <string>
#include "EventManager.h"
#include "GameStatistics.h"
#include "PlayerTerminal.h"
#include "ExternalStateManager.h"

class GameManager {
	bool isRunning = false;
	bool poisonedSettings = false;

	PlayerTerminal userInterface;
	GameStatistics statisticsManager;
	EventManager eventController;
	ExternalStateManager externalManager;

	std::string leaderboardFile = "leaderboard.txt";
	std::string eventsFile = "newEvents.txt";


	std::vector<LeaderboardEntry> leaderboardData;

	void checkSettings();
	void menuRunner();
	std::string trimTrailing(std::string str);
	void gameLoop();
	bool processEventResponse(const char& response, Event& event);

public:

	GameManager();
	void loadGame();
	void newGame();
	void loadLeaderboard();
	void settingsChanger();


};

#endif // !GAMEMANAGER_H
