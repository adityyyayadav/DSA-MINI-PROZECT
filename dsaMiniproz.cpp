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
    string date; // Format: DD/MM/YYYY
    string venue;
    AttendeeNode* attendeeHead;
    EventNode* next; // Pointer for hash table chaining

    EventNode(string id, string name, string dt, string v)
        : eventID(id), eventName(name), date(dt), venue(v), attendeeHead(nullptr), next(nullptr) {}
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
     * [SORTING - Insertion Sort]
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
     * Utility function to clear the input buffer safely.
     */
    void clearInputBuffer() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
                // We've finished reading one event's data
                currentEvent = nullptr;
                continue;
            }

            stringstream ss(line);
            string field1, field2, field3, field4;

            // Parse the line. A pipe '|' is our delimiter.
            getline(ss, field1, '|');
            getline(ss, field2, '|');
            getline(ss, field3, '|');
            getline(ss, field4, '|');

            if (!field4.empty()) {
                // This line has 4 fields, so it's an Event line
                // 1. Create and hash the event
                currentEvent = new EventNode(field1, field2, field3, field4);
                int index = hashFunction(currentEvent->eventID);
                
                // 2. Insert into hash table (chaining)
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
        // Open file in output mode (truncates/overwrites by default)
        ofstream outFile(FILENAME);
        if (!outFile) {
            cerr << "Error: Could not open " << FILENAME << " for writing." << endl;
            return;
        }

        for (int i = 0; i < TABLE_SIZE; ++i) {
            EventNode* event = hashTable[i];
            while (event != nullptr) {
                outFile << event->eventID << "|" << event->eventName << "|"
                        << event->date << "|" << event->venue << "\n";
                
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

        EventNode* newNode = new EventNode(id, name, date, venue);

        int index = hashFunction(id);
        
        // [Linear Data Structure - Chaining]
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

        // [Searching - Hash-Based]
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
        // Note: Data will be saved to file on exit.
    }

    // *** 1. NEW FUNCTION ADDED HERE ***
    /**
     * [HASHING + LINKED LIST]
     * Finds and removes an event from the hash table.
     */
    void removeEvent() {
        string eventID;
        cout << "\n--- Remove Event ---\n";
        cout << "Enter Event ID to remove: ";
        cin >> eventID;
        clearInputBuffer();

        int index = hashFunction(eventID);
        EventNode* current = hashTable[index];
        EventNode* prev = nullptr;

        // Traverse the linked list (chain) to find the event
        while (current != nullptr && current->eventID != eventID) {
            prev = current;
            current = current->next;
        }

        // Case 1: Event not found
        if (current == nullptr) {
            cout << "Error: Event '" << eventID << "' not found.\n";
            return;
        }

        // Case 2: Event is found. Unlink it from the list.
        if (prev == nullptr) {
            // The event is the first node in the chain
            hashTable[index] = current->next;
        } else {
            // The event is in the middle or at the end
            prev->next = current->next;
        }

        // Now, we must free all allocated memory for this event
        
        // 1. Delete its attendee list
        AttendeeNode* attCurrent = current->attendeeHead;
        while (attCurrent != nullptr) {
            AttendeeNode* attToDelete = attCurrent;
            attCurrent = attCurrent->next;
            delete attToDelete;
        }

        // Get the name for the success message *before* deleting
        string removedEventName = current->eventName;

        // 2. Delete the event node itself
        delete current;

        cout << "\nSuccess! Event '" << removedEventName << "' has been removed.\n";
        // Note: The removal will be saved to the file on exit.
    }

    /**
     * [SEARCHING - Hash-Based]
     * Finds and displays all details for a single event,
     */
    void viewEventDetails() {
        string eventID;
        cout << "\n--- View Event Details ---\n";
        cout << "Enter Event ID to view: ";
        cin >> eventID;
        clearInputBuffer();

        EventNode* event = findEvent(eventID);

        if (event == nullptr) {
            cout << "Error: Event '" << eventID << "' not found.\n";
            return;
        }

        // Print Event Details
        cout << "\n----------------------------------------\n";
        cout << "Event ID:    " << event->eventID << "\n";
        cout << "Name:        " << event->eventName << "\n";
        cout << "Date:        " << event->date << "\n";
        cout << "Venue:       " << event->venue << "\n";
        cout << "----------------------------------------\n";

        // [Linear Data Structure - Traversal]
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
    }

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

        // 2. [DSA: Sorting]
        // Sort the vector using Insertion Sort
        insertionSort(allEvents);

        // 3. Print the sorted list
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
};

// --- UTILITY AND MAIN FUNCTIONS ---

void printHeader() {
    cout << R"(
=========================================================

          PICT Event Tracker & Management System

=========================================================
)";
}

// *** 2. MENU UPDATED HERE ***
void printMenu() {
    cout << "\nMain Menu:\n";
    cout << "  1. Add New Event\n";
    cout << "  2. Register for Event\n";
    cout << "  3. View Event Details (and Attendee List)\n";
    cout << "  4. Search Event by Name\n";
    cout << "  5. View All Events (Sorted by Name)\n";
    cout << "  6. Remove Event\n";
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
    // Initialize with a hash table of size 10
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

        // *** 3. SWITCH/CASE UPDATED HERE ***
        switch (choice) {
            case 1:
                tracker.addEvent();
                break;
            case 2:
                tracker.registerForEvent();
                break;
            case 3:
                tracker.viewEventDetails();
                break;
            case 4:
                tracker.searchEventByName();
                break;
            case 5:
                tracker.viewEventsSortedByName();
                break;
            case 6:
                tracker.removeEvent();
                break;
            case 0:
                cout << "Saving events to file...\n";
                // [File Handling] Save all data before exiting
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