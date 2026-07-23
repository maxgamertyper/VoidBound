/*
Max A. Allen
Void Bound
A Sci-Fi terminal game aiming to provide a more dynamic experience by
using relationships to determine a middle-ground event
*/

#ifndef PLAYER_TERMINAL
#define PLAYER_TERMINAL

#include <string>
#include "EventManager.h"
#include "ExternalStateManager.h"
#include <chrono>
#include <thread>

class PlayerTerminal {
	const static int TERMINAL_SIZE = 54;
	const static int TERMINAL_EMPTY = 50;
	const static int NONTERMINAL_SIZE = 40;
	const static int RESOURCES_SIZE = 36;


	void printCenteredLine(const std::string& string, int width = 50, bool includeBars = false);

	void goUpXLines(int count);
	void goDownXLines(int count);
	void goRightXChars(int count);
	void goLeftXChars(int count);
	void jumpToStartOfLine();
	void clearRestOfLine();
	void printLeaderBoardEntry(const LeaderboardEntry& entry, int rank);
	void printBars(int width = TERMINAL_EMPTY);
	void printEqualLine(int width = NONTERMINAL_SIZE);


public:

	PlayerTerminal() {};

	void sleep(int seconds) { std::this_thread::sleep_for(std::chrono::seconds(seconds)); }

	void clearLines(int count);
	void clearTerminal();

	std::string formatDouble(const double& number);
	void printEvent(const Event& event, const std::string& playerName);

	int awaitValidCharInput(const std::vector<char>& validInputs, const std::string& prompt, const std::string& invalidResponse = "");
	std::string awaitStrInput(const std::string& prompt);
	void sleepUpdateIncorrectInput(const std::string& prompt, const std::string& incorrectResponse);

	void printDialogue(const std::string& type, const std::string& npc, const std::string& dialogue, const std::string& prompt = "");

	// scenes
	void printResponse(const std::string& response, const std::string& npc);
	void printDeath(int resourceIndex);
	void printResources(const double resources[4]);
	void bootSequence();
	void newGameInputSequence(std::string& outPlayerName, std::string& outVesselName);
	void printMainMenu();
	void leaderboardSequence(const std::vector<LeaderboardEntry>& board, int page, int count);
	void settingsSequence(std::string& eventsFile, std::string& leaderboardFile);
	bool welcomeBackSequence(const std::string& playerName, const std::string& vesselName);
	std::string registryLookup();
	bool startSequence(const std::string& playerName, const std::string& vesselName);

};

#endif // !PLAYER_TERMINAL