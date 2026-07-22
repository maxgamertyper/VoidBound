
#include <string>
#include <ctime>
#include <iostream>
#include <format>
#include "EventManager.hpp"
#include "GameStatistics.hpp"
#include "PlayerTerminal.hpp"
#include "GameManager.hpp"

GameManager::GameManager() { 
	UI.BootSequence();
	std::cout << "Establishing Handshake with Space Command..." << std::endl;
	ExternalManager.loadSettings(eventsFile, leaderboardFile);
	UI.sleep(5);
	checkSettings();


	if (poisonedSettings) {
		return;
	}
	EventController = EventManager(eventsFile);
	EventController.saturatePools();
	MenuRunner();
}

void GameManager::MenuRunner() {
	char mainMenuDirective = 'a';
	UI.PrintMainMenu();

	while (true) {
		char playerChoice = UI.AwaitCharInputSeperate("SEC-OS:// INPUT DIRECTIVE > ", "[!] INVALID COMMAND. RE-INDEXING...");
		bool validInput = false;

		switch (playerChoice) {
		case '1':
			NewGame();
			UI.ClearTerminal();
			UI.PrintMainMenu();
			break;
		case '2':
			LoadGame();
			UI.ClearTerminal();
			UI.PrintMainMenu();
			break;
		case '3':
			LoadLeaderboard();
			UI.ClearTerminal();
			UI.PrintMainMenu();
			break;
		case '4':
			SettingsChanger();
			UI.ClearTerminal();
			UI.PrintMainMenu();
			break;
		case '5':
			//End game
			return;
		default:
			UI.SleepUpdateIncorrectInput("SEC-OS:// INPUT DIRECTIVE > ", "[!] INVALID COMMAND. RE-INDEXING...");
			break;
		}
	}
}

void GameManager::LoadGame() {
	std::string vesselName = UI.registryLookup();
	std::string saveFile = "save" + ExternalManager.sanitizeFileName(vesselName);

	SaveData thisSaveFile;
	bool successfulLoad = ExternalManager.loadSaveFile(saveFile, thisSaveFile);

	if (!successfulLoad || !ExternalManager.checkSaveGame(thisSaveFile)) {
		std::cout << "Critical: save file has invalid format or does not exist!";
		UI.sleep(3);
		return;
	}

	StatisticsManager.setGameSeed(thisSaveFile.gameSeed);
	StatisticsManager.processSeed();

	StatisticsManager.setPlayerName(thisSaveFile.playerName);
	StatisticsManager.setVesselName(thisSaveFile.vesselName);
	StatisticsManager.setCurrentDay(thisSaveFile.currentDay);

	for (int i = 0; i < 4; i++) {
		StatisticsManager.setResource(i,thisSaveFile.resources[i]);
		StatisticsManager.setRelationship(i, thisSaveFile.relationships[i]);
	}

	bool continueSave = UI.welcomeBackSequence(
		StatisticsManager.getPlayerName(), 
		StatisticsManager.getVesselName()
	);

	if (!continueSave) {
		std::cout << "Space Command: sending rescue response!";
		ExternalManager.deleteFile(saveFile);
		return;
	}

	UI.sleep(5);

	gameLoop();
}

void GameManager::gameLoop() {
	while (true) {
		Event currentEvent = EventController.getRandomEvent();
		UI.PrintEvent(currentEvent, StatisticsManager.getPlayerName());
		char response = 'a';


		while (response!='Q') {
			response = UI.AwaitCharInputInline("Your Command, Captain ");
			bool valid = processEventResponse(response, currentEvent);

			if (response == 'R') { // not valid automatically
				UI.printResources(StatisticsManager.getResources());
			}

			if (!valid) {
				UI.PrintEvent(currentEvent, StatisticsManager.getPlayerName());
				continue;
			}
			else {
				break;
			}
		}

		SaveData currentGame = StatisticsManager.toSaveGame();
		
		for (int i = 0; i < 5; i++) {
			currentGame.previousEvents[i] = EventController.getPreviousEventHeader(i);
		}
		
		ExternalManager.saveGame(currentGame);

		if (StatisticsManager.checkDefeat()) {
			break;
		}
		
		if (response == 'Q') {
			return;
		}

		if (rand() % 2 == 1) {
			StatisticsManager.progressDay();
			std::cout << "Survived the Day! resources used for survival: {-1 Oxygen, -1 Water, -1 Food}";
		}
		
		UI.sleep(2);
	}

	ExternalManager.addLeaderboardEntry(leaderboardFile,
		StatisticsManager.getPlayerName(),
		StatisticsManager.getVesselName(),
		StatisticsManager.getCurrentDay()
	);

	int numZeroResources = 0;
	int resourceIndex = -1;
	if (StatisticsManager.getResource(0)<=0) {
		numZeroResources++;
		resourceIndex = 0;
	} 
	if (StatisticsManager.getResource(1) <= 0) {
		numZeroResources++;
		resourceIndex = 1;
	}
	if (StatisticsManager.getResource(2) <= 0) {
		numZeroResources++;
		resourceIndex = 2;
	}
	if (StatisticsManager.getResource(3) <= 0) {
		numZeroResources++;
		resourceIndex = 3;
	}

	UI.printDeath((numZeroResources == 1) ? resourceIndex : 4);
	std::cout << "Goodbye, Captian...";
	UI.sleep(3);
	UI.ClearTerminal();
	UI.sleep(4);

	std::string file = ExternalManager.sanitizeFileName(StatisticsManager.getVesselName());
	ExternalManager.deleteFile("save"+file);
}

