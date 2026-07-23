/*
Max A. Allen
Void Bound
A Sci-Fi terminal game aiming to provide a more dynamic experience by
using relationships to determine a middle-ground event
*/

#include "EventManager.h"
#include "PlayerTerminal.h"
#include "ExternalStateManager.h"

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
void PlayerTerminal::goUpXLines(int count) {
	std::cout << "\033[" << std::to_string(count) << "A" << std::flush;
	std::cout << std::flush;
}
void PlayerTerminal::goDownXLines(int count) {
	std::cout << "\033[" << std::to_string(count) << "B" << std::flush;
	std::cout << std::flush;
}
void PlayerTerminal::goRightXChars(int count) {
	std::cout << "\033[" << std::to_string(count) << "C" << std::flush;
	std::cout << std::flush;
}
void PlayerTerminal::goLeftXChars(int count) {
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
	std::cout << "||" << std::setw(width + 2) << 
		std::setfill(' ') << std::right << "||" << std::endl;
}
void PlayerTerminal::printEqualLine(int width) {
	std::cout << std::setw(width) << std::setfill('=') << "" << std::endl;
}
std::string PlayerTerminal::formatDouble(const double& number) {
	return std::format("{:.2f}", static_cast<double>(number));
}

int PlayerTerminal::awaitValidCharInput(const std::vector<char>& validInputs, const std::string& prompt, const std::string& invalidResponse) {
	std::string thisInput;

	while (true) {
		std::getline(std::cin, thisInput);

		while (thisInput.size() != 1) {
			if (invalidResponse.empty()) { // use an inline
				goUpXLines(1);
				jumpToStartOfLine();
				clearRestOfLine();
				std::cout << prompt << "(Invalid Input Length: " << thisInput << "): ";
			}
			else {
				sleepUpdateIncorrectInput(prompt, invalidResponse);
			}


			std::getline(std::cin, thisInput);
		}
		// means that its only a single char

		int validIndex = -1;
		for (int i = 0; i < validInputs.size(); i++) {
			if (std::toupper(thisInput.at(0)) == std::toupper(validInputs.at(i))) {
				validIndex = i;
				break;
			}
		}

		if (validIndex != -1) {
			return validIndex;
		}
		else {
			if (invalidResponse.empty()) { // use an inline
				goUpXLines(1);
				jumpToStartOfLine();
				clearRestOfLine();
				std::cout << prompt << "(Invalid Option: " << thisInput << "): ";
			}
			else {
				sleepUpdateIncorrectInput(prompt, invalidResponse);
			}
		}
	}
}

std::string PlayerTerminal::awaitStrInput(const std::string& prompt) {
	std::string out;

	std::cout << prompt;
	std::getline(std::cin, out);

	return out;
}

