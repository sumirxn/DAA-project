**Time and Space Complexity Analysis**

Let n = number of courses (nodes) and e = number of prerequisite edges

**Task 4 : Time and Space Complexity**

1. Building the In-Degree Array: 
   We iterate over every edge in the graph exactly once to compute in-degree values. 
   This is O(e)

2. Initial Queue Population: 
   We scan all n courses once to find those with in-degree 0. 
   This is O(n)

3. BFS Processing (Kahn's Algorithm): 
   Each course is enqueued and dequeued exactly once : O(n) 

   Each edge is relaxed (in-degree decremented) exactly once : O(e)
   
   Total processing cost is O(n + e)

5. Semester Layer Tracking: 
   The layer-size variable divides the BFS into rounds without any additional passes
   No extra cost beyond the BFS itself

Total time complexity : O(n + e)
This is optimal, since we must examine every course and every prerequisite relationship at least once to determine the correct ordering.

Total space complexity : O(n + e)
- Adjacency list stores all edges : O(n + e)
- In-degree array : O(n)
- BFS queue holds at most n courses in the worst case : O(n)
- Output order array : O(n)

**Efficiency for Large-Scale Course Catalogs**

For a university with thousands of courses, the graph is typically sparse.
Most courses have only a few prerequisites, so e is much closer to n than to n^2.
In a sparse graph, O(n + e) is effectively O(n), making the algorithm highly efficient even at scale.

Example: A catalog with 10,000 courses and 25,000 prerequisite edges
- The algorithm processes all 10,000 nodes and 25,000 edges in a single BFS pass
- This is well within milliseconds on any modern processor
- Memory usage scales linearly with catalog size

Worst case: A chain of courses where each requires the previous one (e = n-1)
- Time is still O(n), just with a minimum semester count equal to n (one course per semester)
- This is unavoidable — no algorithm can do better if the dependencies form a chain

**Cycle Detection**
If the completed course count at the end is less than n, a cycle exists in the prerequisite graph.
This catches invalid course structures (e.g., Course A requires Course B and Course B requires Course A)
Cycle detection comes for free with no additional time cost beyond the standard BFS pass.
