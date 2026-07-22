#ifndef PLAYER_TERMINAL
#define PLAYER_TERMINAL

#include <string>
#include "EventManager.hpp"
#include "ExternalStateManager.hpp"
#include <chrono>
#include <thread>

class PlayerTerminal {
	void printCenteredLine(const std::string& string, const int& width = 50, bool includeBars = false);

	void goUpXLines(const int& count);
	void goDownXLines(const int& count);
	void goRightXChars(const int& count);
	void goLeftXChars(const int& count);
	void jumpToStartOfLine();
	void clearRestOfLine();
	void printLeaderBoardEntry(const LeaderboardEntry& entry, const int& rank);
	void printBars(int width = 50);
	void printEqualLine(int width = 40);

	std::string formatDouble(const double& number);

public:
	
	PlayerTerminal() {};

	void sleep(const int& seconds) { std::this_thread::sleep_for(std::chrono::seconds(seconds)); }

	void clearLines(int count);
	void clearTerminal();
	
	char awaitCharInputInline(const std::string& prompt);
	char awaitCharInputSeperate(const std::string& prompt, const std::string& invalidResponse);
	void printEvent(const Event& event, const std::string& playerName);
	std::string awaitStrInput(const std::string& prompt);
	void sleepUpdateIncorrectInput(const std::string& prompt, const std::string& incorrectResponse);
	
	void printDialogue(const std::string& type, const std::string& NPC, const std::string& dialogue, const std::string& prompt = "");

	// scenes
	void printResponse(const std::string& response, const std::string& NPC);
	void printDeath(const int& resourceIndex);
	void printResources(const double resources[4]);
	void bootSequence();
	void newGameInputSequence(std::string& outPlayerName, std::string& outVesselName);
	void printMainMenu();
	void leaderboardSequence(const std::vector<LeaderboardEntry>& leaderboard, const int& pageNumber, const int& pageCount);
	void settingsSequence(std::string& eventsFile, std::string& leaderboardFile);
	bool welcomeBackSequence(const std::string& playerName, const std::string& vesselName);
	std::string registryLookup();
	bool startSequence(const std::string& playerName, const std::string& vesselName);

};

#endif // !PLAYER_TERMINAL