void PlayerTerminal::printCenteredLine(const std::string& text, int width, bool bars) {
	int padding = width - text.length();
	int leftPad = padding / 2;
	int rightPad = padding - leftPad;

	if (bars) {
		std::cout << "||";
	}
	std::cout << std::string(leftPad, ' ') << text << std::string(rightPad, ' ');
	if (bars) {
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
void PlayerTerminal::printDialogue(const std::string& type, const std::string& npc, const std::string& response, const std::string& prompt) {
	clearTerminal();

	printEqualLine();
	std::cout << "FROM: " << npc << std::endl;
	std::cout << "TYPE: " << type << std::endl;
	std::cout << std::setw(NONTERMINAL_SIZE) << 
		std::setfill('-') << "" << std::endl;
	std::cout << '\"' << response << '\"' << std::endl;
	printEqualLine();

	if (!prompt.empty()) {
		std::cout << prompt << std::endl << std::endl;

	}
}
void PlayerTerminal::printEvent(const Event& event, const std::string& playerName) {
	printDialogue(
		event.header,
		event.npc,
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

	std::cout << "\nC to attempt to convince this npc for a middle ground\nR to view resources\nQ to quit" << std::endl;
	std::cout << "Your Command, Captain " << playerName << ": ";
}
void PlayerTerminal::printResponse(const std::string& dialogue, const std::string& npc) {
	printDialogue(
		"RESPONSE",
		npc,
		dialogue
	);
}
void PlayerTerminal::printResources(const double resources[4]) {
	clearTerminal();
	printEqualLine();
	printBars(RESOURCES_SIZE);
	printCenteredLine("CALCULATING RESOURCES...", RESOURCES_SIZE, true);
	printBars(RESOURCES_SIZE);
	printEqualLine();

	// pretend to do smth
	sleep(2);
	goUpXLines(5);
	clearLines(5);

	// clear the data and show the resources
	printEqualLine();
	printBars(RESOURCES_SIZE);

	printCenteredLine(
		"OXYGEN :" + std::to_string(resources[0]), 
		RESOURCES_SIZE, 
		true
	);
	printCenteredLine(
		"WATER  :" + std::to_string(resources[1]), 
		RESOURCES_SIZE, 
		true
	);
	printCenteredLine(
		"FOOD  :" + std::to_string(resources[2]), 
		RESOURCES_SIZE, 
		true
	);
	printCenteredLine(
		"SCRAP :" + std::to_string(resources[3]), 
		RESOURCES_SIZE, 
		true
	);

	printBars(RESOURCES_SIZE);
	printEqualLine();

	awaitStrInput("SEC-OS://RESOURCES (TYPE ANYTHING) > "); 
	// just wait until they type smth
}
void PlayerTerminal::printDeath(int resourceIndex) {
	std::string npc;
	std::string dismantleEvent;

	switch (resourceIndex) {
	case 0: // oxygen
		npc = "Alisa {Exploration Master}";
		dismantleEvent = "Atmospheric scrubbers have completely failed. Scrubbing reserves at 0%.\n"
			"Life support is offline across all decks. It's getting cold, Captain...\n"
			"May the void take us quietly.";
		break;
	case 1: //Water
		npc = "Carrie {Aeronautics Engineer}";
		dismantleEvent = "Hydro-recyclers burned out 12 hours ago. Condensation traps "
			"are bone dry.\nCooling loops are overheating and crew "
			"delirium is setting in.\nWe can't survive another cycle "
			"without fluid.";
		break;
	case 2: // food
		npc = "Vance {Combat Warden}";
		dismantleEvent = "Ration crates are completely bare. Mutiny broke out in "
			"the mess hall, and the crew no longer has the strength "
			"to stand watch.\nWe're starving out here, Captain.\n"
			"Command has collapsed.";
		break;
	case 3: // scrap
		npc = "Malik {Military Pilot}";
		dismantleEvent = "Hull structural integrity compromised! We ran out of scrap "
			"for patches—\nmicro-meteorites just breached the primary "
			"bulkhead!\nDecompression in 3... 2... Evacuate immediately!";
		break;
	default:
		npc = "CENTRAL AI {System Warning}";
		dismantleEvent = "CRITICAL SYSTEM FAILURE: Multiple life-support parameters "
			"dropped below operational threshold.\nColony status: TERMINATED.";
		break;
	}

	printDialogue(
		"TERMINATION WARNING",
		npc,
		dismantleEvent
	);
	sleep(8);
}

// Scenes
void PlayerTerminal::bootSequence() {
	clearTerminal();

	printEqualLine(TERMINAL_SIZE);
	printBars();
	printBars();

	printCenteredLine("--- DEEP SPACE LOGISTICS ---", TERMINAL_EMPTY, true);
	printCenteredLine("BOOTING SECTOR OS v4.2", TERMINAL_EMPTY, true);

	printBars();
	printBars();
	printEqualLine(TERMINAL_SIZE);

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

void PlayerTerminal::newGameInputSequence(std::string& plrName, std::string& vslName) {
	clearTerminal();

	printEqualLine(TERMINAL_SIZE);

	printCenteredLine("--- NEW SESSION INITIALIZATION ---", TERMINAL_SIZE);
	std::cout << std::endl;

	printEqualLine(TERMINAL_SIZE);

	std::cout << "      COMMANDER NAME : [" << 
		std::setfill('-') << std::setw(20) << "" << "]\n";

	std::cout << "      VESSEL REGISTRY: [" << 
		std::setw(20) << "" << "]" << std::endl;

	printEqualLine(TERMINAL_SIZE);

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
	plrName = playerName;
	plrName.resize(20, ' ');

	// go back to the commander name location
	goUpXLines(1);
	jumpToStartOfLine();
	goRightXChars(24);

	// print the new name cut off
	std::cout << plrName << "]" << std::flush;
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
	vslName = vesselName;
	vslName.resize(20, ' ');

	//go back to the vessel locaiton
	goUpXLines(1);
	jumpToStartOfLine();
	goRightXChars(24);

	// print the new name cut off
	std::cout << vslName << "]" << std::flush;
	clearRestOfLine();

	// go to the next blank line
	goDownXLines(2);
	jumpToStartOfLine();
	printCenteredLine(
		"--- Registering Ship with Space Command ---", 
		TERMINAL_SIZE
	);
	std::cout << std::endl;
	printEqualLine(TERMINAL_SIZE);

	//wait 5 seconds
	sleep(5);

	// Stage 2: registered Ship
	goUpXLines(2);
	jumpToStartOfLine();
	clearRestOfLine();
	printCenteredLine(
		"--- Success: Ship Registered! ---", 
		TERMINAL_SIZE
	);
	goDownXLines(2);
	jumpToStartOfLine();

	// wait 2 seconds
	sleep(2);
}

void PlayerTerminal::printMainMenu() {
	clearTerminal();

	printEqualLine(TERMINAL_SIZE);
	printCenteredLine("=== MAIN MENU ===");
	std::cout << std::endl;
	printEqualLine(TERMINAL_SIZE);

	printBars();

	// Menu Options
	printCenteredLine(
		"[1] REGISTER NEW VEHICLE (NEW GAME)", 
		TERMINAL_EMPTY, 
		true
	);
	printCenteredLine(
		"[2] SEARCH VEHICLE REGISTRY (LOAD GAME)", 
		TERMINAL_EMPTY, 
		true
	);
	printCenteredLine(
		"[3] SECTOR LEADERBOARD (LEADERBOARD)",
		TERMINAL_EMPTY, 
		true
	);
	printCenteredLine(
		"[4] TERMINAL CONFIG (SETTINGS)", 
		TERMINAL_EMPTY, 
		true
	);
	printCenteredLine(
		"[5] ABANDON VESSEL (QUIT)", 
		TERMINAL_EMPTY, 
		true
	);

	printBars();
	printEqualLine(TERMINAL_SIZE);
	std::cout << std::endl;

	std::cout << "SEC-OS:// INPUT DIRECTIVE > ";
}

void PlayerTerminal::leaderboardSequence(const std::vector<LeaderboardEntry>& board, int page, int count) {
	clearTerminal();

	//head
	std::string pageSection = "(PG " + 
		std::to_string(page) + " / " +
		std::to_string(count) + ") ";

	printEqualLine(TERMINAL_SIZE);
	printCenteredLine(
		"--- GALACTIC RECORDS" + pageSection + "---",
		TERMINAL_SIZE
	);
	std::cout << std::endl;
	printEqualLine(TERMINAL_SIZE);

	//page layout
	std::cout << "||" << " RNK  DAYS  "
		<< std::setfill(' ') << std::setw(19) << std::left << "COMMANDER"
		<< std::setw(19) << std::left << "VESSEL" << "||" << std::endl;

	std::cout << "||" << std::setw(TERMINAL_EMPTY) << 
		std::setfill('-') << "" << "||" << std::endl;

	//entries
	int startingIndex = (page - 1) * 10;

	int endingIndex = board.size() >= startingIndex + 10 ? 
		startingIndex + 10 : board.size();

	int emptyEntries = 10 - (endingIndex - startingIndex);

	for (; startingIndex < endingIndex; startingIndex++) {
		printLeaderBoardEntry(board.at(startingIndex), startingIndex + 1);
	}
	for (; emptyEntries > 0; emptyEntries--) {
		printBars();
	}

	//tail
	printEqualLine(TERMINAL_SIZE);
	printCenteredLine(
		"[N] NEXT PAGE  |  [P] PREVIOUS PAGE  | [Q] QUIT", 
		TERMINAL_EMPTY, 
		true);
	printEqualLine(TERMINAL_SIZE);
	std::cout << "SEC-OS://RECORDS/CMD > ";
}

void PlayerTerminal::printLeaderBoardEntry(const LeaderboardEntry& entry, int rank) {
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

void PlayerTerminal::settingsSequence(std::string& evntFile, std::string& lboardFile) {

	clearTerminal();

	//HEAD
	printEqualLine(TERMINAL_SIZE);
	printCenteredLine("=== SETTINGS ===");
	std::cout << std::endl;
	printEqualLine(TERMINAL_SIZE);

	std::cout << "||" << std::setw(TERMINAL_EMPTY) << 
		std::setfill('-') << "" << "||" << std::endl;
	printCenteredLine("--- WARNING: FILE LENGTH LIMIT: 20 ---", 
		TERMINAL_EMPTY, 
		true);
	std::cout << "||" << std::setw(TERMINAL_EMPTY) << 
		std::setfill('-') << "" << "||" << std::endl;

	printBars();
	printBars();

	std::cout << "||     Events File      : [";
	std::cout << std::setfill('-') << std::setw(20) << 
		std::left << evntFile << "]    ||" << std::endl;

	std::cout << "||     Leaderboard File : [";
	std::cout << std::setfill(' ') << std::setw(20) << 
		std::left << lboardFile << "]    ||" << std::endl;

	//TAIL
	printBars();
	printBars();
	printEqualLine(TERMINAL_SIZE);

	//align input location to events
	goUpXLines(5);
	jumpToStartOfLine();
	goRightXChars(27);

	std::string input = awaitStrInput("");

	if (!input.empty()) {
		evntFile = input;
		if (evntFile.size() > 20) {
			evntFile.resize(20);
		}

		if (evntFile.size() < 4) {
			evntFile += ".txt";
		}

		if (evntFile.substr(evntFile.length() - 4) != ".txt") {
			if (evntFile.size() <= 16) {
				evntFile += ".txt";
			}
			else {
				evntFile.resize(evntFile.size() - 4);
				evntFile += ".txt";
			}
		}
	}

	std::string tempPrint = evntFile;
	tempPrint.resize(20, '*');

	//Show the new locaiton
	goUpXLines(1);
	jumpToStartOfLine();
	clearRestOfLine();
	std::cout << "||     Events File      : [" << 
		tempPrint << "]    ||" << std::endl;


	//align input to Leaderboard
	jumpToStartOfLine();
	clearRestOfLine();

	std::cout << "||     Leaderboard File : [";
	std::cout << std::setfill('-') << std::left << 
		std::setw(20) << lboardFile << "]    ||";

	jumpToStartOfLine();
	goRightXChars(27);

	input = awaitStrInput("");

	if (!input.empty()) {
		lboardFile = input;
		if (lboardFile.size() > 20) {
			lboardFile.resize(20);
		}

		if (lboardFile.size() < 4) {
			lboardFile += ".txt";
		}

		if (lboardFile.substr(lboardFile.length() - 4) != ".txt") {
			if (lboardFile.size() <= 16) {
				lboardFile += ".txt";
			}
			else {
				lboardFile.resize(lboardFile.size() - 4);
				lboardFile += ".txt";
			}
		}
	}

	tempPrint = lboardFile;
	tempPrint.resize(20, '*');

	goUpXLines(1);
	jumpToStartOfLine();
	clearRestOfLine();

	std::cout << "||     Leaderboard File : [" << 
		tempPrint << "]    ||" << std::endl;

	goDownXLines(3);
	jumpToStartOfLine();

	sleep(5);
}

bool PlayerTerminal::welcomeBackSequence(const std::string& plrName, const std::string& vslName) {

	printDialogue(
		"Response Request",
		"Space Command (Amaia)",
		"\"Captain " + plrName +
		", we need you to respond, there are urgent events happening in your vessel, " +
		vslName + "!\n Please respond or a rescue fleet will be sent for clean up.\"\n",
		"Respond to Space Command? (Y/N)\n\nY to return to your ship (Continue Save) \nN to accept rescue fleet (Abandon Save)"
	);

	std::string prompt = "Your Command, Captain " + plrName + ": ";
	std::cout << prompt;

	std::vector<char> validOptions = { 'N', 'Y' };
	return awaitValidCharInput(validOptions, prompt);
}

bool PlayerTerminal::startSequence(const std::string& plrName, const std::string& vslName) {

	printDialogue(
		"Liftoff Request",
		"Space Command (Ryla)",

		"Captain " + plrName +
		", we have noticed a surge of heat and fuel usage in your area.\n Supposedly for you new vessel, " + vslName
		+ "\n You may be new to this but you need to send us a liftoff request before you do so. \n Are you commencing lift off?",

		"Confirm liftoff with Space Command? (L/N)\n\nL to lift off, N to bail"
	);

	std::string prompt = "Your Command, Captain " + plrName + ": ";
	std::cout << prompt;

	std::vector<char> validOptions = { 'N', 'L' };
	return awaitValidCharInput(validOptions, prompt);
}

std::string PlayerTerminal::registryLookup() {
	clearTerminal();

	//HEAD
	printEqualLine(TERMINAL_SIZE);
	printCenteredLine("=== REGISTRY LOOKUP ===");
	std::cout << std::endl;
	printEqualLine(TERMINAL_SIZE);

	std::cout << "||" << std::setw(TERMINAL_EMPTY) 
		<< std::setfill('-') << "" << "||" << std::endl;

	printCenteredLine(
		"--- WARNING: FILE LENGTH LIMIT: 20 ---", 
		TERMINAL_EMPTY, 
		true
	);

	std::cout << "||" << std::setw(TERMINAL_EMPTY) 
		<< std::setfill('-') << "" << "||" << std::endl;

	printBars();
	printBars();

	printCenteredLine(
		"Vessel Name: [--------------------]", 
		TERMINAL_EMPTY, 
		true
	);

	printBars();
	printBars();
	printEqualLine(TERMINAL_SIZE);

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

	printCenteredLine("Vessel Name: [" + tempPrint + "]", TERMINAL_EMPTY, true);

	goDownXLines(3);
	jumpToStartOfLine();
	sleep(3);

	return input;
}