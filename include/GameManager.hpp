#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <string>
#include "EventManager.hpp"
#include "GameStatistics.hpp"
#include "PlayerTerminal.hpp"
#include "ExternalStateManager.hpp"

class GameManager {
	bool isRunning = false;
	bool poisonedSettings = false;

	PlayerTerminal UI;
	GameStatistics StatisticsManager;
	EventManager EventController;
	ExternalStateManager ExternalManager;

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
