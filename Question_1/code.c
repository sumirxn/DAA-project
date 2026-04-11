#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TEXT_LEN  1000000
#define MAX_PAT_LEN   1000
#define ASCII_SIZE    256
#define MAX_MATCHES   100000


// TASK 1 - Build the LPS array
// For each position i, lps[i] stores the length of the longest prefix of pattern[0..i] that is also a suffix of pattern[0..i]
// This tells KMP how far to fall back in the pattern on a mismatch
void buildLPS(const char *pattern, int m, int *lps) {

    lps[0] = 0;
    int len = 0; // length of current matching prefix-suffix
    int i   = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            // character extends the matching prefix-suffix
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                // fall back using lps - do NOT increment i here
                // we re-examine pattern[i] against the shorter prefix
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}


// TASK 2 - KMP Search
// Finds all occurrences of pattern in text.
// i moves through the text and never goes backward.
// j tracks how many pattern characters have been matched so far.
void kmpSearch(const char *text, int n,
               const char *pattern, int m,
               int *matches, int *matchCount,
               long long *comparisons) {

    int *lps = (int *)malloc(m * sizeof(int));
    buildLPS(pattern, m, lps);

    *matchCount  = 0;
    *comparisons = 0;

    int i = 0; // text pointer
    int j = 0; // pattern pointer

    while (i < n) {
        (*comparisons)++;

        if (text[i] == pattern[j]) {
            i++;
            j++;
        }

        if (j == m) {
            // full match found - record starting position
            if (*matchCount < MAX_MATCHES)
                matches[(*matchCount)++] = i - j;
            // fall back using lps to keep searching (handles overlaps too)
            j = lps[j - 1];

        } else if (text[i] != pattern[j]) {
            // mismatch
            if (j != 0)
                j = lps[j - 1]; // we had some matches, use lps to recover
            else
                i++; // nothing matched, just move forward in text
        }
    }

    free(lps);
}


// TASK 4 - Boyer-Moore Bad Character table
// badChar[c] stores the rightmost index of character c in the pattern.
// If c is not in the pattern, it stores -1.
// This tells us how far to shift the pattern on a mismatch.
void buildBadChar(const char *pattern, int m, int *badChar) {
    for (int i = 0; i < ASCII_SIZE; i++)
        badChar[i] = -1;

    for (int i = 0; i < m; i++)
        badChar[(unsigned char)pattern[i]] = i;
}


// TASK 4 - Boyer-Moore Search
// Scans the pattern right-to-left and skips sections of the text on mismatch.
// Faster than KMP on non-repetitive DNA but degrades on repetitive sequences.
void bmSearch(const char *text, int n,
              const char *pattern, int m,
              int *matches, int *matchCount,
              long long *comparisons) {

    int badChar[ASCII_SIZE];
    buildBadChar(pattern, m, badChar);

    *matchCount  = 0;
    *comparisons = 0;

    int s = 0; // current alignment of pattern against text

    while (s <= n - m) {
        int j = m - 1; // start from rightmost character of pattern

        while (j >= 0 && pattern[j] == text[s + j]) {
            (*comparisons)++;
            j--;
        }

        if (j < 0) {
            // full match found
            if (*matchCount < MAX_MATCHES)
                matches[(*matchCount)++] = s;
            s += (s + m < n) ? (m - badChar[(unsigned char)text[s + m]]) : 1;

        } else {
            (*comparisons)++;
            // shift pattern using bad character heuristic - always at least 1
            int shift = j - badChar[(unsigned char)text[s + j]];
            s += (shift > 1) ? shift : 1;
        }
    }
}


// TASK 5 - Helper: generate a repetitive DNA sequence
// repeats the given unit count times then appends suffix
void generateRepetitiveDNA(char *buf, const char *repeat, int count, const char *suffix) {
    buf[0] = '\0';
    for (int i = 0; i < count; i++)
        strcat(buf, repeat);
    strcat(buf, suffix);
}

