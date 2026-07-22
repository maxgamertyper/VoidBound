#include "EventManager.hpp"
#include "PlayerTerminal.hpp"
#include "ExternalStateManager.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <format>

void PlayerTerminal::clearLines(int count) {
	for (; count > 0; count--) {
		std::cout << "\033[A\r\033[K" << std::flush;
	}
	std::cout << std::flush;
}
void PlayerTerminal::clearTerminal() {
	std::cout << "\033[2J\033[H";
	std::cout << std::flush;
}
void PlayerTerminal::goUpXLines(const int& count) {
	std::cout << "\033[" << std::to_string(count) << "A" << std::flush;
	std::cout << std::flush;
}
void PlayerTerminal::goDownXLines(const int& count) {
	std::cout << "\033[" << std::to_string(count) << "B" << std::flush;
	std::cout << std::flush;
}
void PlayerTerminal::goRightXChars(const int& count) {
	std::cout << "\033[" << std::to_string(count) << "C" << std::flush;
	std::cout << std::flush;
}
void PlayerTerminal::goLeftXChars(const int& count) {
	std::cout << "\033[" << std::to_string(count) << "D" << std::flush;
	std::cout << std::flush;
}
void PlayerTerminal::jumpToStartOfLine() {
	std::cout << "\r";
	std::cout << std::flush;
}
void PlayerTerminal::clearRestOfLine() {
	std::cout << "\033[K";
	std::cout << std::flush;
}
void PlayerTerminal::printBars(int width) {
	std::cout << "||" << std::setw(width+2) << std::setfill(' ') << std::right << "||" << std::endl;
}
void PlayerTerminal::printEqualLine(int width) {
	std::cout << std::setw(width) << std::setfill('=') << "" << std::endl;
}
std::string PlayerTerminal::formatDouble(const double& number) {
	return std::format("{:.2f}", static_cast<double>(number));
}

char PlayerTerminal::awaitCharInputInline(const std::string& prompt) {
	std::string thisInput;
	std::getline(std::cin, thisInput);

	while (thisInput.size()!=1) {
		clearLines(1);
		std::cout << prompt << "(Invalid Input : " << thisInput << "): ";

		std::getline(std::cin, thisInput);
	}

	return std::toupper(thisInput.at(0));
}
char PlayerTerminal::awaitCharInputSeperate(const std::string& prompt, const std::string& invalidResponse) {
	std::string thisInput;
	std::getline(std::cin, thisInput);

	while (thisInput.size() != 1) {
		clearLines(1);
		std::cout << invalidResponse;
		sleep(2);
		jumpToStartOfLine();
		clearRestOfLine();
		std::cout << prompt;


		std::getline(std::cin, thisInput);
	}

	return std::toupper(thisInput.at(0));
}
std::string PlayerTerminal::awaitStrInput(const std::string& prompt) {
	std::string out;
	
	std::cout << prompt;
	std::getline(std::cin, out);

	return out;
}



void PlayerTerminal::printCenteredLine(const std::string& text, const int& width, bool includeBars) {
	int padding = width - text.length();
	int leftPad = padding / 2;
	int rightPad = padding - leftPad;

	if (includeBars) {
		std::cout << "||";
	}
	std::cout << std::string(leftPad, ' ') << text << std::string(rightPad, ' ');
	if (includeBars) {
		std::cout << "||" << std::endl;
	}
}
void PlayerTerminal::sleepUpdateIncorrectInput(const std::string& prompt, const std::string& failMessage) {
	goUpXLines(1);
	jumpToStartOfLine();
	clearRestOfLine();
	std::cout << failMessage;
	sleep(2);
	jumpToStartOfLine();
	clearRestOfLine();
	std::cout << prompt;
}

