#include <bits/stdc++.h>
using namespace std;

// --- DATA STRUCTURE DEFINITIONS ---

/*
    Node for a Singly Linked List to store attendees.
 */
struct AttendeeNode {
    string studentID;
    string studentName;
    AttendeeNode* next;

    AttendeeNode(string id, string name)
        : studentID(id), studentName(name), next(nullptr) {}
};

/**
 * Node to store Event details.
 */
struct EventNode {
    string eventID;
    string eventName;
    string date; // Format: DD/MM/YYYY (for display)
    string venue;
    string sortableDate; // Format: YYYYMMDD (for sorting)
    AttendeeNode* attendeeHead;
    EventNode* next; // Pointer for hash table chaining

    EventNode(string id, string name, string dt, string v, string sort_dt)
        : eventID(id), eventName(name), date(dt), venue(v), sortableDate(sort_dt), attendeeHead(nullptr), next(nullptr) {}
};

/**
 * The main EventTracker class.
 */
class EventTracker {
private:
    EventNode** hashTable;
    int TABLE_SIZE;
    string FILENAME = "events.txt";

    /**
     *
     * A simple hash function to map an Event ID string to an index.
     */
    int hashFunction(const string& eventID) {
        unsigned int hash = 0;
        for (char c : eventID) {
            hash = (hash * 31 + c) % TABLE_SIZE;
        }
        return hash;
    }

    /**
     * SEARCHING - Hash-Based
     */
    EventNode* findEvent(const string& eventID) {
        int index = hashFunction(eventID);
        EventNode* current = hashTable[index];

        while (current != nullptr) {
            if (current->eventID == eventID) {
                return current;
            }
            current = current->next;
        }
        return nullptr; // Not found
    }

    /**
     * [SORTING - Insertion Sort by Name]
     */
    void insertionSort(vector<EventNode*>& events) {
        for (int i = 1; i < events.size(); ++i) {
            EventNode* key = events[i];
            int j = i - 1;

            // Compare event names for sorting
            while (j >= 0 && events[j]->eventName > key->eventName) {
                events[j + 1] = events[j];
                j = j - 1;
            }
            events[j + 1] = key;
        }
    }

    /**
     * [SORTING - Insertion Sort by Date]
     */
    void insertionSortByDate(vector<EventNode*>& events) {
        for (int i = 1; i < events.size(); ++i) {
            EventNode* key = events[i];
            int j = i - 1;

            // Compare sortableDate string (YYYYMMDD)
            while (j >= 0 && events[j]->sortableDate > key->sortableDate) {
                events[j + 1] = events[j];
                j = j - 1;
            }
            events[j + 1] = key;
        }
    }


    /**
     * Utility function to clear the input buffer safely.
     */
    void clearInputBuffer() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    /**
     * Converts "DD/MM/YYYY" to "YYYYMMDD" for sorting.
     */
    string toSortableDate(const string& dd_mm_yyyy) {
        // DD/MM/YYYY
        // 0123456789
        return dd_mm_yyyy.substr(6, 4) + dd_mm_yyyy.substr(3, 2) + dd_mm_yyyy.substr(0, 2);
    }


    /**
     * [FILE HANDLING]
     * Loads all event and attendee data from the sequential file
     */
    void loadEventsFromFile() {
        ifstream inFile(FILENAME);
        if (!inFile) {
            cout << "[System] No existing event file found. Starting fresh." << endl;
            return;
        }

        string line;
        EventNode* currentEvent = nullptr;

        while (getline(inFile, line)) {
            if (line.empty()) continue;

            if (line == "END_EVENT") {
                currentEvent = nullptr;
                continue;
            }

            stringstream ss(line);
            string field1, field2, field3, field4, field5; // Added field5

            // Parse the line. A pipe '|' is our delimiter.
            getline(ss, field1, '|');
            getline(ss, field2, '|');
            getline(ss, field3, '|');
            getline(ss, field4, '|');
            getline(ss, field5, '|'); // Read the 5th field (sortableDate)

            if (!field5.empty()) { // Check for the 5th field now
                // This line has 5 fields, so it's an Event line
                currentEvent = new EventNode(field1, field2, field3, field4, field5); // Use 5-arg constructor
                int index = hashFunction(currentEvent->eventID);
                
                currentEvent->next = hashTable[index];
                hashTable[index] = currentEvent;
                
            } else if (currentEvent != nullptr && !field2.empty()) {
                
                AttendeeNode* newAttendee = new AttendeeNode(field1, field2);
                
                if (currentEvent->attendeeHead == nullptr) {
                    currentEvent->attendeeHead = newAttendee;
                } else {
                    AttendeeNode* temp = currentEvent->attendeeHead;
                    while (temp->next != nullptr) {
                        temp = temp->next;
                    }
                    temp->next = newAttendee;
                }
            }
        }
        inFile.close();
        cout << "[System] Successfully loaded events from " << FILENAME << endl;
    }

