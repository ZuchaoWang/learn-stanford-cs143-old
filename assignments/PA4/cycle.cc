#include "cycle.h"

bool CycleDetector::isCyclicUtil(int v, bool visited[], bool *recStack)
{
  visited[v] = true;
  recStack[v] = true;

  // check all neighbors
  for (List<int> *crawler = adjLists[v]; crawler != NULL; crawler = crawler->tl())
  {
    int i = *(crawler->hd());
    if (!visited[i] && isCyclicUtil(i, visited, recStack))
    {
      return true;
    }
    else if (recStack[i])
    {
      return true;
    }
  }

  recStack[v] = false; // remove the vertex from recursion stack
  return false;
}

CycleDetector::CycleDetector(int vertices)
{
  this->vertices = vertices;
  adjLists = new List<int> *[vertices];
  for (int i = 0; i < vertices; i++)
  {
    adjLists[i] = NULL;
  }
}

void CycleDetector::addEdge(int v, int w)
{
  int *ptr = new int(w);
  adjLists[v] = new List<int>(ptr, adjLists[v]);
}

bool CycleDetector::isCyclic()
{
  bool *visited = new bool[vertices];
  bool *recStack = new bool[vertices];
  for (int i = 0; i < vertices; i++)
  {
    visited[i] = false;
    recStack[i] = false;
  }

  // call the recursive helper function to detect cycle in different DFS trees
  for (int i = 0; i < vertices; i++)
  {
    if (isCyclicUtil(i, visited, recStack))
    {
      return true;
    }
  }

  return false;
}
