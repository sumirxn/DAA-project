**Intelligent Text Correction using Edit Distance (Damerau-Levenshtein Distance)**

- Text editors and messaging applications need to detect misspelled words and suggest corrections
- The system compares user input against a dictionary and finds the closest valid word
- The distance metric used is the minimum edit distance (Damerau-Levenshtein distance), which counts the minimum number of operations (insertions, deletions, substitutions, transpositions) needed to transform one string into another
- We implement a Dynamic Programming approach to efficiently compute this distance

**Task 1: Formulating the Recurrence Relation**

- Let `dp[i][j]` represent the minimum edit distance between the first `i` characters of the source string and the first `j` characters of the target string
- Base cases:
  - `dp[0][j] = j` (need j insertions to transform empty string to target[0..j-1])
  - `dp[i][0] = i` (need i deletions to transform source[0..i-1] to empty string)

- Recurrence relation:
  - If `source[i-1] == target[j-1]`:
    - `dp[i][j] = dp[i-1][j-1]` (no operation needed, characters match)
  - Else:
    - `dp[i][j] = 1 + min(dp[i-1][j], dp[i][j-1], dp[i-1][j-1])`
      - `dp[i-1][j] + 1` represents deletion of source[i-1]
      - `dp[i][j-1] + 1` represents insertion of target[j-1]
      - `dp[i-1][j-1] + 1` represents substitution of source[i-1] with target[j-1]
      - 
  - If two adjacent characters are swapped:
    - If i > 1 and j > 1 and source[i-1] == target[j-2] and source[i-2] == target[j-1]:
      dp[i][j] = min(dp[i][j], dp[i-2][j-2] + 1)

Pseudocode:
```
function editDistance(source, target)
    input : source string (user typed word)
            target string (dictionary word)
    output : minimum edit distance
    
    m = length of source
    n = length of target
    
    // Create DP table
    dp[0..m][0..n]
    
    // Initialize base cases
    for i = 0 to m:
        dp[i][0] = i
    
    for j = 0 to n:
        dp[0][j] = j
    
    // Fill the DP table
    for i = 1 to m:
        for j = 1 to n:
            if source[i-1] == target[j-1] then:
                dp[i][j] = dp[i-1][j-1]
            else:
                dp[i][j] = 1 + min(
                    dp[i-1][j],      // deletion
                    dp[i][j-1],      // insertion
                    dp[i-1][j-1]     // substitution
                )
           if i > 1 and j > 1 and source[i-1] == target[j-2] and source[i-2] == target[j-1] then:
                dp[i][j] = min(dp[i][j], dp[i-2][j-2] + 1)
    
    return dp[m][n]
```

**Task 2: Implementing the DP Solution**

- Create a 2D DP table where each cell dp[i][j] stores the edit distance
- Fill the table row by row, computing each cell based on previously computed values
- Handle edge cases:
  - Empty source or target strings
  - Single character strings
  - Identical strings (distance = 0)



**Task 3: Reconstructing the Operation Sequence**

- To output the sequence of operations (insert, delete, substitute, transposition), we need to backtrack through the DP table
- Start from dp[m][n] and trace back to dp[0][0]
- At each step, determine which operation was used by comparing:
  - `dp[i][j]` with `dp[i-1][j-1]` → substitution or match
  - `dp[i][j]` with `dp[i-1][j]` → deletion
  - `dp[i][j]` with `dp[i][j-1]` → insertion
  - `dp[i][j]` with `dp[i-2][j-2]` → transposition

Pseudocode for backtracking:
```
function reconstructOperations(source, target, dp)
    input : source string, target string, DP table
    output : list of operations
    
    operations = []
    i = length of source
    j = length of target
    
    while i > 0 or j > 0:
        if i > 0 and j > 0 and source[i-1] == target[j-1]:
            // characters match, move diagonally
            i = i - 1
            j = j - 1
        else if i > 0 and j > 0 and dp[i][j] == dp[i-1][j-1] + 1:
            // substitution
            operations.append("Substitute " + source[i-1] + " with " + target[j-1] + " at position " + i)
            i = i - 1
            j = j - 1
        else if i > 0 and dp[i][j] == dp[i-1][j] + 1:
            // deletion
            operations.append("Delete " + source[i-1] + " at position " + i)
            i = i - 1
        else if j > 0 and dp[i][j] == dp[i][j-1] + 1:
            // insertion
            operations.append("Insert " + target[j-1] + " at position " + (i + 1))
            j = j - 1
    
    return reverse(operations)
```
