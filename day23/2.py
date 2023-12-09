from queue import PriorityQueue
from collections import defaultdict

vertex = set("ABCDEFG")
#     B D
# G A  \  F
#     C E 

distance = {
    ('G', 'A'): 4,
    ('A', 'B'): 10, ('A', 'C'): 5,
    ('B', 'D'): 10, ('B', 'E'): 10,
    ('C', 'E'): 10,
    ('D', 'F'): 10,
    ('E', 'F'): 10,
    ('G', 'F'): 6
}

def to_symmetric(dicty, x, y):
    if (x,y) in dicty:
        return dicty[(x,y)]
    elif (y,x) in dicty:
        return dicty[(y,x)]
    raise KeyError(f'({x}, {y})')

heuristic = {
    'G': 2, 'A': 4, 'B': 3, 'C': 3,
    'D': 2, 'E': 2, 'F': 0
}

neighbour = {
    'A': ('G', 'C', 'B'), 
    'B': ('D', 'E'),
    'C': ('E'),
    'D': ('F'),
    'E': ('F'),
    'G': ('F'),
    'F': ('G', 'D', 'E')
}

infty = 1e20

def A_star(start, goal):
    gScore = defaultdict(lambda : infty) # exact
    gScore[start] = 0
    
    source_node = { start: None }
    open_set = PriorityQueue()
    open_set.put((heuristic[start], start))

    while not open_set.empty():
        _, node = open_set.get()
        print(open_set.queue)
        if node == goal:
            path = [goal, ]
            # functional refactory throught reduce?
            while (pnode := source_node[path[-1]]) != None:
                path.append(pnode)
            return list(reversed(path)), gScore[node]

        gs = gScore[node]
        for neigh_node in neighbour[node]:
            if ((gss := gs + to_symmetric(distance, node, neigh_node)) < gScore[neigh_node]):
                source_node[neigh_node] = node
                gScore[neigh_node] = gss
                open_set.put((gss + heuristic[neigh_node], neigh_node))

    return None

print(A_star('A', 'F'))

