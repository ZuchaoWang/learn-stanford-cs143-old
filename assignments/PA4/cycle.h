#ifndef CYCLE_H
#define CYCLE_H

#include "list.h"

class CycleDetector {
private:
    int vertices;
    List<int>** adjLists;
    bool isCyclicUtil(int v, bool visited[], bool *recStack);

public:
    CycleDetector(int vertices);
    void addEdge(int v, int w);
    bool isCyclic();
};

#endif