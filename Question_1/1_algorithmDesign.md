**DNA Pattern Matching using KMP and Boyer-Moore Algorithm**

- DNA sequences consist of the characters {A, G, C, T} and can span millions of characters
- Seaching for a patter within such a sequence using a naive approach leads to redundant comparisons and poor performance
- We are looking at two algorithms to make this searching more efficient
    - KMP Algorithm
    - Boyer Moore Algorithm

**Task 1 : Constructing the LPS array**
- The LPS (Longest Prefix Suffix) array is the preprocessing step that makes KMP efficient
- It is computed from the pattern alone

- For a pattern of length m, `lps[i]` stores the length of the longest proper prefix of `pattern[0...i]`, that is also a suffix of `pattern[0...i]` (proper prefix means that a prefix is not the full string itself)

- When a mismatch occurs after matching j characters of the pattern, we know that `pattern[0...j-1]` matched `text[i-j....i-1]`
- The LPS value `lps[j-1]` tells us the longest portion of that matched prefix that can still serve as the start of a new match, so we jump to `lps[j-1]` instead of restarting from 0

Pseudocode : 
```
function buildLPS(pattern, m) 
    input : pattern, length of pattern
    output : array of length m
    lps[0]=0 //first character does not have proper prefix
    len = 0 //length of current matching prefix-suffix
    i = 1 //we start from second char
    while i<m do :
        if pattern[i] == pattern[len] then : 
            //current char extends the current matching prefix-suffix
            len = len + 1
            lps[i] = len
            i = i + 1
        else : 
            if len != 0 then : 
                // we try a shorter prefix using the previously computed LPS
                len = lps[len-1]
            else : 
                // no prefix matched for this position
                lps[i] = 0
                i = i+1
    return lps
```

**Task 2 : KMP Algorithm**

- In KMP ALgorithm, we use two pointers
    - i : moves through the text
    - j : tracks how many pattern characters have been matched so far

- When a mismatch happens, instead of restarting, we jump to the next postion using the LPS array

- while searching
    - if a match occurs (`text[i]`==`pattern[j]`), then both pointers advance
    - once j reaches m, it means that the full match is found, we report the match and then set `j = lps[j-1]`, so we can continue searching for more occurences of the pattern and not miss any overlapping matches
    - when there is a mismatch
        - if j>0, we set `j=lps[j-1]` and continue searching
        - if j=0, we advance the i pointer

Pseudocode : 
```
function KMPsearch(text, n, pattern, m)
    input : text - the DNA sequence to search
            n - length of text
            pattern - gene pattern 
            m - length of pattern
    output : list of all starting positions where the pattern occurs in text

    lps = buildLPS(pattern, m) //preprocessing step 
    i = 0 //pointer for the text
    j = 0 //pointer for the pattern
    while i<n do:
        if text[i]==pattern[j] then:
            //characters match, so advance both pointers
            i = i + 1
            j = j + 1
        if j==m then:
            //this means that full match is found
            report match at position (i-j) //storing the match
            //lps[j-1] will give the longest prefix of patter that can still continue from this point
            //enabling overlapping matches
            j = lps[j-1]
        else if i<n AND text[i]!=pattern[j] then:
            //there is a mismatch 
            if j!=0 then :
                //we already matched j characters, so we can use LPS to skip forward
                j = lps[j-1]
            else
                //we have not matched any characters, just advance the text pointer and restart
                i = i+1
```