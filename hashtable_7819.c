//Jonathan Vidal MP 7819: hashtable_7819.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define WORD_SIZE 8

struct Lexicon {
    int m;  // slot size of the hash table
    int* t;  // hash table
    char* a;  // storage (NULL separated string)
} lexicon;

typedef char* Word;
bool hashTableCreated = false;
int idx = 0;

void hashCreate_7819(struct Lexicon *l, int m);  // 14
bool hashEmpty_7819(struct Lexicon *l);
bool hashFull_7819(struct Lexicon *l);
void hashPrint_7819(struct Lexicon* l);  // 13
void hashInsert_7819(struct Lexicon* l, char* w);  // 10
void hashDelete_7819(struct Lexicon* l, char* w);  // 11
void hashSearch_7819(struct Lexicon* l, char* w);  // 12

void hashBatch_7819(struct Lexicon *l, char* filename);

int findPosition_7819(struct Lexicon* l, char* w);
int hashIndex_7819(const char* w, int m);
char* getKey_7819(struct Lexicon* l, int startIdx);
void replaceElement_7819(struct Lexicon* l, int startIdx);

int maxPosition_7819(struct Lexicon* l);

char* rtrim_7819(const char* s);
char* ltrim_7819(const char* s);
char* trim_7819(const char* s);

void usage_7819(const char* cmd, const char* errMsg) { //prints error message and shows how to run program
    printf("%s\n", errMsg);
    printf("Usage is: %s filename\n", cmd);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        usage_7819(argv[0], "too few arguments specified.");
        return 1;
    }
    
    if (argc > 2) {
        usage_7819(argv[0], "too many file name arguments specified.");
        return 1;
    }
    
    hashBatch_7819(&lexicon, argv[1]);
    
    return 0;
}

void hashBatch_7819(struct Lexicon *l, char* filename) {
    FILE *file = fopen(filename, "r");
    
    if (!file) {
        printf("Cannot open %s\n", filename);
        return;
    }
    
    char line[3000];
    int linenum = 0;
    while (fgets(line, sizeof(line), file)) {
        ++linenum;
        
        if (line == NULL || strlen(trim_7819(line)) == 0) {
            continue;
        }
        
        char* opt = strtok(line, " ");
        char* word = strtok(NULL, " ");
        
        int operation = atoi(opt);
        if (operation != 13 && (word == NULL || strcmp(trim_7819(word), "") == 0)) {
            printf("\nNo value is found for operation %d at line %d in the text file.\n", operation,
                   linenum);
            continue;
        }
        
        switch (operation) {
            case 10:  // insert
                hashInsert_7819(&lexicon, word);
                break;
            case 11:  // delete
                hashDelete_7819(&lexicon, word);
                break;
            case 12:  // search
                hashSearch_7819(&lexicon, word);
                break;
            case 13:  // print
                hashPrint_7819(&lexicon);
                break;
            case 14:  // create
                hashCreate_7819(&lexicon, atoi(word));
                break;
            default:
                printf(
                       "Invalid operation %d at line %d. Supported operations are 10, 11, 12, 13 and 14.\n",
                       operation, linenum);
                return;
        }
    }
    fclose(file);
}

void hashCreate_7819(struct Lexicon* l, int m) {
    if (hashTableCreated) {
        printf("Hash table is already created.\n");
        exit(0);
    }
    
    l->m = m;
    l->t = (int*) malloc(m * sizeof(int));
    l->a = (char*) malloc((WORD_SIZE * m) * sizeof(char)); // allocates memory to '\0' separated string. WORD_SIZE times the size of hash table
    int i;
    for (i = 0; i < m; i++) {
        // initializes initial index as -1 for each element of hash table
        // -1 means the hash table element is empty
        l->t[i] = -1;
    }
    
    hashTableCreated = true;
}

void hashInsert_7819(struct Lexicon* l, char* w) {
    if (!hashTableCreated) {
        printf("Hash table is not yet created.\n");
        exit(0);
    }
    
    if (hashFull_7819(l)) {
        // TODO"
        printf("\n*****HASH TABLE IS FULL*****\n");
        return;
    }
    
    w = trim_7819(w); /* removes trailing and leading spaces before inserting the word in hash table */
    int currentPos = findPosition_7819(l, w);
    
    if (l->t[currentPos] != -1) {
        // same string already exists at same location
        return;
    }
    
    // insert string
    int len = strlen(w);
    l->t[currentPos] = idx;
    int i;
    for (i = 0; i < len; i++) {
        l->a[idx++] = w[i];
    }
    l->a[idx++] = '\0';
}

int findPosition_7819(struct Lexicon* l, char* w) {
    int currentPosition = hashIndex_7819(w, l->m);
    
    // quadratic probing find position
    int offset = 0;
    while (l->t[currentPosition] != -1
           && strcmp(w, getKey_7819(l, l->t[currentPosition])) != 0) {
        offset++;
        currentPosition = (currentPosition + (offset * offset)) % l->m;
        
        if (offset == l->m) {
            return -1;
        }
    }
    
    return currentPosition;
}

