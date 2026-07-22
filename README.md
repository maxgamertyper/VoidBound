# VoidBound

A terminal-based sci-fi survival and decision-making simulation written in C++. Captain a deep-space vessel, manage critical life-support resources, and interact with crew members through a dynamic persuasion/counter-offer system. Written for my C++ class in the summer semester of 2026

---

## How to Run the Program

### Prerequisites
* Suggested OS: **Windows 10/11** (developed on Windows 11)

### Quick Start
1. Download and extract the latest `VoidBound.zip` release.
2. Ensure the executable (`VoidBound.exe`) remains in the **same directory** as the required support text files (`events.txt`, `settings.txt`, and `leaderboard.txt`).

> If a file is missing, `settings.txt` and `leaderboard.txt` can be dynamically generated, however events.txt cannot be due to the amount of customization avaliable in it.

3. If you would like a clear leaderboard file, you can delete `leaderboard.txt` or change the leaderboard file in settings.txt through the game settings menu or manually.
4. Double-click **`VoidBound.exe`** to launch the game directly in your terminal.

---

## Features Implemented

* **Dynamic Persuasion:** Interact with distinct crew members: Vance, the combat warden; Alisa, the exploration master; Malik, the military pilot; and Carrie, the aeronautics engineer. Each event changes your relationship with these people and as that relationship changes, so does the value of the persuasion metric.
* **Resource Management:** Track real-time status of vital resources (Oxygen, Water, Food, and Scrap). However, the relationships between you and NPCs are hidden.
* **Save File System:** auto-saves game state after each player response to an event by using the vessel name (`save{sanitized vessel name}.txt`) which can be then loaded again later.
* **Event Pool:** Has a very friendly event pool format making custom events very easy to implement. Additionally, tracks the previous 5 events so that players dont get repeated events super often.
* **Persistent Leaderboard:** Automatically logs completed game runs, vessel names, and total survived days to an external text file (`leaderboard.txt` or otherwise stated in `settings.txt`).
* **Terminal UI Utility:** Custom interface utilities featuring clean text-box borders, inline input loops, screen resets, and context-specific death messages.
* **Input Sanitization:** Sanitizes the majority of user input to ensure the game can run smoothly. Often has custom failure strings to show if there was an issue with the input. This includes custom files like (`leaderboard.txt` and `events.txt`)
> `settings.txt` does not have any sanitization as it consists of files to lookup and those can be really anything.

---

## Known Issues & Limitations

* **Terminal Resizing:** Standard ASCII box formatting and headers may wrap incorrectly if the command prompt window is manually shrunk below 80 columns wide.
* **Single Active Save Slot:** Currently, saving automatically updates the save file (`save<VesselName>.txt`). Starting a new game with an identical vessel name will overwrite previous progress.
* **Fixed Event Pool Limit:** The event duplicate filter tracks up to 5 previously seen events; once the history threshold is reached in a long run, events will begin recycling.
* **Save File Loading:** Players must know the name of a vessel to load the save file or look through the folder to find the save file with the vessel name attached. 
* **Typos:** I haven't seen any typos in a while, but I know I have a lot of trouble with typos so be aware their may be some typos.

---

## Credits & Attributions

* **Architecture & Logic:** C++ implementation is fully original and by me (File I/O parsing, state handling, game loops, terminal UI rendering). File format is also fully original and by me (`events.txt`).
* **Event Narrative Content:** Flavor text and event scenario datasets were generated with the assistance of AI tools to test event parsing at scale. (may change within the next week)
