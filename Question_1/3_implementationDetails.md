**Implemetation Details**

1. Programming Strategy
   - KMP : 
      - First we preprocess the pattern using buildLPS()
      - Then we search the text using kmpSearch()
   
   - Boyer-Moore : 
      - Preprocessing done with buildBadChar()
      - Searching done with bmSearch()

   For the experiments, a runExperiment function runs both algorithms on the same input and prints a side by side comparision, ensuring that both the algorithms are tested under the identical conditions

2. Data Structures
   - LPS Array(int[], size m)      
      - Stores the failure function for KMP. Built once from the pattern before searching begins
      - Each value tells KMP how far to fall back in the pattern on a mismatch, avoiding any backward movement in the text

   - Bad Character Table(int[256], fixed size)
      - Stores the rightmost occurrence of each ASCII character in the pattern
      - Used by Boyer-Moore to calculate how far to skip forward in the text on a mismatch. Size is fixed at 256 to cover all ASCII characters

   - Matches array(int[], size MAX_MATCHES)
      - Stores the starting position of every match found
      - Shared across both algorithms so results can be compared directly for accuracy checking

3. Design Techniques
   - Two-pointer technique 
      - KMP uses two pointers, i into the text and j into the pattern. 
      - The key constraint is that i never moves backward, which is what gives KMP its O(n) search guarantee.

   - Preprocessing  
      - both algorithms preprocess the pattern before searching. 
      - (O(m) for KMP, O(m + σ) for Boyer-Moore) 

   - Dynamic memory allocation
      - the LPS array and match results are heap allocated using malloc so the implementation handles variable-length sequences up to 1,000,000 characters without stack overflow.
