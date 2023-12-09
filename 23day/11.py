corridor_len = 11
def encode(x,y): return x + (corridor_len + 3) * y
def decode(w): return (w % (corridor_len + 3), w // (corridor_len + 3))

class State:
    costs = {
        'A': 1,
        'B': 10,
        'C': 100,
        'D': 1000,
        '.': 0,
    }
    finalrooms = {
        'A': 0,
        'B': 1,
        'C': 2,
        'D': 3,
    }

    def __init__ (self, corridor, rooms, cost):
        self.corridor = corridor
        self.rooms = rooms
        self.cost = cost

    def __repr__ (self):
        table = ['.'] * ((corridor_len + 3) * 5)
        table[corridor_len + 2::corridor_len + 3] = ['\n'] * 5
        table[0:corridor_len + 2] = ['#'] * (corridor_len + 2)
        table[::corridor_len + 3] = ['#'] * 5
        table[(corridor_len + 3) * 4:] = ['#'] * (corridor_len + 2)
        table[corridor_len + 1::corridor_len + 3] = ['#'] * 5

        for i,j in ((x,y) for x in (1,2,4,6,8,10,11) for y in (2,3)):
            table[encode(i,j)] = '#'
        for x, a in enumerate(self.corridor):
            table[encode(x+1, 1)] = a
        for x, ls in enumerate(self.rooms):
            x = 3 + x * 2
            table[encode(x, 2)], table[encode(x, 3)] = ls[0], ls[1]

        return ''.join(table)

    # x marks the position in the corridor
    def goalroom_available(self, x):
        rx = State.finalrooms[self.corridor[x]] 
        # *=2 to obtain the index in the corridor
        for p in range(min(x, (rx + 1) * 2), max(x, (rx + 1) * 2) + 1):
            if p != x and self.corridor[p] != '.':
                return False

        finalroom = self.rooms[rx]
        return (finalroom[0] == '.') and (finalroom[1] in ('.', self.corridor[x]))

    def room_reached(self, rx, j):
        a = self.rooms[rx][j]
        return (rx == State.finalrooms[a]) and \
            (j == 1 or j == 0 and self.rooms[rx][1] == a)

    def heuristic(self):
        cost = 0
        for x, a in enumerate(self.corridor):
            if a != '.':
                rx = State.finalrooms[a]
                cost += (abs(x - (rx+1)*2) + 1) * State.costs[a]

        for rx, l in enumerate(self.rooms):
            for ry, a in enumerate(l):
                if a != '.' and not self.room_reached(rx, ry):
                    rxx = State.finalrooms[a]
                    cost += (abs((rxx - rx)*2) + 1) * State.costs[a]

        return cost

    """
    notes: move from one room to another is wlog stopping midway in the 
    corridor, hence we can assume any movement is room -> corridor -> room
    """
    def next_moves(self):
        available_moves = [ ]
        for x, a in enumerate(self.corridor):
            if a != '.' and self.goalroom_available(x):
                rx = State.finalrooms[self.corridor[x]]
                new_room = ('.', a) if self.rooms[rx][1] == '.' else (a, a)
                cost = (abs(x - (rx+1)*2) + (2 if self.rooms[rx][1] == '.' else 1)) * State.costs[a]
                available_moves.append(State(
                    tuple(self.corridor[i] if i != x else '.'
                        for i in range(len(self.corridor))),
                    tuple(self.rooms[i] if i != rx else new_room
                        for i in range(len(self.rooms))),
                    self.cost + cost
                ))

        for rx, l in enumerate(self.rooms):
            ry, a = (0, l[0]) if l[0] != '.' else (1, l[1])
            if a == '.' or self.room_reached(rx, ry):
                continue

            n = 1 + next((i for i in range((rx+1) * 2, -1, -1) if self.corridor[i] != '.'), -1)
            m = next((i for i in range((rx+1) * 2, corridor_len) if self.corridor[i] != '.'), 11)
            pos = set(range(min(m,n), max(m,n))).difference({2,4,6,8})

            for nx in pos:
                new_room = ('.', l[1] if ry == 0 else '.')
                cost = (abs(nx - (rx+1)*2) + 1 + ry) * State.costs[a]
                available_moves.append(State(
                    tuple(self.corridor[i] if i != nx else a
                        for i in range(len(self.corridor))),
                    tuple(self.rooms[i] if i != rx else new_room
                        for i in range(len(self.rooms))),
                    self.cost + cost
                ))

        return available_moves

    def __hash__ (self):
        return hash(''.join(self.corridor) + ''.join(a+b for a,b in self.rooms))
    
    def __eq__ (self, other):
        return hash(self) == hash(other)

    def __lt__ (self, other):
        return self.cost < other.cost

s0 = State(('.',) * corridor_len, (
    ('D', 'B'), ('D', 'C'), ('B', 'A'), ('A', 'C')
), 0)

s1 = State(('.',) * corridor_len, (
    ('B', 'A'), ('C', 'D'), ('B', 'C'), ('D', 'A')
), 0)

sF = State(('.',) * corridor_len, (
    ('A', 'A'), ('B', 'B'), ('C', 'C'), ('D', 'D')
), None)

from queue import PriorityQueue
from collections import defaultdict

def A_star(start, goal):
    infty = 1e20
    gScore = defaultdict(lambda : infty) # exact
    gScore[start] = 0
    
    source_node = { start: None }
    open_set = PriorityQueue()
    open_set.put((start.heuristic(), start))

    while not open_set.empty():
        _, node = open_set.get()
        # print(node, gScore[node], len(open_set.queue))
        if node == goal:
            path = [node, ]
            # functional refactory throught reduce?
            while (pnode := source_node[path[-1]]) != None:
                path.append(pnode)
            return list(reversed(path)), gScore[node]

        gs = gScore[node]
        for neigh_node in node.next_moves():
            if (neigh_node.cost < gScore[neigh_node]):
                source_node[neigh_node] = node
                gScore[neigh_node] = neigh_node.cost
                open_set.put((neigh_node.cost + neigh_node.heuristic(), neigh_node))

    return None
