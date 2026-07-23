/*
Max A. Allen
Void Bound
A Sci-Fi terminal game aiming to provide a more dynamic experience by
using relationships to determine a middle-ground event
*/

#include "EventManager.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

void EventManager::saturatePools() {
	std::ifstream eventFile;
	readingState readerState = readingState::Head;

	eventFile.open(fileName);

	if (!eventFile.is_open()) {
		std::cout << "Error: could not open event file" << std::endl;
		return;
	}

	std::string eventLine;
	std::getline(eventFile, eventLine); // remove the layout header

	Event currentEvent;

	while (std::getline(eventFile, eventLine)) {

		if (eventLine.find(": ") == std::string::npos) {
			//Headers
			if (eventLine == "[EVENT]") {
				currentEvent = Event();
				readerState = readingState::Head;
			}
			else if (eventLine == "[ACCEPT_EFFECTS]") {
				readerState = readingState::Accept;
			}
			else if (eventLine == "[DENY_EFFECTS]") {
				readerState = readingState::Deny;
			}
			else if (eventLine == "[COUNTER_OFFER]") {
				readerState = readingState::Counter;
			}
			else if (eventLine == "[END]") {
				bool validEvent = checkEventSanitizaiton(currentEvent);

				if (!validEvent) {
					std::cout << "Recieved END command with invalid event details! Skipping Event, Details:" << std::endl;
					printAllEventDetails(currentEvent);
					continue;
				}

				if (currentEvent.isUltimatum && currentEvent.relationshipUltimatum) {
					relationshipUltimatumPool.push_back(currentEvent);
				}
				else {
					masterEventPool.push_back(currentEvent);
				}
			}
			continue;
		}

		std::stringstream lineData(eventLine);

		std::string key;
		std::string value;
		std::string fill;

		std::getline(lineData, key, ':');
		std::getline(lineData, fill, ' ');
		std::getline(lineData, value);


		setEventByName(key, value, readerState, currentEvent);
	}
}

std::vector<double> EventManager::splitString(const std::string& string) {
	std::stringstream stream(string);
	std::vector<double> returnHolder;

	std::string temp;
	while (std::getline(stream, temp, ',')) {
		returnHolder.push_back(std::stod(temp));
	}

	return returnHolder;
}

// I know this function doesnt look nice but i dont know how to directly manipulate fields in c++ so it has to be hard-coded
void EventManager::setEventByName(const std::string& keyName, const std::string& value, const readingState& RS, Event& event) {
	switch (RS) {
	case readingState::Head:
		if (keyName == "isUltimatum") event.isUltimatum = (value == "true");
		else if (keyName == "relationshipUltimatum") event.relationshipUltimatum = (value == "true");
		else if (keyName == "npc") event.npc = value;
		else if (keyName == "header") event.header = value;
		else if (keyName == "prompt") event.prompt = value;
		else if (keyName == "dialogue") event.dialogue = value;
		break;




	case readingState::Accept:
		if (keyName == "response") event.acceptResponse = value;
		else {
			std::vector<double> splitData;
			try {
				splitData = splitString(value);
			}
			catch (const std::invalid_argument) {
				std::cout << "Invalid Event Argument for ACCEPT_" << keyName << 
					" (Parsing Error): " << value << std::endl;
				break;
			}
			if (splitData.size() != 4) {
				std::cout << "Invalid Event Argument for ACCEPT_" << keyName << 
					" (Invalid Length): " << value << std::endl;
				break;
			}

			if (keyName == "resources") {
				for (size_t i = 0; i < 4; ++i) {
					event.acceptResources[i] = splitData[i];
				}
			}
			else if (keyName == "relationships") {
				for (size_t i = 0; i < 4; ++i) {
					event.acceptRelationships[i] = splitData[i];
				}
			}
		}
		break;


	case readingState::Deny:
		if (keyName == "response") event.denyResponse = value;
		else {
			std::vector<double> splitData;
			try {
				splitData = splitString(value);
			}
			catch (const std::invalid_argument) {
				std::cout << "Invalid Event Argument for DENY_" << keyName << 
					" (Parsing Error): " << value << std::endl;
				break;
			}

			if (splitData.size() != 4) {
				std::cout << "Invalid Event Argument for DENY_" << keyName << 
					" (Invalid Length): " << value << std::endl;
				break;
			}

			if (keyName == "resources") {
				for (size_t i = 0; i < 4; ++i) {
					event.denyResources[i] = splitData[i];
				}
			}
			else if (keyName == "relationships") {
				for (size_t i = 0; i < 4; ++i) {
					event.denyRelationships[i] = splitData[i];
				}
			}
		}
		break;



	case readingState::Counter:
		if (keyName == "goodResponse") event.counterPositiveResponse = value;
		else if (keyName == "neutralResponse") event.counterNeutralResponse = value;
		else if (keyName == "badResponse") event.counterNegativeResponse = value;
		else {
			std::vector<double> splitData;
			try {
				splitData = splitString(value);
			}
			catch (const std::invalid_argument) {
				std::cout << "Invalid Event Argument for COUNTER_" << keyName << 
					" (Parsing Error): " << value << std::endl;
				break;
			}
			if (splitData.size() != 4) {
				std::cout << "Invalid Event Argument for COUNTER_" << keyName << 
					" (Invalid Length): " << value << std::endl;
				break;
			}


			if (keyName == "resources") {
				for (size_t i = 0; i < 4; ++i) {
					event.counterBaseResources[i] = splitData[i];
				}
			}
			else if (keyName == "relationships") {
				for (size_t i = 0; i < 4; ++i) {
					event.counterBaseRelationships[i] = splitData[i];
				}
			}
			else if (keyName == "resourceModifier") {
				for (size_t i = 0; i < 4; ++i) {
					event.counterResourceModifier[i] = splitData[i];
				}
			}
			else if (keyName == "relationshipModifier") {
				for (size_t i = 0; i < 4; ++i) {
					event.counterRelationshipModifier[i] = splitData[i];
				}
			}
		}
		break;


	default:
		break;
	}
}