// TASK 5 - Helper: generate a random DNA sequence of length n
void generateRandomDNA(char *buf, int n, unsigned int seed) {
    const char bases[] = "ACGT";
    unsigned int state = seed;
    for (int i = 0; i < n; i++) {
        state  = state * 1664525u + 1013904223u;
        buf[i] = bases[(state >> 16) & 3];
    }
    buf[n] = '\0';
}

double elapsedMs(clock_t start) {
    return ((double)(clock() - start) / CLOCKS_PER_SEC) * 1000.0;
}

// TASK 5 - Run one experiment: compare KMP and Boyer-Moore on the same input
void runExperiment(const char *label,
                   const char *text, int n,
                   const char *pattern, int m,
                   int verbose) {

    int *kmpMatches = (int *)malloc(MAX_MATCHES * sizeof(int));
    int *bmMatches  = (int *)malloc(MAX_MATCHES * sizeof(int));

    int       kmpCount = 0,  bmCount = 0;
    long long kmpCmp   = 0,  bmCmp   = 0;

    clock_t t0 = clock();
    kmpSearch(text, n, pattern, m, kmpMatches, &kmpCount, &kmpCmp);
    double kmpMs = elapsedMs(t0);

    t0 = clock();
    bmSearch(text, n, pattern, m, bmMatches, &bmCount, &bmCmp);
    double bmMs = elapsedMs(t0);

    printf("\n%s\n", label);
    printf("  Text length : %d | Pattern : %.*s%s\n",
           n, m > 20 ? 20 : m, pattern, m > 20 ? "..." : "");

    printf("\n  %-20s %10s %15s %12s\n", "Algorithm", "Matches", "Comparisons", "Time(ms)");
    printf("  %-20s %10d %15lld %12.3f\n", "KMP",         kmpCount, kmpCmp, kmpMs);
    printf("  %-20s %10d %15lld %12.3f\n", "Boyer-Moore", bmCount,  bmCmp,  bmMs);

    printf("  Accuracy : %s\n", kmpCount == bmCount ? "PASS" : "FAIL");

    if (verbose && kmpCount > 0) {
        printf("  Matches at : ");
        int show = kmpCount < 5 ? kmpCount : 5;
        for (int i = 0; i < show; i++) {
            printf("%d", kmpMatches[i]);
            if (i < show - 1) printf(", ");
        }
        if (kmpCount > 5) printf(" ... (%d total)", kmpCount);
        printf("\n");
    }

    if (kmpCount == 0)
        printf("  No matches found.\n");

    free(kmpMatches);
    free(bmMatches);
}


