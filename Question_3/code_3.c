#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_LEN 100
#define MAX_OPERATIONS 200
#define MAX_SUGGESTIONS 5

/* Structure to store one edit operation */
typedef struct {
    char type;          // I = Insert, D = Delete, S = Substitute
    char operand;       // Character involved
    int position;       // Position in word
} Operation;

/* Structure to store one suggested correction */
typedef struct {
    char word[MAX_WORD_LEN];
    int distance;                       // Edit distance from typed word
    Operation operations[MAX_OPERATIONS];
    int operationCount;
} Suggestion;

/* Return minimum of three integers */
int min3(int a, int b, int c) {
    int m = a;
    if (b < m) m = b;
    if (c < m) m = c;
    return m;
}
// TASK 1 & 2 - Build DP table and compute edit distance
/*
   Compute Damerau-Levenshtein edit distance:
   Supports insert, delete, substitute, transpose
*/
int editDistance(const char *s1, const char *s2,
                 int **dpTable, int *rows, int *cols)
{
    int m = strlen(s1);
    int n = strlen(s2);

    *rows = m + 1;
    *cols = n + 1;

    /* Allocate DP table dynamically */
    *dpTable = (int *)malloc((*rows) * (*cols) * sizeof(int));
    int *dp = *dpTable;

    /* Base case: converting word to empty string */
    for (int i = 0; i <= m; i++)
        dp[i * (*cols)] = i;

    /* Base case: converting empty string to word */
    for (int j = 0; j <= n; j++)
        dp[j] = j;

    /* Fill DP table */
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {

            /* No cost if characters match */
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;

            /* Minimum of delete, insert, substitute */
            dp[i * (*cols) + j] = min3(
                dp[(i - 1) * (*cols) + j] + 1,
                dp[i * (*cols) + (j - 1)] + 1,
                dp[(i - 1) * (*cols) + (j - 1)] + cost
            );

            /* Check transposition case */
            if (i > 1 && j > 1 &&
                s1[i - 1] == s2[j - 2] &&
                s1[i - 2] == s2[j - 1]) {

                int trans = dp[(i - 2) * (*cols) + (j - 2)] + 1;

                if (trans < dp[i * (*cols) + j])
                    dp[i * (*cols) + j] = trans;
            }
        }
    }

    /* Final cell contains answer */
    return dp[m * (*cols) + n];
}

// TASK 3 - Backtrack through DP table to reconstruct operations
/*
   Backtrack through DP table to find exact edit operations
*/
void reconstructOperations(const char *source,
                           const char *target,
                           int *dpTable,
                           int rows,
                           int cols,
                           Operation *operations,
                           int *opCount)
{
    int i = strlen(source);
    int j = strlen(target);
    *opCount = 0;

    while (i > 0 || j > 0) {

        int curr = dpTable[i * cols + j];

        /* Characters match → move diagonally */
        if (i > 0 && j > 0 &&
            source[i - 1] == target[j - 1]) {
            i--;
            j--;
        }

        /* Substitution case */
        else if (i > 0 && j > 0 &&
                 curr == dpTable[(i - 1) * cols + (j - 1)] + 1) {
            operations[*opCount].type = 'S';
            operations[*opCount].operand = target[j - 1];
            operations[*opCount].position = i;
            (*opCount)++;
            i--;
            j--;
        }

        /* Deletion case */
        else if (i > 0 &&
                 curr == dpTable[(i - 1) * cols + j] + 1) {
            operations[*opCount].type = 'D';
            operations[*opCount].operand = source[i - 1];
            operations[*opCount].position = i;
            (*opCount)++;
            i--;
        }

        /* Insertion case */
        else {
            operations[*opCount].type = 'I';
            operations[*opCount].operand = target[j - 1];
            operations[*opCount].position = i + 1;
            (*opCount)++;
            j--;
        }
    }

    /* Reverse operation list to correct order */
    for (int k = 0; k < *opCount / 2; k++) {
        Operation temp = operations[k];
        operations[k] = operations[*opCount - 1 - k];
        operations[*opCount - 1 - k] = temp;
    }
}

