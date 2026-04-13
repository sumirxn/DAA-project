**Academic Course Planning using Topological Sort (Kahn's Algorithm)**

A university's course catalog can contain hundreds or thousands of courses, each with specific prerequisites.
Students must complete prerequisites before enrolling in advanced courses.
The goal is to determine a valid order of course completion and the minimum number of semesters needed.
We use topological sorting on a Directed Acyclic Graph (DAG) to solve this efficiently.

**Task 1 : Modelling the Course Structure as a Directed Graph**

We represent the course catalog as a Directed Acyclic Graph (DAG) where:
- Each node represents a course (identified by a course ID from 0 to n-1)
- A directed edge from course A to course B means "A must be completed before B" (A is a prerequisite of B)

For each node we maintain:
- An adjacency list to store which courses become unlocked after completing this course
- An in-degree count that tracks how many prerequisites a course still has remaining

A course with in-degree 0 has no unmet prerequisites and can be taken immediately.
The graph must be a DAG (no cycles), because a cycle would mean a course is its own prerequisite, which is impossible in a valid curriculum.

**Task 2 : Topological Sorting using Kahn's Algorithm**

We use Kahn's Algorithm (BFS-based topological sort) rather than DFS, because Kahn's naturally groups courses by the earliest semester they can be taken.
This makes it directly useful for computing the semester plan in Task 3.

Core idea of Kahn's Algorithm:
- We start with all courses that have no prerequisites (in-degree 0) — these can be taken in semester 1
- After "completing" a course, we reduce the in-degree of all courses that depend on it
- Any course whose in-degree drops to 0 is now available and can be taken in the next semester
- We repeat this process until all courses are scheduled

Pseudocode :
```
function topoSort(graph, n)
    input : graph - adjacency list of n courses
            n - total number of courses
    output : list of courses in a valid topological order (or empty if cycle detected)

    // Step 1: Compute in-degree for every course
    inDegree[0..n-1] = 0
    for each course u in 0..n-1 do:
        for each course v in graph[u] do:
            inDegree[v] = inDegree[v] + 1

    // Step 2: Enqueue all courses with no prerequisites
    queue = empty queue
    for each course u in 0..n-1 do:
        if inDegree[u] == 0 then:
            enqueue(queue, u)

    // Step 3: Process courses in BFS order
    order = empty list
    while queue is not empty do:
        u = dequeue(queue)
        append u to order
        for each neighbour v of u do:
            inDegree[v] = inDegree[v] - 1
            if inDegree[v] == 0 then:
                enqueue(queue, v)

    // Step 4: Cycle detection
    if length(order) != n then:
        return [] // cycle detected, no valid ordering exists
    return order
```

**Task 3 : Computing the Minimum Number of Semesters**

The topological sort above gives a valid order, but not necessarily the fastest plan.
To compute the minimum number of semesters, we extend Kahn's algorithm to track levels (BFS layers).

All courses with in-degree 0 at the same point in time can be taken simultaneously in the same semester.
After completing them, newly unlocked courses form the next semester, and so on.
The number of BFS layers is the minimum number of semesters.

Pseudocode :
```
function minSemesters(graph, n)
    input : graph - adjacency list of n courses
            n - total number of courses
    output : minimum number of semesters to complete all courses, or -1 if a cycle exists

    // Step 1: Compute in-degree for every course
    inDegree[0..n-1] = 0
    for each course u in 0..n-1 do:
        for each course v in graph[u] do:
            inDegree[v] = inDegree[v] + 1

    // Step 2: Enqueue all courses available in semester 1
    queue = empty queue
    for each course u in 0..n-1 do:
        if inDegree[u] == 0 then:
            enqueue(queue, u)

    semesters = 0
    completed = 0

    // Step 3: Process one semester (one BFS layer) at a time
    while queue is not empty do:
        semesters = semesters + 1
        layerSize = size of queue // all courses in this semester
        for i = 1 to layerSize do:
            u = dequeue(queue)
            completed = completed + 1
            for each neighbour v of u do:
                inDegree[v] = inDegree[v] - 1
                if inDegree[v] == 0 then:
                    enqueue(queue, v)

    // Step 4: Cycle detection
    if completed != n then:
        return -1 // cycle detected
    return semesters
```

Key insight:
The BFS layer approach ensures we assign each course to the earliest possible semester.
Within a semester, we process all courses whose prerequisites were just satisfied — never earlier, never later.
This gives the globally optimal (minimum) semester count.