//Sub-Scenes
void PlayerTerminal::printDialogue(const std::string& type, const std::string& NPC, const std::string& response, const std::string& prompt) {
	clearTerminal();

	printEqualLine();
	std::cout << "FROM: " << NPC << std::endl;
	std::cout << "TYPE: " << type << std::endl;
	std::cout << std::setw(40) << std::setfill('-') << "" << std::endl;
	std::cout << '\"' << response << '\"' << std::endl;
	printEqualLine();

	if (!prompt.empty()) {
		std::cout << prompt << std::endl << std::endl;

	}
}
void PlayerTerminal::printEvent(const Event& event, const std::string& playerName) {
	printDialogue(
		event.header,
		event.NPC,
		event.dialogue,
		event.prompt + " (Y/N/C/R)"
	);

	std::cout << "Y to accept the request of this transmisson {"
		<< formatDouble(event.acceptResources[0]) << " Oxygen, "
		<< formatDouble(event.acceptResources[1]) << " Water, "
		<< formatDouble(event.acceptResources[2]) << " Food, "
		<< formatDouble(event.acceptResources[3]) << " Scrap}";

	std::cout << "\nN to deny the request {" 
		<< formatDouble(event.denyResources[0]) << " Oxygen, "
		<< formatDouble(event.denyResources[1]) << " Water, "
		<< formatDouble(event.denyResources[2]) << " Food, "
		<< formatDouble(event.denyResources[3]) << " Scrap}";
		
	std::cout << "\nC to attempt to convince this NPC for a middle ground\nR to view resources\nQ to quit" << std::endl;
	std::cout << "Your Command, Captain " << playerName << ": ";
}
void PlayerTerminal::printResponse(const std::string& dialogue, const std::string& NPC) {
	printDialogue(
		"RESPONSE",
		NPC,
		dialogue
	);
}
void PlayerTerminal::printResources(const double resources[4]) {
	clearTerminal();
	printEqualLine();
	printBars(36);
	printCenteredLine("CALCULATING RESOURCES...",36,true);
	printBars(36);
	printEqualLine();

	// pretend to do smth
	sleep(2);
	goUpXLines(5);
	clearLines(5);

	// clear the data and show the resources
	printEqualLine();
	printBars(36);

	printCenteredLine("OXYGEN :" + std::to_string(resources[0]), 36, true);
	printCenteredLine("WATER  :" + std::to_string(resources[1]), 36, true);
	printCenteredLine("FOOD  :" + std::to_string(resources[2]), 36, true);
	printCenteredLine("SCRAP :" + std::to_string(resources[3]), 36, true);

	printBars(36);
	printEqualLine();

	awaitStrInput("SEC-OS://RESOURCES (TYPE ANYTHING) > "); // just wait until they type smth
}
void PlayerTerminal::printDeath(const int& resourceIndex) {
	std::string NPC;
	std::string dismantleEvent;

	switch (resourceIndex) {
		case 0: // oxygen
			NPC = "Alisa {Exploration Master}";
			dismantleEvent = "Atmospheric scrubbers have completely failed. Scrubbing reserves at 0%.\nLife support is offline across all decks. It's getting cold, Captain...\nMay the void take us quietly.";
			break;
		case 1: //Water
			NPC = "Carrie {Aeronautics Engineer}";
			dismantleEvent = "Hydro-recyclers burned out 12 hours ago. Condensation traps are bone dry.\nCooling loops are overheating and crew delirium is setting in.\nWe can't survive another cycle without fluid.";
			break;
		case 2: // food
			NPC = "Vance {Combat Warden}";
			dismantleEvent = "Ration crates are completely bare. Mutiny broke out in the mess hall, and the crew no longer has the strength to stand watch.\nWe're starving out here, Captain.\nCommand has collapsed.";
			break;
		case 3: // scrap
			NPC = "Malik {Military Pilot}";
			dismantleEvent = "Hull structural integrity compromised! We ran out of scrap for patches—\nmicro-meteorites just breached the primary bulkhead!\nDecompression in 3... 2... Evacuate immediately!";
			break;
		default:
			NPC = "CENTRAL AI {System Warning}";
			dismantleEvent = "CRITICAL SYSTEM FAILURE: Multiple life-support parameters dropped below operational threshold.\nColony status: TERMINATED.";
			break;
	}

	printDialogue(
		"TERMINATION WARNING",
		NPC,
		dismantleEvent
	);
	sleep(8);
}

