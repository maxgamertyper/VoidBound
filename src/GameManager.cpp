/*
Max A. Allen
Void Bound
A Sci-Fi terminal game aiming to provide a more dynamic experience by
using relationships to determine a middle-ground event
*/


#include <string>
#include <ctime>
#include <iostream>
#include <format>
#include "EventManager.h"
#include "GameStatistics.h"
#include "PlayerTerminal.h"
#include "GameManager.h"

GameManager::GameManager() {
	userInterface.bootSequence();
	std::cout << "Establishing Handshake with Space Command..." << std::endl;
	externalManager.loadSettings(eventsFile, leaderboardFile);
	userInterface.sleep(5);
	checkSettings();


	if (poisonedSettings) {
		return;
	}
	eventController = EventManager(eventsFile);
	eventController.saturatePools();
	menuRunner();
}

void GameManager::menuRunner() {
	char mainMenuDirective = 'a';
	userInterface.printMainMenu();
	std::vector<char> validOptions = { '1', '2', '3', '4', '5' };

	while (true) {
		int playerChoiceIndex = userInterface.awaitValidCharInput(
			validOptions, 
			"SEC-OS:// INPUT DIRECTIVE > ", 
			"[!] INVALID COMMAND. RE-INDEXING..."
		);

		switch (playerChoiceIndex) {
		case 0:
			newGame();
			userInterface.clearTerminal();
			userInterface.printMainMenu();
			break;
		case 1:
			loadGame();
			userInterface.clearTerminal();
			userInterface.printMainMenu();
			break;
		case 2:
			loadLeaderboard();
			userInterface.clearTerminal();
			userInterface.printMainMenu();
			break;
		case 3:
			settingsChanger();
			userInterface.clearTerminal();
			userInterface.printMainMenu();
			break;
		case 4:
			//End game
			return;
		default: // should never happen
			break;
		}
	}
}

void GameManager::loadGame() {
	std::string vesselName = userInterface.registryLookup();
	std::string saveFile = "save" + externalManager.sanitizeFileName(vesselName) + ".txt";

	SaveData thisSaveFile;
	bool successfulLoad = externalManager.loadSaveFile(saveFile, thisSaveFile);

	if (!successfulLoad || !externalManager.checkSaveGame(thisSaveFile)) {
		std::cout << "Critical: save file has invalid format or does not exist!";
		userInterface.sleep(3);
		return;
	}

	statisticsManager.setGameSeed(thisSaveFile.gameSeed);
	statisticsManager.processSeed();

	statisticsManager.setPlayerName(thisSaveFile.playerName);
	statisticsManager.setVesselName(thisSaveFile.vesselName);
	statisticsManager.setCurrentDay(thisSaveFile.currentDay);

	for (int i = 0; i < 4; i++) {
		statisticsManager.setResource(i, thisSaveFile.resources[i]);
		statisticsManager.setRelationship(i, thisSaveFile.relationships[i]);
	}

	bool continueSave = userInterface.welcomeBackSequence(
		statisticsManager.getPlayerName(),
		statisticsManager.getVesselName()
	);

	if (!continueSave) {
		std::cout << "Space Command: sending rescue response!";
		externalManager.deleteFile(saveFile);
		return;
	}

	userInterface.sleep(5);

	gameLoop();
}

