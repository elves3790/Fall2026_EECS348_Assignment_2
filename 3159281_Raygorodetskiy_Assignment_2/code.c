#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 512
#define MAX_STR_LEN 256
#define INITIAL_CAPACITY 16

typedef struct {
    char sender[MAX_STR_LEN];
    char subject[MAX_STR_LEN];
    char date[MAX_STR_LEN];
    int sender_priority; 
    long date_val;       // Formatted as YYYYMMDD for direct chronological comparison
} Email;

typedef struct {
    Email *arr;
    int size;
    int capacity;
} MaxHeap;

// Helper to assign numeric weight to categories: Higher number = Higher priority
int get_category_priority(const char *category) {
    if (strcmp(category, "Boss") == 0) return 5;
    if (strcmp(category, "Subordinate") == 0) return 4;
    if (strcmp(category, "Peer") == 0) return 3;
    if (strcmp(category, "ImportantPerson") == 0) return 2;
    if (strcmp(category, "OtherPerson") == 0) return 1;
    return 0;
}

// Convert "MM-DD-YYYY" into integer YYYYMMDD for straightforward numeric comparison
long parse_date(const char *date_str) {
    int m, d, y;
    if (sscanf(date_str, "%d-%d-%d", &m, &d, &y) == 3) {
        return (long)y * 10000 + (long)m * 100 + (long)d;
    }
    return 0;
}

// Returns > 0 if a has higher priority than b, < 0 if b is higher, 0 if equal
int compare_emails(const Email *a, const Email *b) {
    if (a->sender_priority != b->sender_priority) {
        return a->sender_priority - b->sender_priority;
    }
    // Tie-breaker: newest date takes precedence
    if (a->date_val > b->date_val) return 1;
    if (a->date_val < b->date_val) return -1;
    return 0;
}

// Heap Operations
MaxHeap* create_heap() {
    MaxHeap *h = (MaxHeap *)malloc(sizeof(MaxHeap));
    h->size = 0;
    h->capacity = INITIAL_CAPACITY;
    h->arr = (Email *)malloc(sizeof(Email) * h->capacity);
    return h;
}

void swap(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_up(MaxHeap *h, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (compare_emails(&h->arr[idx], &h->arr[parent]) > 0) {
            swap(&h->arr[idx], &h->arr[parent]);
            idx = parent;
        } else {
            break;
        }
    }
}

void heapify_down(MaxHeap *h, int idx) {
    while (1) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int largest = idx;

        if (left < h->size && compare_emails(&h->arr[left], &h->arr[largest]) > 0) {
            largest = left;
        }
        if (right < h->size && compare_emails(&h->arr[right], &h->arr[largest]) > 0) {
            largest = right;
        }

        if (largest != idx) {
            swap(&h->arr[idx], &h->arr[largest]);
            idx = largest;
        } else {
            break;
        }
    }
}

void insert_email(MaxHeap *h, Email e) {
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->arr = (Email *)realloc(h->arr, sizeof(Email) * h->capacity);
    }
    h->arr[h->size] = e;
    heapify_up(h, h->size);
    h->size++;
}

Email* peek_email(MaxHeap *h) {
    if (h->size == 0) return NULL;
    return &h->arr[0];
}

void pop_email(MaxHeap *h) {
    if (h->size == 0) return;
    h->arr[0] = h->arr[h->size - 1];
    h->size--;
    if (h->size > 0) {
        heapify_down(h, 0);
    }
}

void free_heap(MaxHeap *h) {
    if (h) {
        free(h->arr);
        free(h);
    }
}

// String helper to trim leading/trailing whitespaces and newlines
void trim(char *str) {
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r' || str[start] == '\n') {
        start++;
    }
    int end = strlen(str) - 1;
    while (end >= start && (str[end] == ' ' || str[end] == '\t' || str[end] == '\r' || str[end] == '\n')) {
        end--;
    }
    int len = end - start + 1;
    if (len > 0) {
        memmove(str, str + start, len);
        str[len] = '\0';
    } else {
        str[0] = '\0';
    }
}

int main(int argc, char *argv[]) {
    FILE *file = stdin;
    if (argc > 1) {
        file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
            return 1;
        }
    }

    MaxHeap *heap = create_heap();
    char line[MAX_LINE_LEN];

    while (fgets(line, sizeof(line), file)) {
        trim(line);
        if (strlen(line) == 0) continue;

        if (strncmp(line, "EMAIL ", 6) == 0) {
            char *content = line + 6;
            char *sender = strtok(content, ",");
            char *subject = strtok(NULL, ",");
            char *date = strtok(NULL, ",");

            if (sender && subject && date) {
                trim(sender);
                trim(subject);
                trim(date);

                Email e;
                strncpy(e.sender, sender, MAX_STR_LEN - 1);
                e.sender[MAX_STR_LEN - 1] = '\0';
                strncpy(e.subject, subject, MAX_STR_LEN - 1);
                e.subject[MAX_STR_LEN - 1] = '\0';
                strncpy(e.date, date, MAX_STR_LEN - 1);
                e.date[MAX_STR_LEN - 1] = '\0';

                e.sender_priority = get_category_priority(e.sender);
                e.date_val = parse_date(e.date);

                insert_email(heap, e);
            }
        } else if (strcmp(line, "NEXT") == 0) {
            Email *next_email = peek_email(heap);
            if (next_email != NULL) {
                printf("Next email:\n");
                printf("Sender: %s\n", next_email->sender);
                printf("Subject: %s\n", next_email->subject);
                printf("Date: %s\n", next_email->date);
            }
        } else if (strcmp(line, "READ") == 0) {
            pop_email(heap);
        } else if (strcmp(line, "COUNT") == 0) {
            printf("There are %d emails to read.\n", heap->size);
        }
    }

    if (file != stdin) {
        fclose(file);
    }
    free_heap(heap);
    return 0;
}