    // --- (These two functions are now called by the merged wrapper function) ---
    /**
     * [SEARCHING - Linear Search]
     * Searches for an event by name.
     */
    void searchEventByName() {
        string nameQuery;
        cout << "\n--- Search Event by Name ---\n";
        cout << "Enter Event Name (or part of it): ";
        getline(cin, nameQuery);

        bool found = false;
        cout << "\nSearch Results:\n";
        cout << "------------------------------------------------------------------\n";
        cout << left << setw(10) << "EventID"
             << setw(30) << "Name"
             << setw(15) << "Date"
             << setw(20) << "Venue" << "\n";
        cout << "------------------------------------------------------------------\n";

        for (int i = 0; i < TABLE_SIZE; ++i) {
            EventNode* current = hashTable[i];
            while (current != nullptr) {
                if (current->eventName.find(nameQuery) != string::npos) {
                    cout << left << setw(10) << current->eventID
                         << setw(30) << current->eventName
                         << setw(15) << current->date
                         << setw(20) << current->venue << "\n";
                    found = true;
                }
                current = current->next;
            }
        }

        if (!found) {
            cout << "No events found matching that name.\n";
        }
        cout << "------------------------------------------------------------------\n";
    }

    /**
     * [SORTING]
     * Displays all events, sorted alphabetically by name.
     */
    void viewEventsSortedByName() {
        cout << "\n--- All Events (Sorted by Name) ---\n";

        vector<EventNode*> allEvents;
        for (int i = 0; i < TABLE_SIZE; ++i) {
            EventNode* current = hashTable[i];
            while (current != nullptr) {
                allEvents.push_back(current);
                current = current->next;
            }
        }

        if (allEvents.empty()) {
            cout << "No events in the system yet.\n";
            return;
        }

        insertionSort(allEvents); // Sorts by name

        cout << "------------------------------------------------------------------\n";
        cout << left << setw(30) << "Name"
             << setw(10) << "EventID"
             << setw(15) << "Date"
             << setw(20) << "Venue" << "\n";
        cout << "------------------------------------------------------------------\n";

        for (EventNode* event : allEvents) {
            cout << left << setw(30) << event->eventName
                 << setw(10) << event->eventID
                 << setw(15) << event->date
                 << setw(20) << event->venue << "\n";
        }
        cout << "------------------------------------------------------------------\n";
    }

    /**
     * [SORTING]
     * Displays all events, sorted by date.
     */
    void viewEventsSortedByDate() {
        cout << "\n--- All Events (Sorted by Date) ---\n";

        vector<EventNode*> allEvents;
        for (int i = 0; i < TABLE_SIZE; ++i) {
            EventNode* current = hashTable[i];
            while (current != nullptr) {
                allEvents.push_back(current);
                current = current->next;
            }
        }

        if (allEvents.empty()) {
            cout << "No events in the system yet.\n";
            return;
        }

        insertionSortByDate(allEvents); // Sorts by date

        cout << "------------------------------------------------------------------\n";
        cout << left << setw(15) << "Date"
             << setw(30) << "Name"
             << setw(10) << "EventID"
             << setw(20) << "Venue" << "\n";
        cout << "------------------------------------------------------------------\n";

        for (EventNode* event : allEvents) {
            cout << left << setw(15) << event->date
                 << setw(30) << event->eventName
                 << setw(10) << event->eventID
                 << setw(20) << event->venue << "\n";
        }
        cout << "------------------------------------------------------------------\n";
    }


public:
    /**
     * Initializes the hash table and loads data from the file.
     */
    EventTracker(int size = 10) : TABLE_SIZE(size) {
        
        hashTable = new EventNode*[TABLE_SIZE];
        
        for (int i = 0; i < TABLE_SIZE; ++i) {
            hashTable[i] = nullptr;
        }
        
        // [File Handling]
        loadEventsFromFile();
    }

