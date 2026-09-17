// Name: Anthony Raygorodetskiy
// Ku id: 3159281
// Description: C program that takes in emails from and input file and orders them
// based on who its sent from (prioritizing the boss or subordinates), it then prints out what 
// emails were open and how many total emails were sent to you
//Collaborators: OpenAI, Gemini
//Creation Date: 9/15/2026
//Recent edit date: 9/17/2026
//Revisions: Primarily focused on maintainability by creating a more structured process for
// managing emails as well as for many specific commands. 

#include <stdio.h>      // CHANGE: Provides input/output functions such as printf() and fgets().
#include <stdlib.h>     // Provides general-purpose functions.
#include <string.h>     // Provides string functions such as strcmp(), strchr(), and strncpy().

#define MAX_EMAILS 1000 // CHANGE: Creates a constant for the maximum number of emails.

// CHANGE: Added an enum so the priority values have meaningful names
// instead of using unexplained numbers such as 1, 2, 3, 4, and 5.
typedef enum {
    OTHER_PRIORITY = 1,             // Gives OtherPerson the lowest priority.
    IMPORTANT_PERSON_PRIORITY = 2,  // Gives ImportantPerson priority 2.
    PEER_PRIORITY = 3,              // Gives Peer priority 3.
    SUBORDINATE_PRIORITY = 4,      // Gives Subordinate priority 4.
    BOSS_PRIORITY = 5               // Gives Boss the highest priority.
} Priority;                         // Names this group of values "Priority".

// Creates a structure to store all information about one email.
typedef struct {
    char sender[30];        // Stores the sender category, such as "Boss".
    char subject[200];      // Stores the subject of the email.
    char date[11];          // Stores the date in MM-DD-YYYY format.
    
    Priority priority;     // Stores the sender's priority level.
    int dateValue;         // Stores the date as a number for easy comparison.
} Email;                    // Names this structure "Email".

// Creates a structure for the MaxHeap.
typedef struct {
    Email emails[MAX_EMAILS]; // Stores all emails in an array.
    int size;                  // Keeps track of how many emails are currently in the heap.
} MaxHeap;                     // Names this structure "MaxHeap".

// Determines the priority of an email based on its sender.
Priority getPriority(char sender[]) {

    // Checks whether the sender is Boss.
    if (strcmp(sender, "Boss") == 0)
        return BOSS_PRIORITY; // Boss gets the highest priority.

    // Checks whether the sender is Subordinate.
    else if (strcmp(sender, "Subordinate") == 0)
        return SUBORDINATE_PRIORITY; // Subordinate gets priority 4.

    // Checks whether the sender is Peer.
    else if (strcmp(sender, "Peer") == 0)
        return PEER_PRIORITY; // Peer gets priority 3.

    // Checks whether the sender is ImportantPerson.
    else if (strcmp(sender, "ImportantPerson") == 0)
        return IMPORTANT_PERSON_PRIORITY; // ImportantPerson gets priority 2.

    // If none of the previous categories match, it is OtherPerson.
    else
        return OTHER_PRIORITY; // OtherPerson gets the lowest priority.
}

// Converts the date from MM-DD-YYYY into a number.
// This makes it easier to determine which date is newer.
int getDateValue(char date[]) {

    int month; // Stores the month from the date.
    int day;   // Stores the day from the date.
    int year;  // Stores the year from the date.

    // Reads the month, day, and year from the date string.
    sscanf(date, "%d-%d-%d", &month, &day, &year);

    // Converts the date into YYYYMMDD format as an integer.
    // For example, 01-03-2025 becomes 20250103.
    return year * 10000 + month * 100 + day;
}

// Determines whether email1 has a higher priority than email2.
int higherPriority(Email email1, Email email2) {

    // If email1 has a higher sender priority, it comes first.
    if (email1.priority > email2.priority)
        return 1; // Returns true.

    // If email1 has a lower sender priority, it does not come first.
    if (email1.priority < email2.priority)
        return 0; // Returns false.

    // If both emails have the same sender priority,
    // compare their dates.
    if (email1.dateValue > email2.dateValue)
        return 1; // The newer email comes first.

    // Otherwise, email2 has equal or higher priority.
    return 0; // Returns false.
}