bool EventManager::checkEventSanitizaiton(const Event& event) {
	bool headerFilled = !event.header.empty() &&
		!event.prompt.empty() &&
		!event.npc.empty() &&
		!event.dialogue.empty();

	bool responsesFilled = !event.acceptResponse.empty() &&
		!event.denyResponse.empty() &&
		!event.counterNegativeResponse.empty() &&
		!event.counterPositiveResponse.empty() &&
		!event.counterNeutralResponse.empty();

	return headerFilled && responsesFilled;
}


Event EventManager::getRandomEvent() {
	int chances = 5;
	Event randomEvent;

	while (chances > 0) {
		int randomIndex = rand() % masterEventPool.size();

		randomEvent = masterEventPool[randomIndex];

		bool isDuplicate = false;

		for (std::string previousHeader : previousPool) {
			if (randomEvent.header == previousHeader) {
				isDuplicate = true;
				break;
			}
		}

		if (isDuplicate) {
			chances--;
			continue;
		}

		break;
	}

	logEventHistory(randomEvent.header);

	return randomEvent;
}


void EventManager::logEventHistory(const std::string& eventHeader) {
	previousPool[previousPoolHead] = eventHeader;

	previousPoolHead = (previousPoolHead + 1) % 5;
}

void EventManager::printAllEventDetails(const Event& event) {
	std::cout << "Head:" << std::endl;
	std::cout << "Ultimatum: " << std::to_string(event.isUltimatum) << std::endl;
	std::cout << "Relationship Ultimatum: " << 
		std::to_string(event.relationshipUltimatum) << std::endl;
	std::cout << "Header: " << event.header << std::endl;
	std::cout << "npc: " << event.npc << std::endl;
	std::cout << "Dialogue: " << event.dialogue << std::endl;
	std::cout << "Prompt: " << event.prompt << std::endl;

	std::cout << std::endl << "Accept:" << std::endl;
	std::cout << "Resources: " << 
		listToString(event.acceptResources) << std::endl;
	std::cout << "Relationships: " << 
		listToString(event.acceptRelationships) << std::endl;
	std::cout << "Response: " << event.acceptResponse << std::endl;

	std::cout << std::endl << "Deny:" << std::endl;
	std::cout << "Resources: " << 
		listToString(event.denyResources) << std::endl;
	std::cout << "Relationships: " << 
		listToString(event.denyRelationships) << std::endl;
	std::cout << "Response: " << event.denyResponse << std::endl;

	std::cout << std::endl << "Counter:" << std::endl;
	std::cout << "Resources Base: " << 
		listToString(event.counterBaseResources) << std::endl;
	std::cout << "Resources Modifier: " << 
		listToString(event.counterResourceModifier) << std::endl;
	std::cout << "Relationships Base: " << 
		listToString(event.counterBaseRelationships) << std::endl;
	std::cout << "Relationships Modifier: " << 
		listToString(event.counterRelationshipModifier) << std::endl;
	std::cout << "Negative Response: " << event.counterNegativeResponse << std::endl;
	std::cout << "Neutral Response: " << event.counterNeutralResponse << std::endl;
	std::cout << "Positive Response: " << event.counterPositiveResponse << 
		std::endl << std::endl << std::endl;
}

std::string EventManager::listToString(const double list[4]) {
	return "[" +
		std::to_string(list[0]) + "," +
		std::to_string(list[1]) + "," +
		std::to_string(list[2]) + "," +
		std::to_string(list[3]) + "]";
}
