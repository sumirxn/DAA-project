**Implementation Details**

**1. Programming Strategy**

Edit Distance (Levenshtein Distance):
- Build a 2D DP table to compute the minimum edit distance between two strings
- Preprocess by creating the table and initializing base cases
- Fill the table iteratively using the recurrence relation
- Backtrack through the table to reconstruct the exact sequence of operations

For the experiments:
- Create a correctionSystem function that:
  1. Loads a dictionary of valid words
  2. For each typed (misspelled) word, computes edit distance to all dictionary words
  3. Returns the top N closest matches sorted by distance
  4. Provides detailed operation sequences for the best match

**2. Data Structures**

- DP Table (int[m+1][n+1])
  - 2D array storing the edit distance for all substring pairs
  - Initialized with base cases (distances from empty string)
  - Filled using dynamic programming to avoid recomputation
  - Size: (source_length + 1) × (target_length + 1)

- Operations Array (struct[], size m+n)
  - Each element records one operation: type (DELETE/INSERT/SUBSTITUTE), character, and position
  - Built during the backtracking phase
  - Used to show the user the exact corrections needed

- Match Results (struct[], size k)
  - For each dictionary word, stores:
    - The word itself
    - Edit distance from the typed word
    - Operation sequence needed
    - Sorted by distance for presentation to user

- Dictionary (char*[], size n_words)
  - Stored as an array of strings
  - Can be sorted by length for optimization (early termination)
  - For large dictionaries, can use a trie or hash table for faster lookups

**3. Design Techniques**

- Dynamic Programming with Memoization
  - Avoids recomputing the same subproblems
  - Every dp[i][j] is computed exactly once
  - Enables efficient solution where naive approach would take exponential time

- Two-Pointer Technique (in backtracking)
  - i and j pointers traverse the DP table from bottom-right to top-left
  - At each step, we move based on which previous cell was used (deletion, insertion, or substitution)
  - Ensures we reconstruct operations in the correct order

- Dictionary Filtering
  - For efficiency, pre-filter dictionary words:
    - Group words by length
    - Only compare with words within ±2 characters of typed word length
    - Skip words if distance exceeds a threshold (e.g., 2 or 3)

- Early Termination Optimization
  - If edit distance exceeds a maximum threshold, stop computing further
  - Example: if threshold = 2 and current distance reaches 3, skip remaining cells
  - Reduces average-case time for large dictionaries