int main(void) {

    printf("DNA Pattern Matching - KMP vs Boyer-Moore\n\n");


    // TASK 1 - Show the LPS array for a sample pattern
    printf("TASK 1: LPS Array\n");

    const char *pat1 = "ACACAG";
    int         m1   = strlen(pat1);
    int         lps1[MAX_PAT_LEN];
    buildLPS(pat1, m1, lps1);

    printf("  Pattern : ");
    for (int i = 0; i < m1; i++) printf("  %c", pat1[i]);
    printf("\n  Index   : ");
    for (int i = 0; i < m1; i++) printf("%3d", i);
    printf("\n  LPS     : ");
    for (int i = 0; i < m1; i++) printf("%3d", lps1[i]);
    printf("\n\n");
    printf("  lps[4]=3 means prefix 'ACA' == suffix 'ACA' in 'ACACA'\n");
    printf("  So on a mismatch at index 5, j falls back to 3, not 0\n");


    // TASK 2 - KMP search on a short DNA sequence
    printf("\nTASK 2: KMP Search\n");

    const char *text2 = "ACGTACACACAGTTACACAGACG";
    const char *pat2  = "ACACAG";
    int n2 = strlen(text2), m2 = strlen(pat2);

    int       matches2[MAX_MATCHES];
    int       count2 = 0;
    long long cmp2   = 0;

    kmpSearch(text2, n2, pat2, m2, matches2, &count2, &cmp2);

    printf("  Text    : %s\n", text2);
    printf("  Pattern : %s\n\n", pat2);
    for (int i = 0; i < count2; i++)
        printf("  Match %d at position %d : %.*s\n", i+1, matches2[i], m2, text2 + matches2[i]);
    printf("\n  Comparisons : %lld  (text length = %d)\n", cmp2, n2);


    // TASK 3 - Worst-case demonstration on a repetitive DNA sequence
    printf("\nTASK 3: Worst-Case on Repetitive DNA\n");

    char wcText[600], wcPat[20];
    generateRepetitiveDNA(wcText, "A", 500, "");  // 500 A's
    generateRepetitiveDNA(wcPat,  "A", 8,   "T"); // AAAAAAAAT

    int wcN = strlen(wcText), wcM = strlen(wcPat);
    int wcMatches[MAX_MATCHES];
    int wcCount = 0;
    long long wcCmp = 0;

    kmpSearch(wcText, wcN, wcPat, wcM, wcMatches, &wcCount, &wcCmp);

    printf("  Text    : %d A's (poly-A run)\n", wcN);
    printf("  Pattern : %s\n", wcPat);
    printf("  KMP comparisons  : %lld  (n + m = %d)\n", wcCmp, wcN + wcM);
    printf("  Naive would need : ~%d\n", wcN * wcM);
    printf("  Matches found    : %d  (correct - pattern not present)\n", wcCount);


    // TASK 4 and 5 - Experiments comparing KMP vs Boyer-Moore
    printf("\nTASK 4 and 5: Experiments\n");

    char *longText = (char *)malloc(MAX_TEXT_LEN + 10);
    char *longPat  = (char *)malloc(MAX_PAT_LEN  + 10);

    // Exp 1: repetitive DNA - telomere repeat
    // KMP handles this well, Boyer-Moore shifts collapse to 1
    runExperiment("Exp 1: Repetitive - Telomere (TTAGGG repeat)",
                  "TTAGGGTTAGGGTTAGGGTTAGGGTTAGGG", 30, "TTAGGG", 6, 1);

    // Exp 2: non-repetitive DNA - coding sequence
    // Boyer-Moore gets larger skips here
    runExperiment("Exp 2: Non-repetitive - Coding sequence",
                  "ATGGCTAGCATCGATCGATCGATCGATCGAT", 31, "ATCGATCG", 8, 1);

    // Exp 3: long repetitive DNA - worst case for Boyer-Moore
    generateRepetitiveDNA(longText, "AAAA", 25000, ""); // 100,000 A's
    strcpy(longPat, "AAAAAAAAAAC");
    runExperiment("Exp 3: Long repetitive - 100k poly-A",
                  longText, strlen(longText), longPat, strlen(longPat), 0);

    // Exp 4: long non-repetitive DNA - typical coding region
    // Boyer-Moore should skip more and use fewer comparisons
    generateRandomDNA(longText, 100000, 42);
    strcpy(longPat, "ACGTTGCA");
    runExperiment("Exp 4: Long non-repetitive - 100k random, 8bp pattern",
                  longText, strlen(longText), longPat, strlen(longPat), 0);

    // Exp 5: longer pattern on non-repetitive DNA
    // more characters means bigger skips for Boyer-Moore
    generateRandomDNA(longText, 100000, 99);
    strcpy(longPat, "ACGTCGATGCTAGCTA");
    runExperiment("Exp 5: Long non-repetitive - 100k random, 16bp pattern",
                  longText, strlen(longText), longPat, strlen(longPat), 0);

    // Exp 6: pattern not in text - accuracy check
    generateRepetitiveDNA(longText, "ACGT", 25000, "");
    strcpy(longPat, "TTTTCCCCC");
    runExperiment("Exp 6: Pattern absent - expect 0 matches",
                  longText, strlen(longText), longPat, strlen(longPat), 0);

    // Exp 7: single character pattern - edge case
    runExperiment("Exp 7: Single character pattern 'T'",
                  "AACGTTACGGCATTTACGTA", 20, "T", 1, 1);

    // Exp 8: pattern equals full text - edge case
    runExperiment("Exp 8: Pattern equals full text",
                  "ACGTACGT", 8, "ACGTACGT", 8, 1);

    free(longText);
    free(longPat);
    return 0;
}