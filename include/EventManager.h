/*
Max A. Allen
Void Bound
A Sci-Fi terminal game aiming to provide a more dynamic experience by
using relationships to determine a middle-ground event
*/

#ifndef EVENT_MANAGER

#define EVENT_MANAGER

#include <string>
#include <vector>

struct Event {
	bool isUltimatum = false;
	bool relationshipUltimatum = false;
	std::string header;
	std::string npc;
	std::string dialogue;
	std::string prompt;

	// oxygen, water, food, scrap
	double acceptResources[4] = { 0 };
	double counterBaseResources[4] = { 0 };
	double denyResources[4] = { 0 };

	//engineer, pilot, adventurer, Combat Warden
	double acceptRelationships[4] = { 0 };
	double counterBaseRelationships[4] = { 0 };
	double denyRelationships[4] = { 0 };

	double counterResourceModifier[4] = { 0 }; // increment this 1 every 50 points, otherwise round to 2 decimal places
	double counterRelationshipModifier[4] = { 0 }; // increment this 1 every 50 points, otherwise round to 2 decimal places

	std::string acceptResponse;
	std::string denyResponse;
	std::string counterNeutralResponse;
	std::string counterNegativeResponse;
	std::string counterPositiveResponse;
};

enum class readingState {
	Head,
	Accept,
	Deny,
	Counter
};

class EventManager {
	std::vector<Event> masterEventPool;
	std::vector<Event> relationshipUltimatumPool;
	std::string previousPool[5];
	int previousPoolHead = 0;
	std::string fileName;

	void logEventHistory(const std::string& eventHeader);
	bool checkEventSanitizaiton(const Event& event);
	void setEventByName(const std::string& keyName, const std::string& value, const readingState& RS, Event& event);
	std::vector<double> splitString(const std::string&);

	std::string listToString(const double list[4]);
public:

	EventManager(const std::string& eventFilePath) : fileName(eventFilePath) {};
	EventManager() : EventManager("Unkown.txt") {};


	void saturatePools();
	Event getRandomEvent();
	void printAllEventDetails(const Event& event);
	std::string getPreviousEventHeader(int index) { return previousPool[index]; }
};

#endif // !EVENT_MANAGER