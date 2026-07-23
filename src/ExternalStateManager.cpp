/*
Max A. Allen
Void Bound
A Sci-Fi terminal game aiming to provide a more dynamic experience by
using relationships to determine a middle-ground event
*/

#include "ExternalStateManager.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <filesystem>



void ExternalStateManager::checkDefault(std::string& fileName, bool isEventFile) {
	if (!checkFileExists(fileName) && isEventFile) {
		std::cout << "> CRITICAL: could not find default events file or any backup, please download it!" << std::endl;
	}
	else if (!checkFileExists(fileName) && isEventFile) {
		std::cout << "> Alert: could not find default leaderboard file or any backup, creating empty file!" << std::endl;
		createDefaultLeaderboard();
	}
}

void ExternalStateManager::createDefaultLeaderboard() {
	std::ofstream settingsCreator("leaderboard.txt");

	if (!settingsCreator.is_open()) {
		std::cout << "> CRITICAL: could not create default 'leaderboard.txt'" << std::endl;
		return;
	}
}

void ExternalStateManager::createDefaultSettings() {
	std::ofstream settingsCreator("settings.txt");

	if (!settingsCreator.is_open()) {
		std::cout << "> Error: could not create default 'settings.txt'" << std::endl;
		return;
	}

	settingsCreator << "newEvents.txt" << std::endl;
	settingsCreator << "leaderboard.txt" << std::endl;
}

void ExternalStateManager::loadSettings(std::string& eventFile, std::string& leaderboardFile) {
	std::ifstream settingsLoader;

	// defaults
	eventFile = "newEvents.txt";
	leaderboardFile = "leaderboard.txt";


	if (!checkFileExists("settings.txt")) {
		std::cout << "> Warning: could not find 'settings.txt', creating default" << std::endl;
		createDefaultSettings();
		return;
	}

	settingsLoader.open("settings.txt");

	if (!settingsLoader.is_open()) {
		std::cout << "> Error: could not open 'settings.txt'" << std::endl;
		return;
	}

	std::getline(settingsLoader >> std::ws, eventFile);
	std::getline(settingsLoader >> std::ws, leaderboardFile);

	if (!checkFileExists(eventFile)) {
		std::cout << "> Error: Could not find EventFile: '" << 
			eventFile << "', falling back" << std::endl;
		
		eventFile = "newEvents.txt";
		checkDefault(eventFile, true);
	}
	if (!checkFileExists(leaderboardFile)) {
		std::cout << "> Error: Could not find leaderboardFile: '" << 
			leaderboardFile << "', falling back" << std::endl;
		
		leaderboardFile = "leaderboard.txt";
		checkDefault(leaderboardFile, false);
	}
}

bool ExternalStateManager::checkFileExists(const std::string& fileName) {
	return std::filesystem::exists(fileName);
}

bool ExternalStateManager::checkEntry(const LeaderboardEntry& entry) {
	return entry.score != -1 && 
		!entry.playerName.empty() && 
		!entry.vehicleName.empty();
}

std::vector<LeaderboardEntry> ExternalStateManager::loadLeaderboard(const std::string& leaderboardFileName) {
	std::vector<LeaderboardEntry> returnHolder;
	std::ifstream leaderboardFile(leaderboardFileName);

	if (!leaderboardFile.is_open()) {
		std::cout << "> Error: Could not open leaderboard data link." << std::endl;
		return returnHolder;
	}

	std::string unformattedEntry;
	std::getline(leaderboardFile >> std::ws, unformattedEntry); // eat header

	while (std::getline(leaderboardFile >> std::ws, unformattedEntry)) {
		if (unformattedEntry.empty()) continue;


		std::stringstream streamedEntry(unformattedEntry);

		LeaderboardEntry thisEntry;

		std::string holderString;
		std::getline(streamedEntry >> std::ws, holderString, '|'); // the score 

		thisEntry.score = std::stoi(holderString);

		std::getline(streamedEntry >> std::ws, holderString, '|'); // the Player Name 
		thisEntry.playerName = holderString;

		std::getline(streamedEntry >> std::ws, holderString); // the Vehicle Name
		thisEntry.vehicleName = holderString;

		if (checkEntry(thisEntry)) {
			returnHolder.push_back(thisEntry);
		}
		else {
			std::cout << "> Error: malformed leaderboard input: '" << 
				unformattedEntry << "', skipping" << std::endl;
		}
	}

	sortLeaderboardEntries(returnHolder);
	return returnHolder;
}

void ExternalStateManager::sortLeaderboardEntries(std::vector<LeaderboardEntry>& entries) {
	std::sort(
		entries.begin(), 
		entries.end(), 
		// lambda (known from python and java, just had to lookup how)
		[](const LeaderboardEntry& a, const LeaderboardEntry& b) { 
		return a.score > b.score;
		}
	);
}

void ExternalStateManager::writeSettignsFile(std::string& eventsFile, std::string& leaderboardFile) {
	std::ofstream settingsFile("settings.txt");

	if (!settingsFile.is_open()) {
		std::cout << "> Alert: could not open settings.txt, restoring previous files" << std::endl;
		loadSettings(eventsFile, leaderboardFile);
		return;
	}

	settingsFile << eventsFile << std::endl << leaderboardFile << std::endl;
}