/* Sort suggestions in ascending order of distance */
void sortSuggestions(Suggestion *s, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (s[j].distance < s[i].distance) {
                Suggestion temp = s[i];
                s[i] = s[j];
                s[j] = temp;
            }
        }
    }
}

/*
   Compare typed word against every dictionary word
   and generate ranked correction suggestions
*/
Suggestion* findCorrections(const char *typedWord,
                            const char **dictionary,
                            int dictSize,
                            int *suggestionCount)
{
    Suggestion *suggestions =
        (Suggestion *)malloc(dictSize * sizeof(Suggestion));

    *suggestionCount = 0;

    for (int i = 0; i < dictSize; i++) {

        int *dpTable = NULL;
        int rows, cols;

        /* Compute edit distance */
        int dist = editDistance(
            typedWord,
            dictionary[i],
            &dpTable,
            &rows,
            &cols
        );

        /* Store suggestion */
        strcpy(suggestions[*suggestionCount].word, dictionary[i]);
        suggestions[*suggestionCount].distance = dist;

        /* Reconstruct operations */
        reconstructOperations(
            typedWord,
            dictionary[i],
            dpTable,
            rows,
            cols,
            suggestions[*suggestionCount].operations,
            &suggestions[*suggestionCount].operationCount
        );

        (*suggestionCount)++;
        free(dpTable);
    }

    /* Sort by best match first */
    sortSuggestions(suggestions, *suggestionCount);
    return suggestions;
}

/*
   Run one experimental test case
*/
void runExperiment(int expNo,
                   const char *typedWord,
                   const char *expected,
                   const char **dictionary,
                   int dictSize)
{
    int count;
    clock_t start = clock();

    Suggestion *sugs =
        findCorrections(typedWord, dictionary, dictSize, &count);

    clock_t end = clock();
    double timeTaken =
        (double)(end - start) * 1000 / CLOCKS_PER_SEC;

    /* Print experiment header */
    printf("\nExp %d: Testing word correction\n", expNo);
    printf("   Typed Word : %-15s | Expected : %s\n\n",
           typedWord, expected);

    printf("   %-20s %-15s %-15s\n",
           "Suggested Word", "Distance", "Time(ms)");

    /* Show top suggestions */
    for (int i = 0; i < MAX_SUGGESTIONS && i < count; i++) {
        printf("   %-20s %-15d %-15.3f\n",
               sugs[i].word,
               sugs[i].distance,
               (i == 0 ? timeTaken : 0.000));
    }

    /* Accuracy check */
    if (strcmp(sugs[0].word, expected) == 0)
        printf("   Accuracy : PASS\n");
    else
        printf("   Accuracy : FAIL\n");

    printf("   Best Match Found: %s\n", sugs[0].word);

    free(sugs);
}

/* Main function: runs all required experimental test cases */
int main() {

    /* Dictionary words used for testing */
    const char *dictionary[] = {
        "the",
        "receive",
        "spelling",
        "world",
        "apple",
        "hello",
        "accommodate",
        "occasion",
        "halo",
        "accommodation",
        "spellings",
        "word",
        "help"
    };

    int dictSize =
        sizeof(dictionary) / sizeof(dictionary[0]);

    /* Run all assignment test cases */
    runExperiment(1, "teh", "the", dictionary, dictSize);
    runExperiment(2, "recieve", "receive", dictionary, dictSize);
    runExperiment(3, "speling", "spelling", dictionary, dictSize);
    runExperiment(4, "wrld", "world", dictionary, dictSize);
    runExperiment(5, "aple", "apple", dictionary, dictSize);
    runExperiment(6, "hellllo", "hello", dictionary, dictSize);
    runExperiment(7, "accomodate", "accommodate", dictionary, dictSize);
    runExperiment(8, "ocasion", "occasion", dictionary, dictSize);
    runExperiment(9, "halo", "halo", dictionary, dictSize);
    runExperiment(10, "qwerty", "help", dictionary, dictSize);

    return 0;
}