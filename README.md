# DSA-MINI-PROZECT


# 🏛️ PICT Event Tracker & Management System

This is a simple, terminal-based event management system built in C++. It allows users to create, track, and manage college events and their attendees.

This project is a mini-project for a Data Structures and Algorithms (DSA) course. It is built from scratch using foundational concepts like hashing, linked lists, sorting, and file handling, with no external libraries.

## 1. 📋 Features (Functional Requirements)

* **Add New Event:** Create a new event with a unique Event ID, name, date (DD/MM/YYYY), and venue.
* **Register for Event:** Register a student (by ID and name) for an existing event.
* **View Event Details:** Look up an event by its ID to see all its details, including a complete list of registered attendees.
* **Search Event by Name:** Perform a partial-text search to find any event containing a specific string in its name.
* **View All Events (Sorted):** Display a list of all events currently in the system, sorted alphabetically by the event name.
* **Remove Event:** Delete an event from the system using its Event ID. This also deletes all associated attendee data.
* **Data Persistence:** The system automatically saves all events and attendee information to a file (`events.txt`) when you select "Save and Exit." This data is automatically reloaded the next time the program starts.

## 2. 🔧 Technical Design & DSA Concepts

This project uses a combination of core data structures to efficiently store and manage data.

### 2.1. Hashing (Primary Data Structure)

The main data storage is a **Hash Table** (`EventNode** hashTable`).

* **Key:** The `eventID` string is used as the key.
* **Hash Function:** A simple polynomial rolling hash function (`hashFunction`) converts the `eventID` into an array index.
* **Purpose:** This provides an average-case **O(1)** time complexity for finding, removing, and viewing events, which is the system's most common operation.

### 2.2. Linked Lists (Linear Data Structure)

Linked lists are used in two critical ways:

1.  **Collision Handling (Chaining):** If two different `eventID`s hash to the same index, the `EventNode` objects are stored in a **singly linked list** at that index. The `EventNode->next` pointer manages this chain.
2.  **Nested Attendee Lists:** Each `EventNode` object has a *separate* pointer (`attendeeHead`) to its own **singly linked list** of `AttendeeNode`s. This keeps all attendees for a specific event organized together.

### 2.3. Sorting (Insertion Sort)

To fulfill the "View All Events (Sorted)" feature, the system uses **Insertion Sort**.

* It iterates through the entire hash table (and all its chains) and copies all `EventNode` pointers into a temporary `std::vector`.
* It then runs the `insertionSort` algorithm on this vector, sorting the events by their `eventName` string.
* The sorted vector is then printed to the console.

### 2.4. Searching (Hash vs. Linear)

The project demonstrates two different search algorithms:

1.  **Hash-Based Search (O(1) avg):** Used in "View Event Details," "Register for Event," and "Remove Event." The hash function directly finds the correct index, making it extremely fast.
2.  **Linear Search (O(n)):** Used in "Search Event by Name." Since the name is not the key, the code must iterate through *every single event* in the hash table to find matches.

### 2.5. File Handling (Sequential File I/O)

To save and load data, the project uses C++ `fstream` to interact with a sequential file (`events.txt`).

* **`loadEventsFromFile()`:** Called on startup, this function reads `events.txt`, parses each line (using `|` as a delimiter), and rebuilds the *entire* in-memory hash table and all nested attendee lists.
* **`saveEventsToFile()`:** Called on exit (option `0`), this function traverses the entire hash table and writes every event and attendee to `events.txt`, overwriting the old file with the new data.

## 3. 🚀 How to Use

1.  **Compile:** Compile the `dsaMiniproz.cpp` file using a C++ compiler.
    ```bash
    g++ dsaMiniproz.cpp -o event_tracker
    ```
2.  **Run:** Execute the compiled program.
    ```bash
    ./event_tracker
    ```
3.  **Interact:** Follow the on-screen menu options.
4.  **Exit:** Always use option `0` to "Save and Exit" to ensure your data is written to the file.