// Scenes
void PlayerTerminal::bootSequence() {
	clearTerminal();

	printEqualLine(54);
	printBars();
	printBars();

	printCenteredLine("--- DEEP SPACE LOGISTICS ---",50,true);
	printCenteredLine("BOOTING SECTOR OS v4.2",50,true);

	printBars();
	printBars();
	printEqualLine(54);

	std::cout << "\nConnecting to Sector Relay...\n";
	std::cout << "[" << std::setw(20) << std::setfill('-') << "" << "]";

	jumpToStartOfLine();
	goRightXChars(1);

	for (int i = 0; i < 20; ++i) {
		std::cout << "#" << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	sleep(2);
	std::cout << std::endl;
}

void PlayerTerminal::newGameInputSequence(std::string& outPlayerName, std::string& outVesselName) {
	clearTerminal();

	printEqualLine(54);

	printCenteredLine("--- NEW SESSION INITIALIZATION ---", 54);
	std::cout << std::endl;

	printEqualLine(54);

	std::cout << "      COMMANDER NAME : [" << std::setfill('-') << std::setw(20) << "" << "]\n";
	std::cout << "      VESSEL REGISTRY: [" << std::setw(20) << "" << "]" << std::endl;

	printEqualLine(54);

	// Go to the commander line
	jumpToStartOfLine();
	goUpXLines(3);
	goRightXChars(24);

	// await name input
	std::string playerName = awaitStrInput("");
	while (playerName.empty()) {
		goUpXLines(1);
		jumpToStartOfLine();
		goRightXChars(24);
		playerName = awaitStrInput("");
	}
	outPlayerName = playerName;
	outPlayerName.resize(20, ' ');

	// go back to the commander name location
	goUpXLines(1);
	jumpToStartOfLine();
	goRightXChars(24);

	// print the new name cut off
	std::cout << outPlayerName << "]" << std::flush;
	clearRestOfLine();

	// Go to Vessel Registry Line
	goDownXLines(1);
	jumpToStartOfLine();
	goRightXChars(24);

	// change the fill to dashes
	std::cout << std::setfill('-') << std::setw(20) << "" << std::flush;
	goLeftXChars(20);

	// Wait for the user to type the name
	std::string vesselName = awaitStrInput("");
	while (vesselName.empty()) {
		goUpXLines(1);
		jumpToStartOfLine();
		goRightXChars(24);
		vesselName = awaitStrInput("");
	}
	outVesselName = vesselName;
	outVesselName.resize(20, ' ');

	//go back to the vessel locaiton
	goUpXLines(1);
	jumpToStartOfLine();
	goRightXChars(24);

	// print the new name cut off
	std::cout << outVesselName << "]" << std::flush;
	clearRestOfLine();

	// go to the next blank line
	goDownXLines(2);
	jumpToStartOfLine();
	printCenteredLine("--- Registering Ship with Space Command ---", 54);
	std::cout << std::endl;
	printEqualLine(54);

	//wait 5 seconds
	sleep(5);

	// Stage 2: registered Ship
	goUpXLines(2);
	jumpToStartOfLine();
	clearRestOfLine();
	printCenteredLine("--- Success: Ship Registered! ---", 54);
	goDownXLines(2);
	jumpToStartOfLine();

	// wait 2 seconds
	sleep(2);
}

void PlayerTerminal::printMainMenu() {
	clearTerminal();

	printEqualLine(54);
	printCenteredLine("=== MAIN MENU ===");
	std::cout << std::endl;
	printEqualLine(54);

	printBars();

	// Menu Options
	printCenteredLine("[1] REGISTER NEW VEHICLE (NEW GAME)",50,true);
	printCenteredLine("[2] SEARCH VEHICLE REGISTRY (LOAD GAME)",50,true);
	printCenteredLine("[3] SECTOR LEADERBOARD (LEADERBOARD)",50,true);
	printCenteredLine("[4] TERMINAL CONFIG (SETTINGS)",50,true);
	printCenteredLine("[5] ABANDON VESSEL (QUIT)",50,true);

	printBars();
	printEqualLine(54);
	std::cout << std::endl;

	std::cout << "SEC-OS:// INPUT DIRECTIVE > ";
}

void PlayerTerminal::leaderboardSequence(const std::vector<LeaderboardEntry>& leaderboard, const int& pageNumber, const int& pageCount) {
	clearTerminal();

	//head
	printEqualLine(54);
	printCenteredLine("--- GALACTIC RECORDS (PG " + std::to_string(pageNumber) + "/" + std::to_string(pageCount) + ")---", 54);
	std::cout << std::endl;
	printEqualLine(54);

	//page layout
	std::cout << "||" << " RNK  DAYS  " 
		<< std::setfill(' ') << std::setw(19) << std::left << "COMMANDER" 
		<< std::setw(19) << std::left << "VESSEL" << "||" << std::endl;

	std::cout << "||" << std::setw(52) << std::setfill('-') << std::right << "||" << std::endl;

	//entries
	int startingIndex = (pageNumber-1) * 10;
	int endingIndex = leaderboard.size() >= startingIndex + 10 ? startingIndex + 10 : leaderboard.size();
	int emptyEntries = 10 - (endingIndex - startingIndex);

	for (; startingIndex < endingIndex; startingIndex++) {
		printLeaderBoardEntry(leaderboard.at(startingIndex), startingIndex+1);
	}
	for (; emptyEntries > 0; emptyEntries--) {
		printBars();
	}

	//tail
	printEqualLine(54);
	printCenteredLine("[N] NEXT PAGE  |  [P] PREVIOUS PAGE  | [Q] QUIT",50,true);
	printEqualLine(54);
	std::cout << "SEC-OS://RECORDS/CMD > ";
}

void PlayerTerminal::printLeaderBoardEntry(const LeaderboardEntry& entry, const int& rank) {
	std::string rankString = std::to_string(rank);
	if (rank >= 1000) {
		rankString.resize(2);
		rankString += "+";
	}
	rankString.resize(3, ' ');
	std::cout << "||";
	std::cout << " " + rankString;

	std::string dayString = std::to_string(entry.score);
	if (dayString.size() > 4) {
		dayString.resize(3);
		dayString += "+";
	}
	dayString.resize(4, ' ');

	std::cout << "  " + dayString;

	std::string stringCopy = entry.playerName;
	if (stringCopy.size() > 17) {
		stringCopy.resize(15);
		stringCopy += "..";
	}
	stringCopy.resize(19, ' ');
	std::cout << "  " + stringCopy;

	stringCopy = entry.vehicleName;
	if (stringCopy.size() > 17) {
		stringCopy.resize(15);
		stringCopy += "..";
	}
	stringCopy.resize(19, ' ');
	std::cout << stringCopy;
	std::cout << "||" << std::endl;
}

void PlayerTerminal::settingsSequence(std::string& eventsFile, std::string& leaderboardFile) {
	clearTerminal();

	//HEAD
	printEqualLine(54);
	printCenteredLine("=== SETTINGS ===");
	std::cout << std::endl;
	printEqualLine(54);

	std::cout << "||" << std::setw(52) << std::setfill('-') << "||" << std::endl;
	printCenteredLine("--- WARNING: FILE LENGTH LIMIT: 20 ---",50,true);
	std::cout << "||" << std::setw(52) << std::setfill('-') << "||" << std::endl;

	printBars();
	printBars();

	std::cout << "||     Events File      : [";
	std::cout << std::setfill('-') << std::setw(20) << std::left << eventsFile << "]    ||" << std::endl;

	std::cout << "||     Leaderboard File : [";
	std::cout << std::setfill(' ') << std::setw(20) << std::left << leaderboardFile << "]    ||" << std::endl;

	//TAIL
	printBars();
	printBars();
	printEqualLine(54);

	//align input location to events
	goUpXLines(5);
	jumpToStartOfLine();
	goRightXChars(27);

	std::string input = awaitStrInput("");
	
	if (!input.empty()) {
		eventsFile = input;
		if (eventsFile.size() > 20) {
			eventsFile.resize(20);
		}

		if (eventsFile.size() < 4) {
			eventsFile += ".txt";
		}

		if (eventsFile.substr(eventsFile.length() - 4) != ".txt") {
			if (eventsFile.size() <= 16) {
				eventsFile += ".txt";
			}
			else {
				eventsFile.resize(eventsFile.size() - 4);
				eventsFile += ".txt";
			}
		}
	}
	
	std::string tempPrint = eventsFile;
	tempPrint.resize(20,'*');

	//Show the new locaiton
	goUpXLines(1);
	jumpToStartOfLine();
	clearRestOfLine();
	std::cout << "||     Events File      : [" << tempPrint << "]    ||" << std::endl;


	//align input to Leaderboard
	jumpToStartOfLine();
	clearRestOfLine();

	std::cout << "||     Leaderboard File : [";
	std::cout << std::setfill('-') << std::left << std::setw(20) << leaderboardFile << "]    ||";

	jumpToStartOfLine();
	goRightXChars(27);

	input = awaitStrInput("");

	if (!input.empty()) {
		leaderboardFile = input;
		if (leaderboardFile.size() > 20) {
			leaderboardFile.resize(20);
		}

		if (leaderboardFile.size() < 4) {
			leaderboardFile += ".txt";
		}

		if (leaderboardFile.substr(leaderboardFile.length() - 4) != ".txt") {
			if (leaderboardFile.size() <= 16) {
				leaderboardFile += ".txt";
			}
			else {
				leaderboardFile.resize(leaderboardFile.size() - 4);
				leaderboardFile += ".txt";
			}
		}
	}

	tempPrint = leaderboardFile;
	tempPrint.resize(20, '*');

	goUpXLines(1);
	jumpToStartOfLine();
	clearRestOfLine();

	std::cout << "||     Leaderboard File : [" << tempPrint << "]    ||" << std::endl;

	goDownXLines(3);
	jumpToStartOfLine();

	sleep(5);
}

bool PlayerTerminal::welcomeBackSequence(const std::string& playerName, const std::string& vesselName) {
	printDialogue(
		"Response Request",
		"Space Command (Amaia)",
		"\"Captain " + playerName +
		", we need you to respond, there are urgent events happening in your vessel, " +
		vesselName + "!\n Please respond or a rescue fleet will be sent for clean up.\"\n",
		"Respond to Space Command? (Y/N)\n\nY to return to your ship (Continue Save) \nN to accept rescue fleet (Abandon Save)"
	);

	std::string prompt = "Your Command, Captain " + playerName + ": ";
	std::cout << prompt;


	while (true) {
		char response = awaitCharInputInline(prompt);

		if (response == 'Y') {
			return true;
		}
		else if (response == 'N') {
			return false;
		}

		goUpXLines(1);
		jumpToStartOfLine();
		clearRestOfLine();
		std::cout << prompt;
	}
}

bool PlayerTerminal::startSequence(const std::string& playerName, const std::string& vesselName) {
	printDialogue(
		"Liftoff Request",
		"Space Command (Ryla)",

		"Captain " + playerName +
		", we have noticed a surge of heat and fuel usage in your area.\n Supposedly for you new vessel, " + vesselName
		+ "\n You may be new to this but you need to send us a liftoff request before you do so. \n Are you commencing lift off?",

		"Confirm liftoff with Space Command? (L/N)\n\nL to lift off, N to bail"
	);

	std::string prompt = "Your Command, Captain " + playerName + ": ";
	std::cout << prompt;

	while (true) {
		char response = awaitCharInputInline(prompt);

		if (response == 'L') {
			return true;
		}
		else if (response == 'N') {
			return false;
		}

		goUpXLines(1);
		jumpToStartOfLine();
		clearRestOfLine();
		std::cout << prompt;
	}
}

std::string PlayerTerminal::registryLookup() {
	clearTerminal();

	//HEAD
	std::cout << std::setw(54) << std::setfill('=') << "" << std::endl;
	printCenteredLine("=== REGIRTRY LOOKUP ===");
	std::cout << std::endl;
	std::cout << std::setw(54) << "" << std::endl;

	std::cout << "||" << std::setw(52) << std::setfill('-') << "||" << std::endl;
	std::cout << "||";
	printCenteredLine("--- WARNING: FILE LENGTH LIMIT: 20 ---");
	std::cout << "||" << std::endl;
	std::cout << "||" << std::setw(52) << std::setfill('-') << "||" << std::endl;


	printBars();
	printBars();

	std::cout << "||";
	printCenteredLine("Vessel Name: [--------------------]");
	std::cout << "||" << std::endl;

	printBars();
	printBars();
	std::cout << std::setw(54) << std::setfill('=') << "" << std::endl;

	//align input location
	goUpXLines(4);
	goRightXChars(23);

	std::string input = awaitStrInput("");

	if (input.size() > 20) {
		input.resize(20);
	}

	std::string tempPrint = input;
	tempPrint.resize(20, '*');

	//Show the new locaiton
	goUpXLines(1);
	jumpToStartOfLine();
	clearRestOfLine();

	std::cout << "||";
	printCenteredLine("Vessel Name: [" + tempPrint + "]");
	std::cout << "||" << std::endl;

	goDownXLines(3);
	jumpToStartOfLine();
	sleep(3);

	return input;
}