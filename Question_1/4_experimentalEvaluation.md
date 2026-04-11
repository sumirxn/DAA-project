**Experimental Evaluation**

**Task 5 : Analysis of results**

- We have 8 experiments covering a range of DNA sequence types and edge cases
- Both KMP and Boyer-Moore Algorithms are given the same input

| Experiment | Sequence Type        | Text Length | Pattern Length | Purpose                         |
|------------|----------------------|-------------|----------------|---------------------------------|
| Exp 1      | Repetitive (TTAGGG)  | 30          | 6              | Telomere repeat                 |
| Exp 2      | Non-repetitive       | 31          | 8              | Coding sequence                 |
| Exp 3      | Repetitive (poly-A)  | 100,000     | 11             | Worst case — homopolymer run    |
| Exp 4      | Random DNA           | 100,000     | 8              | Typical coding region           |
| Exp 5      | Random DNA           | 100,000     | 16             | Longer pattern                  |
| Exp 6      | Repetitive (ACGT)    | 100,000     | 9              | Pattern absent : accuracy check |
| Exp 7      | Short sequence       | 20          | 1              | Edge case :  single character   |
| Exp 8      | Short sequence       | 8           | 8              | Edge case : pattern equals text |

**Results :**
```
TASK 1: LPS Array
  Pattern :   A  C  A  C  A  G
  Index   :   0  1  2  3  4  5
  LPS     :   0  0  1  2  3  0

  lps[4]=3 means prefix 'ACA' == suffix 'ACA' in 'ACACA'
  So on a mismatch at index 5, j falls back to 3, not 0

TASK 2: KMP Search
  Text    : ACGTACACACAGTTACACAGACG
  Pattern : ACACAG
  Match 1 at position 6  : ACACAG
  Match 2 at position 14 : ACACAG
  Comparisons : 23  (text length = 23)

TASK 3: Worst-Case on Repetitive DNA
  Text    : 500 A's (poly-A run)
  Pattern : AAAAAAAAT
  KMP comparisons  : 500  (n + m = 509)
  Naive would need : ~4500
  Matches found    : 0  (correct - pattern not present)

Exp 1: Repetitive - Telomere (TTAGGG repeat)
  Text length : 30 | Pattern : TTAGGG
  Algorithm            Matches     Comparisons     Time(ms)
  KMP                        5              30        0.002
  Boyer-Moore                5              42        0.001
  Accuracy : PASS | Matches at : 0, 6, 12, 18, 24

Exp 2: Non-repetitive - Coding sequence
  Text length : 31 | Pattern : ATCGATCG
  Algorithm            Matches     Comparisons     Time(ms)
  KMP                        4              31        0.001
  Boyer-Moore                4              41        0.001
  Accuracy : PASS | Matches at : 9, 13, 17, 21

Exp 3: Long repetitive - 100k poly-A
  Text length : 100000 | Pattern : AAAAAAAAAAC
  Algorithm            Matches     Comparisons     Time(ms)
  KMP                        0          100000        0.431
  Boyer-Moore                0           99990        1.033
  Accuracy : PASS | No matches found.

Exp 4: Long non-repetitive - 100k random, 8bp pattern
  Text length : 100000 | Pattern : ACGTTGCA
  Algorithm            Matches     Comparisons     Time(ms)
  KMP                        1          100001        0.759
  Boyer-Moore                1           77328        0.942
  Accuracy : PASS

Exp 5: Long non-repetitive - 100k random, 16bp pattern
  Text length : 100000 | Pattern : ACGTCGATGCTAGCTA
  Algorithm            Matches     Comparisons     Time(ms)
  KMP                        0          100001        1.072
  Boyer-Moore                0           77486        0.899
  Accuracy : PASS | No matches found.

Exp 6: Pattern absent - expect 0 matches
  Text length : 100000 | Pattern : TTTTCCCCC
  Algorithm            Matches     Comparisons     Time(ms)
  KMP                        0          100000        0.130
  Boyer-Moore                0           24997        0.097
  Accuracy : PASS | No matches found.

Exp 7: Single character pattern 'T'
  Text length : 20 | Pattern : T
  Algorithm            Matches     Comparisons     Time(ms)
  KMP                        6              20        0.003
  Boyer-Moore                6              17        0.001
  Accuracy : PASS | Matches at : 4, 5, 12, 13, 14 (6 total)

Exp 8: Pattern equals full text
  Text length : 8 | Pattern : ACGTACGT
  Algorithm            Matches     Comparisons     Time(ms)
  KMP                        1               8        0.001
  Boyer-Moore                1               8        0.001
  Accuracy : PASS | Matches at : 0
```

**Analysis :**

Tasks 1 and 2 : LPS and KMP on short DNA
- The LPS array for "ACACAG" correctly identifies the prefix-suffix relationships within the pattern
- In the search demo, KMP found both matches using exactly 23 comparisons on a text of length 23,   confirming that i visited every character exactly once, as expected from O(n)

Task 3 : Worst case on repetitive DNA
- On a 500-character poly-A sequence with pattern "AAAAAAAAT", KMP used exactly 500 comparisons , effectively n, which is the closest to the theoretical minimum possible
-  This demonstrates KMP's linear guarantee on the exact type of input (homopolymer runs) that is common in real genomic data

Experiment 1 : Repetitive DNA (Telomere)
- On the TTAGGG telomere repeat, KMP used 30 comparisons (equal to the text length) while Boyer-Moore used 42
- This is because every character in the text is part of a repeating pattern, so Boyer-Moore's bad character shifts collapse, it cannot skip anything and ends up doing more work than KMP due to its right-to-left scan overhead

Experiment 2 : Non-repetitive DNA (Coding sequence)
- On a short coding sequence, both algorithms used similar comparisons(31 vs 41)
- The sequence is too short for Boyer-Moore's skip advantage to show meaningfully

Experiment 3 :  Long repetitive DNA (100k poly-A)
- KMP used 100,000 comparisons and completed in 0.431ms
- Boyer-Moore used nearly the same number of comparisons (99,990) but took 1.033ms, more than twice as long
- The bad character shifts on an all-A sequence are nearly always 1, so Boyer-Moore provides no skip benefit and the right-to-left scan adds overhead instead

Experiment 4 and 5 : Long non-repetitive DNA
- On 100,000 characters of random DNA, Boyer-Moore used noticeably fewer comparisons than KMP 
  - 77,328 vs 100,001 for the 8bp pattern
  - 77,486 vs 100,001 for the 16bp pattern
- This shows Boyer-Moore's skip advantage on diverse sequences
- However, despite fewer comparisons, Boyer-Moore was slightly slower in wall-clock time on the 8bp pattern (0.942ms vs 0.759ms), likely due to the overhead of right-to-left scanning per window.
- On the 16bp pattern (Exp 5), Boyer-Moore was faster (0.899ms vs 1.072ms), the longer pattern gives larger skips that outweigh the overhead

Experiment 6 :  Pattern absent
- When the pattern is not present in the text, Boyer-Moore used only 24,997 comparisons vs KMP's 100,000
- This is the demonstration of Boyer-Moore's skip efficiency
- On an ACGT repeat with a pattern that doesn't match, it skips large sections of the text entirely. KMP must still visit every character at least once.

Experiment 7 and 8 : Edge Cases
- Both algorithms handled single-character patterns and the case where the pattern equals the full text correctly. Results were identical, confirming robustness on boundary inputs.