int hashIndex_7819(const char* w, int m) {
    int len = strlen(w);
    
    int sum = 0;
    int i;
    for (i = 0; i < len; i++) {
        sum += w[i];
    }
    
    return (sum % m);
}

char* getKey_7819(struct Lexicon* l, int startIdx) {
    char* arr = l->a;
    
    int len = 0;
    while (*(arr + startIdx + len) != '\0') {
        len++;
    }
    
    char* str = malloc(len + 1);
    int i = 0;
    for (; i < len; i++) {
        *(str + i) = *(arr + startIdx); /* copies string from startIdx up to '\0' */
        arr++;
    }
    *(str + i) = '\0';
    
    return str;
}

void hashPrint_7819(struct Lexicon* l) {
    if (!hashTableCreated) {
        printf("Hash table is not yet created.\n");
        exit(0);
    }
    
    printf("\n    T \t\tA: ");
    
    int maxPos = maxPosition_7819(l);
    int i;
    for (i = 0; i < maxPos || l->a[i] != '\0'; i++) {
        if (l->a[i] == '\0') {
            printf("\\");
        } else {
            printf("%c", l->a[i]);
        }
    }
    if (maxPos != -1) {
        printf("\\");
    }
    printf("\n");
    int j;
    for (j = 0; j < l->m; j++) {
        if (l->t[j] == -1) {
            printf("%d: \n", j);
        } else {
            printf("%d: %d\n", j, l->t[j]);
        }
    }
    printf("\n");
}

int maxPosition_7819(struct Lexicon* l) {
    // we don't know how much its filled
    // returns max index from hash table, that helps to print array unto next '\0' from maxPosition
    int max = -1;
    int i;
    for (i = 0; i < l->m; i++) {
        if (l->t[i] > max) {
            max = l->t[i];
        }
    }
    
    return max;
}

void hashSearch_7819(struct Lexicon* l, char* w) {
    if (!hashTableCreated) {
        printf("Hash table is not yet created.\n");
        exit(0);
    }
    
    w = trim_7819(w);
    if (hashEmpty_7819(l)) {
        printf("%-10s not found\n", w);
        return;
    }
    
    int currentPos = findPosition_7819(l, w);
    if (currentPos == -1 || l->t[currentPos] == -1) {
        printf("%-10s not found\n", w);
    } else {
        printf("%-10s found at slot %d\n", w, currentPos);
    }
}

void hashDelete_7819(struct Lexicon* l, char* w) {
    if (!hashTableCreated) {
        printf("Hash table is not yet created.\n");
        exit(0);
    }
    
    w = trim_7819(w);
    if (hashEmpty_7819(l)) {
        printf("%-10s not found\n", w);
        return;
    }
    
    int currentPos = findPosition_7819(l, w);
    if (currentPos == -1 || l->t[currentPos] == -1) {
        printf("%-10s not found\n", w);
    } else {
        replaceElement_7819(l, l->t[currentPos]);
        l->t[currentPos] = -1;
        printf("%-10s deleted from slot %d\n", w, currentPos);
    }
}

void replaceElement_7819(struct Lexicon* l, int startIdx) {
    // when deleting element, replaces character '*' from startIdx up to '\0'
    while (l->a[startIdx] != '\0') {
        l->a[startIdx++] = '*';
    }
}

bool hashEmpty_7819(struct Lexicon *l) {
    if (!hashTableCreated) {
        printf("Hash table is not yet created.\n");
        exit(0);
    }
    int i;
    for (i = 0; i < l->m; i++) {
        if (l->t[i] != -1) {
            return false;
        }
    }
    
    return true;
}

bool hashFull_7819(struct Lexicon *l) {
    if (!hashTableCreated) {
        printf("Hash table is not yet created.\n");
        exit(0);
    }
    int i;
    for (i= 0; i < l->m; i++) {
        if (l->t[i] == -1) {
            return false;
        }
    }
    
    return true;
}

char* rtrim_7819(const char* s) { //removes leading/right most spaces from string
    while (isspace(*s)) {
        ++s;
    }
    
    return strdup(s);
}

char* ltrim_7819(const char* s) { //removes trailing/left most spaces from string
    char *str = strdup(s);
    if (str != NULL) {
        char* fr = str + strlen(s) - 1;
        while ((isspace(*fr) || *fr == 0) && fr >= str) {
            --fr;
        }
        *++fr = 0;
    }
    
    return str;
}

char* trim_7819(const char* s) { //removes both leading and traling spaces from string
    char *r = rtrim_7819(s);
    char *f = ltrim_7819(r);
    free(r);
    return f;
}
