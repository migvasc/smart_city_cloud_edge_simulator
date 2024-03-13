
from collections import deque
import networkx as nx



graph = {'A': ['B'],
'B': ['A','E', 'C'],
'C': ['B','D','F','G'],
'D': ['C','I'],
'E' : ['B'],
'F': ['C'],
'G': ['C','H'],
'H':['G'],
'I': ['D','O','Q','N','L', 'J'],
'O': ['I','P'],
'P': ['O'],
'Q': ['I','R','S', 'T'],
'R': ['Q'],
'S': ['Q'],
'T': ['Q'],
'N': ['I'],
'L': ['I','M'],
'M': ['L'],
'J': ['I','K'],
'K': ['J'],
}

#print(graph)

def find_shortest_path(graph, start, end):
    dist = {start: [start]}
    q = deque(start)
    while len(q):
        at = q.popleft()
        for next in graph[at]:
            if next not in dist:
                dist[next] = [dist[at], next]
                q.append(next)
    return dist.get(end)

path = find_shortest_path(graph,'A','T')
#print(path)


parent_graph = {}
visited = {}

path = ['A','B','C','D']

for node in path:
    parent_graph[node] = []
    visited[node]= True

def create_parent_graph(node, time):
    #print('analisando no ',node,'time', time)
    if node not in parent_graph:
        parent_graph[node] = []
    if time == 0 : 
        return
    for adj_node in graph[node] :
       # if not adj_node in visited:
        visited[adj_node]=True
        parent_graph[node].append(adj_node)
        create_parent_graph(adj_node, time-1)

for i in range(1,len(path)):
    node = path[i]
   # parent_graph[node].append(path[i-1])
    print('time',i,'no', node)
    print('grafo antes de chamar o metodo')
    print(parent_graph)
    create_parent_graph(node, i)
    print('grafo dps de chamar o metodo')
    print(parent_graph)