// Swaps two Email structures.
void swap(Email *a, Email *b) {

    Email temp = *a; // Temporarily stores the first email.

    *a = *b;         // Copies the second email into the first position.

    *b = temp;       // Copies the original first email into the second position.
}

// Initializes the heap by setting its size to zero.
void initializeHeap(MaxHeap *heap) {

    heap->size = 0; // The heap starts with no emails.
}

// Adds a new email to the MaxHeap.
void insert(MaxHeap *heap, Email email) {

    // CHANGE: Checks whether the heap has reached its maximum capacity.
    if (heap->size >= MAX_EMAILS) {
        printf("Inbox is full.\n"); // Displays an error if there is no room.
        return;                     // Stops the function.
    }

    // Places the new email at the next available position.
    int index = heap->size;

    // Stores the email in the array.
    heap->emails[index] = email;

    // Increases the number of emails in the heap.
    heap->size++;

    // Continues moving the email upward until the heap property is restored.
    while (index > 0) {

        // Calculates the index of the new email's parent.
        int parent = (index - 1) / 2;

        // Checks whether the new email has higher priority than its parent.
        if (higherPriority(heap->emails[index],
                           heap->emails[parent])) {

            // Swaps the email with its parent.
            swap(&heap->emails[index],
                 &heap->emails[parent]);

            // Moves the index to the parent's position.
            index = parent;
        }

        // If the parent already has higher priority, the heap is correct.
        else {
            break; // Stops the while loop.
        }
    }
}

// Returns a pointer to the highest-priority email without removing it.
Email *peek(MaxHeap *heap) {

    // Checks whether the heap is empty.
    if (heap->size == 0) {
        return NULL; // Returns NULL because there is no email.
    }

    // The highest-priority email is always at index 0.
    return &heap->emails[0];
}

// Removes the highest-priority email from the heap.
void removeMax(MaxHeap *heap) {

    // Checks whether the heap is empty.
    if (heap->size == 0) {
        return; // Nothing can be removed.
    }

    // Moves the last email to the root position.
    heap->emails[0] = heap->emails[heap->size - 1];

    // Decreases the number of emails in the heap.
    heap->size--;

    // Starts the downward heap adjustment at the root.
    int index = 0;

    // Continues until the heap property has been restored.
    while (1) {

        // Calculates the index of the left child.
        int left = 2 * index + 1;

        // Calculates the index of the right child.
        int right = 2 * index + 2;

        // Assumes the current email is the highest-priority email.
        int largest = index;

        // Checks whether the left child has higher priority.
        if (left < heap->size &&
            higherPriority(heap->emails[left],
                           heap->emails[largest])) {

            // Makes the left child the highest-priority candidate.
            largest = left;
        }

        // Checks whether the right child has higher priority.
        if (right < heap->size &&
            higherPriority(heap->emails[right],
                           heap->emails[largest])) {

            // Makes the right child the highest-priority candidate.
            largest = right;
        }

        // If the current email is still the highest priority,
        // the heap property is correct.
        if (largest == index) {
            break; // Stops the while loop.
        }

        // Swaps the current email with the higher-priority child.
        swap(&heap->emails[index],
             &heap->emails[largest]);

        // Moves downward to the child's position.
        index = largest;
    }
}

// Creates and returns an Email structure.
Email createEmail(char sender[], char subject[], char date[]) {

    // Creates a local Email variable.
    Email email;

    // Copies the sender into the email structure.
    strcpy(email.sender, sender);

    // Copies the subject into the email structure.
    strcpy(email.subject, subject);

    // Copies the date into the email structure.
    strcpy(email.date, date);

    // Determines the sender's priority.
    email.priority = getPriority(sender);

    // Converts the date into a number for comparison.
    email.dateValue = getDateValue(date);

    // Returns the completed Email structure.
    return email;
}