void GameManager::gameLoop() {
	while (true) {
		Event currentEvent = eventController.getRandomEvent();
		userInterface.printEvent(currentEvent, statisticsManager.getPlayerName());

		std::vector<char> validOptions = { 'Q', 'R', 'Y', 'N', 'C' };
		int responseIndex = -1;


		while (responseIndex != 0) {
			responseIndex = userInterface.awaitValidCharInput(
				validOptions, 
				"Your Command, Captain: ", 
				"Captain, I'm not sure what that command means..."
			);
			bool valid = processEventResponse(
				validOptions.at(responseIndex),
				currentEvent
			);

			if (responseIndex == 1) { // not valid automatically
				userInterface.printResources(statisticsManager.getResources());
				userInterface.printEvent(currentEvent, statisticsManager.getPlayerName());
			}
		}

		SaveData currentGame = statisticsManager.toSaveGame();

		for (int i = 0; i < 5; i++) {
			currentGame.previousEvents[i] = eventController.getPreviousEventHeader(i);
		}

		externalManager.saveGame(currentGame);

		if (statisticsManager.checkDefeat()) {
			break;
		}

		if (responseIndex == 0) {
			return;
		}

		if (rand() % 2 == 1) {
			statisticsManager.progressDay();
			std::cout << "Survived the Day! resources used for survival: {-1 Oxygen, -1 Water, -1 Food}";
		}

		userInterface.sleep(2);
	}

	externalManager.addLeaderboardEntry(leaderboardFile,
		statisticsManager.getPlayerName(),
		statisticsManager.getVesselName(),
		statisticsManager.getCurrentDay()
	);

	int numZeroResources = 0;
	int resourceIndex = -1;
	if (statisticsManager.getResource(0) <= 0) {
		numZeroResources++;
		resourceIndex = 0;
	}
	if (statisticsManager.getResource(1) <= 0) {
		numZeroResources++;
		resourceIndex = 1;
	}
	if (statisticsManager.getResource(2) <= 0) {
		numZeroResources++;
		resourceIndex = 2;
	}
	if (statisticsManager.getResource(3) <= 0) {
		numZeroResources++;
		resourceIndex = 3;
	}

	userInterface.printDeath((numZeroResources == 1) ? resourceIndex : 4);
	std::cout << "Goodbye, Captian...";
	userInterface.sleep(3);
	userInterface.clearTerminal();
	userInterface.sleep(4);

	std::string file = externalManager.sanitizeFileName(
		statisticsManager.getVesselName()
	);
	externalManager.deleteFile("save" + file);
}

bool GameManager::processEventResponse(const char& response, Event& event) {
	double* changingResourceArray = nullptr;
	double* changingRelationshipArray = nullptr;
	std::string npcResponse;

	double counterResourceArray[4] = { 0 };
	double counterRelationshipArray[4] = { 0 };


	if (response == 'Y') {
		changingRelationshipArray = event.acceptRelationships;
		changingResourceArray = event.acceptResources;
		npcResponse = event.acceptResponse;
	}
	else if (response == 'N') {
		changingRelationshipArray = event.denyRelationships;
		changingResourceArray = event.denyResources;
		npcResponse = event.denyResponse;
	}
	else if (response == 'C') {
		int npcIndex;
		changingRelationshipArray = counterRelationshipArray;
		changingResourceArray = counterResourceArray;

		if (event.npc == "Vance {Combat Warden}") {
			npcIndex = 3;
		}
		else if (event.npc == "Alisa {Exploration Master}") {
			npcIndex = 2;
		}
		else if (event.npc == "Malik {Military Pilot}") {
			npcIndex = 1;
		}
		else if (event.npc == "Carrie {Aeronautics Engineer}") {
			npcIndex = 0;
		}
		else {
			npcIndex = 0;
			std::cout << "Alert: error with event, npc unknown. Defaulting to Engineer" << std::endl;
			userInterface.sleep(2);
		}

		int npcRelation = statisticsManager.getRelationship(npcIndex);
		int npcFactor = npcRelation / 50.0;

		if (npcFactor < .7) {
			npcResponse = event.counterNegativeResponse;
		}
		else if (npcFactor < 1.35) {
			npcResponse = event.counterNeutralResponse;
		}
		else {
			npcResponse = event.counterPositiveResponse;
		}

		for (int i = 0; i < 4; i++) {
			counterRelationshipArray[i] = event.counterBaseRelationships[i];
			counterRelationshipArray[i] += event.counterRelationshipModifier[i] * npcFactor;

			counterResourceArray[i] = event.counterBaseResources[i];
			counterResourceArray[i] += event.counterResourceModifier[i] * npcFactor;
		}
	}
	else {
		return false;
	}

	if (response == 'C') {
		std::cout << "Middle-Ground Results: {";
	}
	for (int i = 0; i < 4; i++) {
		double relationship = statisticsManager.getRelationship(i);
		double resource = statisticsManager.getResource(i);

		double relationshipTotal = relationship + changingRelationshipArray[i];
		double resourceTotal = resource + changingResourceArray[i];


		statisticsManager.setRelationship(i, relationshipTotal);
		statisticsManager.setResource(i, resourceTotal);

		if (response == 'C') {
			switch (i) {
			case 0:
				std::cout << userInterface.formatDouble(changingResourceArray[i]) << " Oxygen, ";
				break;
			case 1:
				std::cout << userInterface.formatDouble(changingResourceArray[i]) << " Water, ";
				break;
			case 2:
				std::cout << userInterface.formatDouble(changingResourceArray[i]) << " Food, ";
				break;
			case 3:
				std::cout << userInterface.formatDouble(changingResourceArray[i]) << " Scrap}";
				break;
			default:
				break;

			}

		}
	}
	userInterface.sleep(2);


	userInterface.printResponse(npcResponse, event.npc);
	userInterface.sleep(3);

	return true;
}

