#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_COURSES     100000
#define MAX_EDGES       500000
#define MAX_SEMESTERS   100000

// ─── Adjacency List ──────────────────────────────────────────────────

typedef struct Node {
    int course;
    struct Node *next;
} Node;

Node *graph[MAX_COURSES];   // graph[u] → list of courses unlocked by completing u
int   inDegree[MAX_COURSES];
int   semesterOf[MAX_COURSES]; // which semester each course is assigned to

// Add a directed edge u → v (completing u unlocks v)
void addEdge(int u, int v) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->course = v;
    node->next   = graph[u];
    graph[u]     = node;
    inDegree[v]++;
}

// Free adjacency list for n courses
void freeGraph(int n) {
    for (int i = 0; i < n; i++) {
        Node *cur = graph[i];
        while (cur) {
            Node *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
        graph[i] = NULL;
    }
}


// ─── TASK 2 & 3 — Kahn's BFS Topological Sort + Minimum Semesters ─────────────
// Each BFS layer corresponds to one semester.
// All courses dequeued in the same layer are taken in the same semester.
// Returns the minimum number of semesters, or -1 if a cycle is detected.
int minSemesters(int n, int *order, int *orderSize) {

    int *queue = (int *)malloc(n * sizeof(int));
    int  head  = 0, tail = 0;

    // Enqueue all courses with no prerequisites — these are semester 1
    for (int i = 0; i < n; i++) {
        if (inDegree[i] == 0)
            queue[tail++] = i;
    }

    int semesters = 0;
    int completed = 0;
    *orderSize    = 0;

    while (head < tail) {
        semesters++;

        // Process exactly one BFS layer (one semester) at a time
        int layerSize = tail - head;
        for (int i = 0; i < layerSize; i++) {
            int u = queue[head++];
            semesterOf[u]        = semesters;
            order[(*orderSize)++ ] = u;
            completed++;

            // Unlock dependent courses
            for (Node *nb = graph[u]; nb != NULL; nb = nb->next) {
                inDegree[nb->course]--;
                if (inDegree[nb->course] == 0)
                    queue[tail++] = nb->course;
            }
        }
    }

    free(queue);

    // Cycle detection: if some courses were never completed they are in a cycle
    if (completed != n)
        return -1;

    return semesters;
}


// ─── Experiment Runner ────────────────────────────────────────────────────────

void runExperiment(const char *label, int n,
                   int (*edges)[2], int edgeCount) {

    printf("\n%s\n", label);
    printf("  Courses: %d | Prerequisites: %d\n", n, edgeCount);

    // Reset graph state
    for (int i = 0; i < n; i++) {
        graph[i]    = NULL;
        inDegree[i] = 0;
    }

    // Build graph
    for (int i = 0; i < edgeCount; i++)
        addEdge(edges[i][0], edges[i][1]);

    int *order     = (int *)malloc(n * sizeof(int));
    int  orderSize = 0;

    clock_t start = clock();
    int result    = minSemesters(n, order, &orderSize);
    clock_t end   = clock();
    double  ms    = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

    if (result == -1) {
        printf("  Result : CYCLE DETECTED — no valid ordering exists\n");
        printf("  Completed : 0 out of %d courses scheduled\n", n);
    } else {
        // Print semester-by-semester plan
        for (int s = 1; s <= result; s++) {
            printf("  Semester %d : ", s);
            int first = 1;
            for (int i = 0; i < orderSize; i++) {
                if (semesterOf[order[i]] == s) {
                    if (!first) printf(", ");
                    printf("Course %d", order[i]);
                    first = 0;
                }
            }
            printf("\n");
        }
        printf("  Minimum semesters : %d | Time : %.3fms\n", result, ms);
    }

    free(order);
    freeGraph(n);
}


// ─── Main ─────────────────────────────────────────────────────────────────────

int main(void) {

    printf("=== Academic Course Planning — Topological Sort ===\n");

    // TASK 1 — Show graph construction for the simple mixed example
    printf("\nTASK 1: Graph Modelling\n");
    printf("  Courses  : 0, 1, 2, 3, 4, 5, 6, 7\n");
    printf("  Edges    : 0->2, 0->3, 1->3, 1->4, 2->5, 3->5, 3->6, 4->6, 5->7, 6->7\n");
    printf("  (Course A -> Course B means A must be completed before B)\n");

    // TASK 2 — Show topological order on the same graph
    printf("\nTASK 2: Topological Order\n");
    {
        int n = 8;
        int edges[][2] = {
            {0,2},{0,3},{1,3},{1,4},{2,5},{3,5},{3,6},{4,6},{5,7},{6,7}
        };
        int edgeCount = 10;

        for (int i = 0; i < n; i++) { graph[i] = NULL; inDegree[i] = 0; }
        for (int i = 0; i < edgeCount; i++) addEdge(edges[i][0], edges[i][1]);

        int *order     = (int *)malloc(n * sizeof(int));
        int  orderSize = 0;
        int  result    = minSemesters(n, order, &orderSize);

        printf("  Valid topological order : ");
        for (int i = 0; i < orderSize; i++) {
            printf("%d", order[i]);
            if (i < orderSize - 1) printf(" -> ");
        }
        printf("\n");

        free(order);
        freeGraph(n);
    }

    // ── Experiments ──────────────────────────────────────────────────────────

    // Exp 1: Simple chain 0->1->2->3->4
    {
        int edges[][2] = {{0,1},{1,2},{2,3},{3,4}};
        runExperiment("Exp 1: Simple Chain (0->1->2->3->4)",
                      5, edges, 4);
    }

    // Exp 2: Diamond / fan-in
    {
        int edges[][2] = {{0,2},{1,2},{2,3},{0,4},{1,4},{4,5}};
        runExperiment("Exp 2: Fan-in Diamond",
                      6, edges, 6);
    }

    // Exp 3: Fully parallel (no edges)
    {
        int edges[][2] = {0};
        runExperiment("Exp 3: Fully Parallel (no prerequisites)",
                      5, edges, 0);
    }

    // Exp 4: Cycle detection 0->1->2->0
    {
        int edges[][2] = {{0,1},{1,2},{2,0}};
        runExperiment("Exp 4: Cycle Detection (0->1->2->0)",
                      3, edges, 3);
    }

    // Exp 5: Mixed curriculum
    {
        int edges[][2] = {
            {0,2},{0,3},{1,3},{1,4},{2,5},{3,5},{3,6},{4,6},{5,7},{6,7}
        };
        runExperiment("Exp 5: Mixed Curriculum Structure",
                      8, edges, 10);
    }

    // Exp 6: Large sparse graph — 10,000 courses, 12,000 edges
    printf("\nExp 6: Large Sparse Graph\n");
    printf("  Courses: 10000 | Prerequisites: 12000\n");
    {
        int n          = 10000;
        int edgeCount  = 12000;
        int (*edges)[2] = malloc(edgeCount * sizeof(*edges));

        srand(42);
        // Build a random sparse DAG (always edge from lower to higher index to guarantee acyclic)
        for (int i = 0; i < edgeCount; i++) {
            int u       = rand() % (n - 1);
            int v       = u + 1 + rand() % (n - u - 1);
            edges[i][0] = u;
            edges[i][1] = v;
        }

        for (int i = 0; i < n; i++) { graph[i] = NULL; inDegree[i] = 0; }
        for (int i = 0; i < edgeCount; i++) addEdge(edges[i][0], edges[i][1]);

        int *order     = (int *)malloc(n * sizeof(int));
        int  orderSize = 0;
        clock_t start  = clock();
        int result     = minSemesters(n, order, &orderSize);
        clock_t end    = clock();
        double  ms     = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

        printf("  Minimum semesters : %d | Time : %.3fms\n", result, ms);

        free(edges);
        free(order);
        freeGraph(n);
    }

    // Exp 7: Large chain — 10,000 courses, worst case depth
    printf("\nExp 7: Large Dense Chain (worst case — 10,000 semesters)\n");
    printf("  Courses: 10000 | Prerequisites: 9999\n");
    {
        int n          = 10000;
        int edgeCount  = n - 1;
        int (*edges)[2] = malloc(edgeCount * sizeof(*edges));

        for (int i = 0; i < edgeCount; i++) {
            edges[i][0] = i;
            edges[i][1] = i + 1;
        }

        for (int i = 0; i < n; i++) { graph[i] = NULL; inDegree[i] = 0; }
        for (int i = 0; i < edgeCount; i++) addEdge(edges[i][0], edges[i][1]);

        int *order     = (int *)malloc(n * sizeof(int));
        int  orderSize = 0;
        clock_t start  = clock();
        int result     = minSemesters(n, order, &orderSize);
        clock_t end    = clock();
        double  ms     = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

        printf("  Minimum semesters : %d | Time : %.3fms\n", result, ms);

        free(edges);
        free(order);
        freeGraph(n);
    }

    return 0;
}