bool ExternalStateManager::loadSaveFile(const std::string& fileName, SaveData& save) {
	if (!checkFileExists(fileName)) {
		std::cout << "Critical: save file does not exist: " << fileName;
		return false;
	}

	std::ifstream saveFileData(fileName);

	if (!saveFileData.is_open()) {
		std::cout << "Alert: failed to open save file: " << fileName;
		return false;
	}

	std::string firstLine;
	std::getline(saveFileData >> std::ws, firstLine);

	if (firstLine != "[GAME_SAVE_FILE]") {
		std::cout << "Alert: file \"" << fileName << "\" is not a save file!";
		saveFileData.close();
		return false;
	}

	std::stringstream lineData;

	while (std::getline(saveFileData >> std::ws, firstLine)) {

		lineData.clear();
		lineData.str(firstLine);

		if (firstLine == "[END]") {
			break;
		}
		if (firstLine.starts_with('[')) { // not needed here
			continue;
		}

		std::string inputKey;
		std::string inputValue;

		std::getline(lineData >> std::ws, inputKey, ':');
		std::getline(lineData >> std::ws, inputValue);

		std::cout << inputKey << std::endl << inputValue << std::endl;

		setSaveDataByKey(save, inputKey, inputValue);
	}

	return true;
}

void ExternalStateManager::setSaveDataByKey(SaveData& data, const std::string& key, const std::string& value) {
	if (key.empty() || value.empty()) {
		return;
	}

	if (key == "playerName") {
		data.playerName = value;
	}
	else if (key == "vesselName") {
		data.vesselName = value;
	}
	else if (key == "currentDay") {
		data.currentDay = std::stoi(value);
	}
	else if (key == "gameSeed") {
		data.gameSeed = std::stoi(value);
	}
	else {
		std::stringstream valueStream(value);
		char delimiter = ',';
		int* targetArray = nullptr;

		if (key == "playedEvents") {
			delimiter = '|';
			std::string temporary;

			for (int i = 0; i < 3; i++) {
				std::getline(valueStream >> std::ws, temporary, delimiter);
				data.previousEvents[i] = temporary;
			}

			std::getline(valueStream >> std::ws, temporary);
			data.previousEvents[3] = temporary;

			return; // dont run the next part
		}
		else if (key == "resources") {
			targetArray = data.resources;
		}
		else if (key == "relationships") {
			targetArray = data.relationships;
		}
		else {
			std::cout << "Unknown key: " << key << " with value: " << value;
			return;
		}

		std::string temporary;


		for (int i = 0; i < 3; i++) {
			std::getline(valueStream >> std::ws, temporary, delimiter);
			targetArray[i] = std::stoi(temporary);
		}

		std::getline(valueStream >> std::ws, temporary);
		targetArray[3] = std::stoi(temporary);

	}
}

bool ExternalStateManager::checkSaveGame(const SaveData& data) {
	return !data.playerName.empty() && !data.vesselName.empty();
}

std::string ExternalStateManager::sanitizeFileName(const std::string& fileName) {
	std::string sanitizedName;
	std::string forbidden = "\\/:*?\"<>| ";

	for (char c : fileName) {

		if (forbidden.find(c) != std::string::npos) {
			sanitizedName += '_';
		}
		else {
			sanitizedName += c;
		}

	}

	return sanitizedName;
}

void ExternalStateManager::deleteFile(const std::string& fileName) {
	try {
		if (std::filesystem::remove(fileName)) {
			std::cout << "Response team successfully saved ship, returning to command" << std::endl;
		}
		else {
			std::cout << "Alert: lost contact with response team, abandoning rescure mission" << std::endl;
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error deleting file: " << e.what() << std::endl;
	}
}

void ExternalStateManager::addLeaderboardEntry(const std::string& leaderboard, const std::string& playerName, const std::string& vesselName, const int& score) {
	std::ofstream leaderboardFile(leaderboard, std::ios::app);

	if (!leaderboardFile.is_open()) {
		std::cout << "Error: Could not open leaderboard " << 
			leaderboard << " for appending!" << std::endl;
	}

	leaderboardFile <<
		std::to_string(score) << '|'
		<< playerName << '|'
		<< vesselName
		<< std::endl;

	leaderboardFile.close();
}

void ExternalStateManager::saveGame(const SaveData& data) {
	std::string saveFileName = "save" + sanitizeFileName(data.vesselName) + ".txt";

	std::ofstream saveFile(saveFileName);

	if (!saveFile.is_open()) {
		std::cout << "Error: Could not open saveFile " << 
			saveFileName << " for saving!" << std::endl;
		return;
	}

	std::string resourceString;
	std::string relationshipString;
	std::string playedEvents;

	resourceString = std::to_string(data.resources[0]);
	relationshipString = std::to_string(data.relationships[0]);
	playedEvents = data.previousEvents[0];

	for (int i = 1; i < 4; ++i) {
		resourceString += "," + std::to_string(data.resources[i]);
		relationshipString += "," + std::to_string(data.relationships[i]);
		playedEvents += "," + data.previousEvents[i];
	}

	playedEvents += "," + data.previousEvents[4];


	saveFile <<
		"[GAME_SAVE_FILE]\n[PLAYER_PROFILE]\n"
		<< "playerName: " << data.playerName
		<< "\nvesselName: " << data.vesselName
		<< "\ncurrentDay: " << data.currentDay
		<< "\ngameSeed: " << data.gameSeed
		<< "\n\n[COLONY_STATS]"
		<< "\nresources: " << resourceString
		<< "\nrelationships: " << relationshipString
		<< "\n\n[EVENT_HISTORY]"
		<< "\nplayedEvents: " << playedEvents
		<< "\n[END]";

	saveFile.close();
}