    /**
     * Destructor
     */
    ~EventTracker() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            EventNode* eventCurrent = hashTable[i];
            while (eventCurrent != nullptr) {
                EventNode* eventToDelete = eventCurrent;
                eventCurrent = eventCurrent->next;

                AttendeeNode* attCurrent = eventToDelete->attendeeHead;
                while (attCurrent != nullptr) {
                    AttendeeNode* attToDelete = attCurrent;
                    attCurrent = attCurrent->next;
                    delete attToDelete;
                }

                delete eventToDelete;
            }
        }
        delete[] hashTable;
    }

    /**
     * [FILE HANDLING]
     * Saves the entire in-memory hash table back to the
     * sequential file, overwriting it.
     */
    void saveEventsToFile() {
        ofstream outFile(FILENAME);
        if (!outFile) {
            cerr << "Error: Could not open " << FILENAME << " for writing." << endl;
            return;
        }

        for (int i = 0; i < TABLE_SIZE; ++i) {
            EventNode* event = hashTable[i];
            while (event != nullptr) {
                // Save all 5 fields now
                outFile << event->eventID << "|" << event->eventName << "|"
                        << event->date << "|" << event->venue << "|" << event->sortableDate << "\n";
                
                AttendeeNode* att = event->attendeeHead;
                while (att != nullptr) {
                    outFile << att->studentID << "|" << att->studentName << "\n";
                    att = att->next;
                }
                
                outFile << "END_EVENT\n";
                event = event->next;
            }
        }
        outFile.close();
        cout << "\n[System] All events saved to " << FILENAME << endl;
    }


    /**
     * [HASHING + LINKED LIST]
     * Adds a new event to the in-memory hash table..
     */
    void addEvent() {
        string id, name, date, venue;
        cout << "\n--- Add New Event --- \n";
        cout << "Enter Event ID (e.g., 'TECH01'): ";
        cin >> id;
        clearInputBuffer();

        if (findEvent(id) != nullptr) {
            cout << "Error: Event ID '" << id << "' already exists!\n";
            return;
        }

        cout << "Enter Event Name (e.g., 'CodeFest'): ";
        getline(cin, name);
        
        cout << "Enter Event Date (DD/MM/YYYY): ";
        cin >> date;
        clearInputBuffer();
        cout << "Enter Event Venue: ";
        getline(cin, venue);

        // Convert date to sortable format
        string sort_dt = toSortableDate(date);

        EventNode* newNode = new EventNode(id, name, date, venue, sort_dt);

        int index = hashFunction(id);
        
        newNode->next = hashTable[index];
        hashTable[index] = newNode;

        cout << "\nSuccess! Event '" << name << "' added.\n";

    }

    /**
     * [LINKED LIST]
     * Registers a student for an event.
     */
    void registerForEvent() {
        string eventID;
        cout << "\n--- Register for Event ---\n";
        cout << "Enter Event ID to register for: ";
        cin >> eventID;
        clearInputBuffer();

        EventNode* event = findEvent(eventID);

        if (event == nullptr) {
            cout << "Error: Event '" << eventID << "' not found.\n";
            return;
        }

        string studentID, studentName;
        cout << "Found Event: " << event->eventName << "\n";
        cout << "Enter your Student ID: ";
        cin >> studentID;
        clearInputBuffer();
        cout << "Enter your Full Name: ";
        getline(cin, studentName);

        AttendeeNode* newAttendee = new AttendeeNode(studentID, studentName);

        
        newAttendee->next = event->attendeeHead;
        event->attendeeHead = newAttendee;

        cout << "\nSuccess! " << studentName << " is registered for " << event->eventName << ".\n";
    }

    // --- 1. MERGED: `manageEvent` now includes Update and Remove ---
    void manageEvent() {
        string eventID;
        cout << "\n--- Manage/Remove Event ---\n";
        cout << "Enter Event ID to manage: ";
        cin >> eventID;
        clearInputBuffer();

        int index = hashFunction(eventID);
        EventNode* event = hashTable[index];
        EventNode* prev = nullptr;

        // Traverse the linked list (chain) to find the event
        while (event != nullptr && event->eventID != eventID) {
            prev = event;
            event = event->next;
        }

        // Case 1: Event not found
        if (event == nullptr) {
            cout << "Error: Event '" << eventID << "' not found.\n";
            return;
        }

        // Case 2: Event is found
        cout << "\nFound Event: " << event->eventName;
        cout << "\nWhat do you want to do?\n";
        cout << "  1. Update Event Name\n";
        cout << "  2. Update Event Date\n";
        cout << "  3. Update Event Venue\n";
        cout << "  4. --- DELETE THIS EVENT ---\n";
        cout << "  0. Cancel\n";
        cout << "Enter choice: ";

        int choice;
        cin >> choice;
        clearInputBuffer();

        switch (choice) {
            case 1: {
                string newName;
                cout << "Enter new Event Name: ";
                getline(cin, newName);
                event->eventName = newName;
                cout << "Success! Event Name updated.\n";
                break;
            }
            case 2: {
                string newDate;
                cout << "Enter new Event Date (DD/MM/YYYY): ";
                cin >> newDate;
                clearInputBuffer();
                event->date = newDate;
                event->sortableDate = toSortableDate(newDate); // Don't forget to update this!
                cout << "Success! Event Date updated.\n";
                break;
            }
            case 3: {
                string newVenue;
                cout << "Enter new Event Venue: ";
                getline(cin, newVenue);
                event->venue = newVenue;
                cout << "Success! Event Venue updated.\n";
                break;
            }
            case 4: {
                // --- This is the logic from your old `removeEvent` function ---
                cout << "Are you sure you want to delete '" << event->eventName << "'? (y/n): ";
                char confirm;
                cin >> confirm;
                if (confirm != 'y' && confirm != 'Y') {
                    cout << "Deletion cancelled.\n";
                    break;
                }

                // Unlink it from the list.
                if (prev == nullptr) {
                    hashTable[index] = event->next;
                } else {
                    prev->next = event->next;
                }
                
                // 1. Delete its attendee list
                AttendeeNode* attCurrent = event->attendeeHead;
                while (attCurrent != nullptr) {
                    AttendeeNode* attToDelete = attCurrent;
                    attCurrent = attCurrent->next;
                    delete attToDelete;
                }
                string removedEventName = event->eventName;
                
                // 2. Delete the event node itself
                delete event;

                cout << "\nSuccess! Event '" << removedEventName << "' has been removed.\n";
                break;
            }
            case 0:
                cout << "Action cancelled.\n";
                break;
            default:
                cout << "Invalid choice.\n";
                break;
        }
    }


    // --- 2. MERGED: `searchForEvent` (Search by ID or Name) ---
    void searchForEvent() {
        cout << "\n--- Search for Event ---\n";
        cout << "  1. Search by Event ID (View details)\n";
        cout << "  2. Search by Event Name (Find event)\n";
        cout << "  0. Back to Main Menu\n";
        cout << "Enter choice: ";

        int choice;
        cin >> choice;
        clearInputBuffer();

        switch (choice) {
            case 1: {
                // This is the logic from your old `viewEventDetails` function
                string eventID;
                cout << "\n--- Search by Event ID ---\n";
                cout << "Enter Event ID to view: ";
                cin >> eventID;
                clearInputBuffer();

                EventNode* event = findEvent(eventID);

                if (event == nullptr) {
                    cout << "Error: Event '" << eventID << "' not found.\n";
                    return;
                }

                cout << "\n----------------------------------------\n";
                cout << "Event ID:    " << event->eventID << "\n";
                cout << "Name:        " << event->eventName << "\n";
                cout << "Date:        " << event->date << "\n";
                cout << "Venue:       " << event->venue << "\n";
                cout << "----------------------------------------\n";
                cout << "Registered Attendees:\n";
                AttendeeNode* attCurrent = event->attendeeHead;
                if (attCurrent == nullptr) {
                    cout << "  (No attendees registered yet)\n";
                } else {
                    int count = 1;
                    while (attCurrent != nullptr) {
                        cout << "  " << count << ". " << attCurrent->studentName << " (ID: " << attCurrent->studentID << ")\n";
                        attCurrent = attCurrent->next;
                        count++;
                    }
                }
                cout << "----------------------------------------\n";
                break;
            }
            case 2:
                // This calls your existing `searchEventByName` function
                searchEventByName();
                break;
            case 0:
                return;
            default:
                cout << "Invalid choice.\n";
                break;
        }
    }


    // --- 3. MERGED: `viewAllEvents` (Sort by Name or Date) ---
    void viewAllEvents() {
        cout << "\n--- View All Events (Sorted) ---\n";
        cout << "  1. Sort by Event Name\n";
        cout << "  2. Sort by Event Date\n";
        cout << "  0. Back to Main Menu\n";
        cout << "Enter choice: ";

        int choice;
        cin >> choice;
        clearInputBuffer();

        switch (choice) {
            case 1:
                // This calls your existing `viewEventsSortedByName` function
                viewEventsSortedByName();
                break;
            case 2:
                // This calls your existing `viewEventsSortedByDate` function
                viewEventsSortedByDate();
                break;
            case 0:
                return;
            default:
                cout << "Invalid choice.\n";
                break;
        }
    }


    // --- (This function remains as it's called by the wrapper) ---
    void showStatistics() {
        cout << "\n--- System Statistics ---\n";

        int totalEvents = 0;
        int totalReg = 0;
        EventNode* popularEvent = nullptr;
        int maxAttendees = -1;

        for (int i = 0; i < TABLE_SIZE; ++i) {
            EventNode* current = hashTable[i];
            while (current != nullptr) {
                totalEvents++;
                
                int currentAttendees = 0;
                AttendeeNode* attCurrent = current->attendeeHead;
                while (attCurrent != nullptr) {
                    totalReg++;
                    currentAttendees++;
                    attCurrent = attCurrent->next;
                }

                if (currentAttendees > maxAttendees) {
                    maxAttendees = currentAttendees;
                    popularEvent = current;
                }
                current = current->next;
            }
        }

        cout << "Total Events in System:   " << totalEvents << "\n";
        cout << "Total Registrations:      " << totalReg << "\n";
        
        if (popularEvent != nullptr) {
            cout << "Most Popular Event:       " << popularEvent->eventName 
                 << " (" << maxAttendees << " attendees)\n";
        } else {
            cout << "Most Popular Event:       N/A (No events)\n";
        }
        cout << "----------------------------------------\n";
    }
};