bool GameManager::processEventResponse(const char& response, Event& event) {
	double* changingResourceArray = nullptr;
	double* changingRelationshipArray = nullptr;
	std::string NPCResponse;

	double counterResourceArray[4] = {0};
	double counterRelationshipArray[4] = {0};


	if (response == 'Y') {
		changingRelationshipArray = event.acceptRelationships;
		changingResourceArray = event.acceptResources;
		NPCResponse = event.acceptResponse;
	}
	else if (response == 'N') {
		changingRelationshipArray = event.denyRelationships;
		changingResourceArray = event.denyResources;
		NPCResponse = event.denyResponse;
	}
	else if (response == 'C') {
		int NPCindex;
		changingRelationshipArray = counterRelationshipArray;
		changingResourceArray = counterResourceArray;

		if (event.NPC == "Vance {Combat Warden}") {
			NPCindex = 3;
		}
		else if (event.NPC == "Alisa {Exploration Master}") {
			NPCindex = 2;
		}
		else if (event.NPC == "Malik {Military Pilot}") {
			NPCindex = 1;
		}
		else if (event.NPC == "Carrie {Aeronautics Engineer}") {
			NPCindex = 0;
		}
		else {
			NPCindex = 0;
			std::cout << "Alert: error with event, NPC unknown. Defaulting to Engineer" << std::endl;
			UI.sleep(2);
		}

		int npcRelation = StatisticsManager.getRelationship(NPCindex);
		int npcFactor = npcRelation / 50.0;

		if (npcFactor < .7) {
			NPCResponse = event.counterNegativeResponse;
		}
		else if (npcFactor < 1.35) {
			NPCResponse = event.counterNeutralResponse;
		}
		else {
			NPCResponse = event.counterPositiveResponse;
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
		double relationship = StatisticsManager.getRelationship(i);
		double resource = StatisticsManager.getResource(i);

		double relationshipTotal = relationship + changingRelationshipArray[i];
		double resourceTotal = resource + changingResourceArray[i];


		StatisticsManager.setRelationship(i, relationshipTotal);
		StatisticsManager.setResource(i, resourceTotal);

		if (response == 'C') {
			switch (i) {
				case 0:
					std::cout << std::format("{:.2f}", changingResourceArray[i]) << " Oxygen, ";
					break;
				case 1:
					std::cout << std::format("{:.2f}", changingResourceArray[i]) << " Water, ";
					break;
				case 2:
					std::cout << std::format("{:.2f}", changingResourceArray[i]) << " Food, ";
					break;
				case 3:
					std::cout << std::format("{:.2f}", changingResourceArray[i]) << " Scrap}";
					break;
				default:
					break;

			}
				
		}
	}
	UI.sleep(2);
	

	UI.printResponse(NPCResponse, event.NPC);
	UI.sleep(3);

	return true;
}

void GameManager::NewGame() {
	StatisticsManager.setGameSeed(time(0));
	StatisticsManager.processSeed();

	std::string vesselName;
	std::string playerName;
	UI.NewGameInputSequence(playerName, vesselName);

	vesselName = trimTrailing(vesselName);
	playerName = trimTrailing(playerName);

	
	StatisticsManager.setVesselName(vesselName);
	StatisticsManager.setPlayerName(playerName);
	StatisticsManager.setDefaults();

	bool newGame = UI.startSequence(playerName, vesselName);

	if (!newGame) {
		return;
	}
	
	UI.sleep(5);

	gameLoop();
}

void GameManager::checkSettings() {
	poisonedSettings = !(ExternalStateManager::checkFileExists(eventsFile) && ExternalStateManager::checkFileExists(leaderboardFile));
}

void GameManager::LoadLeaderboard() {
	leaderboardData = ExternalManager.loadLeaderboard(leaderboardFile);
	int pageCount = leaderboardData.size() / 10;
	pageCount = leaderboardData.size() % 10 == 0 ? pageCount : pageCount + 1;
	int currentPage = 1;
	
	UI.LeaderboardSequence(leaderboardData, currentPage, pageCount);
	

	char input = UI.AwaitCharInputSeperate("SEC-OS://RECORDS/CMD > ", "[!] INVALID COMMAND. RE-INDEXING...");
	while (input != 'Q') {
		if (input == 'N') {
			currentPage++;
			if (currentPage > pageCount) {
				UI.SleepUpdateIncorrectInput("SEC-OS://RECORDS/CMD > ", "[!] MAXIMUM INDEX REACHED. CANNOT ADVANCE");
				currentPage--;
			}
			else {
				UI.ClearTerminal();
				UI.LeaderboardSequence(leaderboardData, currentPage, pageCount);
			}
		}
		else if (input == 'P') {
			currentPage--;
			if (currentPage < 1) {
				UI.SleepUpdateIncorrectInput("SEC-OS://RECORDS/CMD > ", "[!] MINIMUM INDEX REACHED. CANNOT DE-INDEX");
				currentPage++;
			}
			else {
				UI.ClearTerminal();
				UI.LeaderboardSequence(leaderboardData, currentPage, pageCount);
			}
		}
		input = UI.AwaitCharInputSeperate("SEC-OS://RECORDS/CMD > ", "[!] INVALID COMMAND. RE-INDEXING...");
	}
	
	return;
}

void GameManager::SettingsChanger() {
	UI.SettingsSequence(eventsFile, leaderboardFile);
	ExternalManager.writeSettignsFile(eventsFile, leaderboardFile);
}

std::string GameManager::trimTrailing(std::string str) {
	const std::string whitespace = " \t\r ";
	size_t end = str.find_last_not_of(whitespace);

	if (end == std::string::npos) {
		return "";
	}

	return str.substr(0, end + 1);
}