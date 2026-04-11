**Time Complexity Analysis**

**DP Solution Time Complexity**

The edit distance DP algorithm involves filling a table of size (m+1) × (n+1) where m is the length of the source word and n is the length of the target word.

1. Building the DP Table
   - We initialize the first row and first column: O(m + n)
   - We fill each cell in the table by looking up at most 3 previous cells and performing constant-time comparisons
   - Total cells to fill: m × n
   - Each cell takes O(1) time to compute
   
   DP table construction time complexity is O(m × n)

2. Backtracking to Reconstruct Operations (Task 4)
   - Starting from dp[m][n], we trace back to dp[0][0]
   - In the worst case, we make m + n moves (either increment i, j, or both)
   - Each step records one operation, which takes O(1) time
   
   Backtracking time complexity is O(m + n)

**Total Time Complexity: O(m × n)**

**Space Complexity Analysis**

1. DP Table Storage
   - We store a 2D array of size (m+1) × (n+1)
   - Each cell stores one integer (the minimum distance)
   
   DP table space: O(m × n)

2. Operations List (from backtracking)
   - In the worst case, we perform m + n operations (all deletions and insertions, no substitutions, or all substitutions)
   - Each operation is a string describing the action
   
   Operations space: O(m + n)

**Total Space Complexity: O(m × n)**

Space Optimization:
- If only the final edit distance is needed (not the operations), we can optimize space to O(min(m, n))
- We only need to keep two rows at a time: the current row being computed and the previous row
- For reconstructing operations, we must maintain the full DP table or use a different strategy (e.g., storing backpointers)

**Worst-Case Scenarios**

1. Completely Different Strings
   - When source and target strings share no common subsequences
   - Every cell in the DP table must be filled with non-trivial comparisons
   - Example: "abc" vs "xyz" → distance = 3 (3 substitutions or 3 insertions + 3 deletions)
   - Complexity: Θ(m × n) - the algorithm touches every cell

2. Very Long Strings
   - When m and n are large (e.g., m = 1000, n = 1000)
   - The table size becomes 1,001 × 1,001 = 1,002,001 cells
   - With dictionary lookups for many words, the total time becomes O(k × m × n) where k is the dictionary size
   
3. One String Much Longer Than the Other
   - When n >> m (or vice versa)
   - Still O(m × n), but the actual work can be better predicted
   - Example: checking if a 5-character typo matches any word in a 100,000-word dictionary

**Real-World Scenarios in Text Correction**

Realistic Typing Errors (Average Case):
- Short words (m, n ≤ 10): O(100) operations
- Medium words (m, n ≤ 20): O(400) operations
- Long words (m, n ≤ 30): O(900) operations
- Completion time: negligible (< 1 ms)

Dictionary Size Impact:
- Small dictionary (100 words): 100 × O(m × n) for all comparisons
- Medium dictionary (10,000 words): 10,000 × O(m × n)
- Large dictionary (100,000+ words): 100,000 × O(m × n)
- For real-time suggestions, only top N closest matches are computed

**Comparison with Naive Approach**

Naive Algorithm (checking all possible edit sequences):
- Without memoization, the number of possible edit sequences is exponential
- Time complexity: O(3^(m+n)) in the worst case
- This is why DP is essential for autocorrect systems

DP vs Naive:
| Metric              | Naive        | DP      |
|---------------------|--------------|---------|
| Time Complexity     | O(3^(m+n))   | O(m×n)  |
| Space Complexity    | O(m+n)       | O(m×n)  |
| Practical Usage     | Infeasible   | Feasible|


**Optimization Techniques for Large-Scale Dictionaries**

1. Early Termination
   - If the distance exceeds a threshold (e.g., 2), stop computing
   - Reduces average-case time

2. Space-Optimized DP (1D Array)
   - For large dictionaries, use rolling array technique
   - Reduces space from O(m × n) to O(min(m, n))
   - Slightly increases time due to array shifting

3. Preprocessing Dictionary
   - Sort words by length
   - Skip words that differ in length by more than the threshold
   - Example: if typed word is length 5 and threshold is 1, skip words of length < 4 or > 6

4. Parallel Processing
   - Compute distances for multiple dictionary words in parallel
   - For k words in dictionary: O(k × m × n) becomes O(m × n) with k processors
