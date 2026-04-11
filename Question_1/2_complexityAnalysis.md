**Time Complexity Analysis**

KMP Time complexity

The KMP algorithm involves two phases, creating the LPS array and searching for the text

1. Building the LPS Array
   - The pointer i only moves forward through the patter, so it increments at most m times
   - The fallback variable lenn can only decrease as many times as it has increased and it is bounded by i 
   - So the maximum number of operations can be 2m

   LPS construction time complexity is O(n)

2. Searching the text
   - The only operation the text pointer i performs is incrementing, so it moves forward at most n times
   - The pattern pointer can fallback on a mismatch, but only as many times as it has advanced, it only advances when i advances so j's total movement is also bounded by n

   The searching time complexity is O(m)

So, total time complexity is O(m) + O(n) = O(m+n)

This holds good for any input, so there is no bad case that makes KMP worse than linear time

**Task 3 : Worst case for KMP and when it arises in genomics**

- KMP is always O(n+m), the worst case is still linear
- In the worst case every single character is visited and compared, with no early termination
- This happens when the pattern and the text are highly repititive and the LPS table causes maximum fallbacks without ever completing a match

- Example of a worst case scenario
   - Text : A A A A A A A A A A A 
   - Pattern : A A A A B

- At every position, KMP matches all the A's in the pattern before hitting a mismatch on B
- The LPS table then causes a series of fallbacks before moving forward
- The complexity is O(m+n), but the algorithm does the maximum possible work at every step

Scenarious in genomic data where this case may arise:
- Telomeric regions : human telomeres consist of thousands of repetitions of the unit TTAGGG. Searching for a pattern like TTAGGGTTAGT within this region causes the same near-complete match followed by mismatch pattern repeatedly across the entire sequence.

- Centromeric repeats : centromeres contain long tandem repeat arrays (e.g., alpha satellite DNA with ~171 bp repeat units). A pattern that almost matches the repeat unit will cause maximum fallbacks across millions of bases.

- Homopolymer runs :  poly-A tails in mRNA sequences (sometimes thousands of A's long) combined with a pattern like "AAAAAC" trigger near-complete matches at every position before hitting the final mismatch.
    

**Task 4 : Comparison with Boyer-Moore Algorithm**

Boyer-Moore Algorithm

- It preprocesses the pattern to build a bad character table for each character in the alphabet
- It stores the rightmost position of that character in the pattern
- When a mismatch occurs, it uses this table to calculate how far to skip forward in the text
- Unlike KMP which scans left to right through both text and pattern, Boyer-Moore scans the pattern right to left. 
- This means a mismatch on the very first comparison (the last character of the pattern) can immediately skip the entire pattern length forward, without inspecting any other characters in that window

Performance comparison between KMP and Boyer-Moore

| PROPERTY                  | KMP    | Boyer-Moore |
|---------------------------|--------|-------------|
|Pre processing             | O(m)   | O(m+x)      | (x is alphabet size : 4 for DNA) 
|Search : Best Case         | O(n+m) | O(n/m)      |
|Search : Average Case      | O(n+m) | O(n)        |
|Search : Worst Case        | O(n+m) | O(n*m)      |
|Skips Text Characters      | No     | Yes         |
|Handles overlapping matches| Yes    | No          |

When KMP outperforms Boyer-Moore : 

- Boyer-Moore's worst case has the same input as KMP's worst case : highly repetitive sequences
- In such cases, the bad character table produces shifts of 1 or 0, which will essentially make Boyer-Moore a naive scan
- KMP Algorithm remains linear irrespective of the input

So, KMP is better for 
- Repetitive DNA sequences (telomeres, centromeres, homopolymer runs)
- Short patterns (less than 5 base pairs), in this case there is no proper advantage of Boyer-Moore's skip
- Any situation where worst case is guaranteed

When Boyer-Moore outperforms KMP : 

- On non-repetitive DNA with a diverse character distribution, Boyer-Moore's skips become very large, making it more efficient

So, Boyer-Moore is better for
- Non repetitive DNA sequences (exons, coding sequences, intergenic regions)
- Long patterns (more than 8-10 base pairs), has room for larger skip distances
- Large scale databse searches where average case speed will dominate


