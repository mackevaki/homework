/* в глубину, рекурсия */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

const int n=5;
bool visited[255];

int graph[5][5] = 
{
{0, 1, 0, 0, 1},
{1, 0, 1, 1, 0},
{0, 1, 0, 0, 1},
{0, 1, 0, 0, 1},
{1, 0, 1, 1, 0}
};

void DFS(int st) { // st – стартовый узел
  int r;
  printf("%d\n", st+1);
  visited[st] = true;
  for (r = 0; r < n; r++)
    if ((graph[st][r]!=0) && (!visited[r]))
      DFS(r);
}

int main() {
  for(int i = 0; i < 255; i++) 
    visited[i] = false;
  DFS(0);
return 0;
}