// --- UTILITY AND MAIN FUNCTIONS ---

void printHeader() {
    cout << R"(
=========================================================

          PICT Event Tracker & Management System

=========================================================
)";
}

// --- 4. Main Menu updated to be shorter ---
void printMenu() {
    cout << "\nMain Menu:\n";
    cout << "  1. Add New Event\n";
    cout << "  2. Register for Event\n";
    cout << "  3. Search for Event (by ID or Name)\n"; 
    cout << "  4. View All Events\n"; 
    cout << "  5. Manage/Remove Event\n";
    cout << "  6. Show System Statistics\n";
    cout << "  0. Save and Exit\n";
    cout << "Enter your choice: ";
}

/**
 * Clears the console screen (cross-platform)
 */
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/**
 * Main function to run the program
 */
int main() {
    EventTracker tracker(10);
    int choice;

    while (true) {
        clearScreen();
        printHeader();
        printMenu();
        
        while (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer

        // --- 5. Main `switch` statement updated ---
        switch (choice) {
            case 1:
                tracker.addEvent();
                break;
            case 2:
                tracker.registerForEvent();
                break;
            case 3:
                tracker.searchForEvent(); // <-- MERGED
                break;
            case 4:
                tracker.viewAllEvents(); // <-- MERGED
                break;
            case 5:
                tracker.manageEvent(); // <-- MERGED
                break;
            case 6:
                tracker.showStatistics();
                break;
            case 0:
                cout << "Saving events to file...\n";
                tracker.saveEventsToFile();
                cout << "Exiting system. Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }

        cout << "\nPress Enter to continue...";
        cin.get(); // Wait for user to press Enter
    }

    return 0;
}