// Displays the highest-priority email without removing it.
void nextEmail(MaxHeap *heap) {

    // Gets the highest-priority email.
    Email *email = peek(heap);

    // Checks whether there are no emails.
    if (email == NULL) {
        return; // Does nothing if the heap is empty.
    }

    // Displays the required "Next email:" message.
    printf("Next email:\n");

    // Displays the sender.
    printf("Sender: %s\n", email->sender);

    // Displays the subject.
    printf("Subject: %s\n", email->subject);

    // Displays the date.
    printf("Date: %s\n", email->date);
}

// Marks the highest-priority email as read by removing it.
void readEmail(MaxHeap *heap) {

    // Removes the highest-priority email.
    removeMax(heap);
}

// Displays the number of unread emails.
void countEmails(MaxHeap *heap) {

    // Prints the current number of emails in the heap.
    printf("There are %d emails to read.\n", heap->size);
}

// CHANGE: Added a separate function to process commands.
// This makes main() shorter and improves maintainability.
void processCommand(char line[], MaxHeap *heap) {

    // Checks whether the line starts with "EMAIL ".
    if (strncmp(line, "EMAIL ", 6) == 0) {

        // Creates a character array for the sender.
        char sender[30];

        // Creates a character array for the subject.
        char subject[200];

        // Creates a character array for the date.
        char date[11];

        // Skips "EMAIL " and points to the actual email information.
        char *data = line + 6;

        // Searches for the first comma.
        char *firstComma = strchr(data, ',');

        // Checks whether the first comma was not found.
        if (firstComma == NULL) {
            return; // Stops processing the invalid line.
        }

        // Replaces the first comma with '\0'.
        // This separates the sender from the rest of the line.
        *firstComma = '\0';

        // CHANGE: Uses strncpy instead of strcpy when copying the sender.
        // This helps prevent copying more characters than the array can hold.
        strncpy(sender, data, sizeof(sender) - 1);

        // Makes sure the sender string ends with a null character.
        sender[sizeof(sender) - 1] = '\0';

        // Searches for the second comma.
        char *secondComma = strchr(firstComma + 1, ',');

        // Checks whether the second comma was not found.
        if (secondComma == NULL) {
            return; // Stops processing the invalid line.
        }

        // Replaces the second comma with '\0'.
        // This separates the subject from the date.
        *secondComma = '\0';

        // CHANGE: Uses strncpy to safely copy the subject.
        strncpy(subject,
                firstComma + 1,
                sizeof(subject) - 1);

        // Makes sure the subject string ends with '\0'.
        subject[sizeof(subject) - 1] = '\0';

        // CHANGE: Uses strncpy to safely copy the date.
        strncpy(date,
                secondComma + 1,
                sizeof(date) - 1);

        // Makes sure the date string ends with '\0'.
        date[sizeof(date) - 1] = '\0';

        // Creates an Email structure using the parsed information.
        Email email = createEmail(sender, subject, date);

        // Adds the email to the MaxHeap.
        insert(heap, email);
    }

    // Checks whether the command is "NEXT".
    else if (strcmp(line, "NEXT") == 0) {

        // Displays the highest-priority email without removing it.
        nextEmail(heap);
    }

    // Checks whether the command is "READ".
    else if (strcmp(line, "READ") == 0) {

        // Removes the highest-priority email.
        readEmail(heap);
    }

    // Checks whether the command is "COUNT".
    else if (strcmp(line, "COUNT") == 0) {

        // Displays the number of unread emails.
        countEmails(heap);
    }
}

// The main function is where the program begins execution.
int main(void) {

    // Creates a MaxHeap variable named heap.
    MaxHeap heap;

    // Initializes the heap so that it starts empty.
    initializeHeap(&heap);

    // Creates a character array to store each input line.
    char line[300];

    // Reads one line at a time from standard input.
    // This can receive input from a .txt file through input redirection.
    while (fgets(line, sizeof(line), stdin) != NULL) {

        // Removes the newline character from the end of the line.
        line[strcspn(line, "\n")] = '\0';

        // Sends the line to the command-processing function.
        processCommand(line, &heap);
    }

    // Indicates that the program finished successfully.
    return 0;
}
