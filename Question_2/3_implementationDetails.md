**Implementation Details**

1. Programming Strategy
   The solution is split into two functions:
   - buildGraph() : constructs the adjacency list and in-degree array from a list of prerequisite pairs
   - minSemesters() : runs the BFS-based topological sort and returns the minimum semester count and course order

   For experiments, a runExperiment() function builds the graph, runs minSemesters(), and prints the semester-by-semester plan alongside the total semester count, allowing easy verification of both correctness and performance.

2. Data Structures
   - Adjacency List (int*[], size n)
     Each index u holds a dynamic array of courses that u unlocks (i.e., courses that list u as a prerequisite).
     Chosen over an adjacency matrix because course graphs are sparse — an n×n matrix would waste O(n^2) memory when e << n^2.

   - In-Degree Array (int[], size n)
     Stores the number of unmet prerequisites for each course.
     Decremented as prerequisites are completed. When a value reaches 0, the course becomes available.
     Central to Kahn's algorithm — this single array drives the entire BFS.

   - BFS Queue (circular queue, capacity n)
     Holds all currently available courses (in-degree 0) waiting to be scheduled.
     Processes courses in FIFO order. The layer-size technique partitions this queue into semesters without needing a nested data structure.

   - Semester Plan Array (int*[], size maxSemesters)
     Each index stores a list of course IDs scheduled in that semester.
     Used for output and experiment display.

3. Design Techniques
   - BFS Layer Partitioning
     At the start of each BFS iteration, we record the current queue size as the layer size.
     We dequeue exactly that many courses, process them, and treat everything we dequeue in this round as one semester.
     Newly unlocked courses (enqueued during this round) are processed in the next round, i.e., the next semester.
     This cleanly separates semesters without any extra data structures.

   - In-Place In-Degree Updates
     Rather than rebuilding the in-degree array between iterations, we decrement it in-place as each edge is processed.
     This avoids any repeated work and keeps the algorithm at a single O(n + e) pass.

   - Cycle Detection via Count Check
     After the BFS completes, we compare the number of scheduled courses against n.
     If they differ, some courses were never added to the queue — meaning they are part of a cycle.
     This is a constant-time check that requires no extra traversal.

   - Dynamic Memory Allocation
     Adjacency lists and semester plan arrays are heap-allocated using malloc so the implementation can handle variable-sized catalogs up to 100,000 courses without stack overflow.