void GameManager::newGame() {
	statisticsManager.setGameSeed(time(0));
	statisticsManager.processSeed();

	std::string vesselName;
	std::string playerName;
	userInterface.newGameInputSequence(playerName, vesselName);

	vesselName = trimTrailing(vesselName);
	playerName = trimTrailing(playerName);


	statisticsManager.setVesselName(vesselName);
	statisticsManager.setPlayerName(playerName);
	statisticsManager.setDefaults();

	bool newGame = userInterface.startSequence(playerName, vesselName);

	if (!newGame) {
		return;
	}

	userInterface.sleep(5);

	gameLoop();
}

void GameManager::checkSettings() {
	poisonedSettings = !(ExternalStateManager::checkFileExists(eventsFile) 
		&& ExternalStateManager::checkFileExists(leaderboardFile));
}

void GameManager::loadLeaderboard() {
	leaderboardData = externalManager.loadLeaderboard(leaderboardFile);
	int pageCount = leaderboardData.size() / 10;
	pageCount = leaderboardData.size() % 10 == 0 ? pageCount : pageCount + 1;
	int currentPage = 1;

	userInterface.leaderboardSequence(leaderboardData, currentPage, pageCount);


	std::vector<char> validOptions = { 'Q', 'N', 'P' };
	int validIndex = userInterface.awaitValidCharInput(
		validOptions, 
		"SEC-OS://RECORDS/CMD > ", 
		"[!] INVALID COMMAND. RE-INDEXING..."
	);

	while (validIndex != 0) { // Q
		if (validIndex == 1) { // N
			currentPage++;
			if (currentPage > pageCount) {
				userInterface.sleepUpdateIncorrectInput(
					"SEC-OS://RECORDS/CMD > ", 
					"[!] MAXIMUM INDEX REACHED. CANNOT ADVANCE"
				);
				currentPage--;
			}
			else {
				userInterface.clearTerminal();
				userInterface.leaderboardSequence(
					leaderboardData, 
					currentPage, 
					pageCount
				);
			}
		}
		else if (validIndex == 2) { // P
			currentPage--;
			if (currentPage < 1) {
				userInterface.sleepUpdateIncorrectInput(
					"SEC-OS://RECORDS/CMD > ", 
					"[!] MINIMUM INDEX REACHED. CANNOT DE-INDEX"
				);
				currentPage++;
			}
			else {
				userInterface.clearTerminal();
				userInterface.leaderboardSequence(
					leaderboardData, 
					currentPage, 
					pageCount
				);
			}
		}
		validIndex = userInterface.awaitValidCharInput(
			validOptions, 
			"SEC-OS://RECORDS/CMD > ", 
			"[!] INVALID COMMAND. RE-INDEXING..."
		);
	}
	return;
}

void GameManager::settingsChanger() {
	userInterface.settingsSequence(eventsFile, leaderboardFile);
	externalManager.writeSettignsFile(eventsFile, leaderboardFile);
}

std::string GameManager::trimTrailing(std::string str) {
	const std::string whitespace = " \t\r ";
	size_t end = str.find_last_not_of(whitespace);

	if (end == std::string::npos) {
		return "";
	}

	return str.substr(0, end + 1);
}