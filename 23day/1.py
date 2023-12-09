corridor_len = 11

def encode(x,y):
    return x + (corridor_len + 2) * y

def decode(w):
    return (w % (corridor_len + 2), w // (corridor_len + 2))

class State:
    # in the end, a State structure is massive, less then ideal 
    # using the A* algorithm
    def __init__ (self, st, ct):
        self.states = st
        self.cost = ct
        self.hash = hash(''.join(list(map(lambda p: p[0] + str(p[1]) + str(p[2]), self.states))))

    costs = {
        'A': 1,
        'B': 1,
        'C': 1,
        'D': 1,
        '.': 0,
    }

    moving_state = 1
    stopped_state = 2
    restarted_state = 3
    final_state = 4

    def _gen_table(self):
        table = ['.'] * ((corridor_len + 2) * 5)
        table[0:corridor_len + 2] = ['#'] * (corridor_len + 2)
        table[::corridor_len + 2] = ['#'] * 5
        table[corridor_len + 1::corridor_len + 2] = ['#'] * 5
        for i,j in ((x,y) for x in (1,2,4,6,8,10,11) for y in (2,3)):
            table[encode(i,j)] = '#'
        table[(corridor_len + 2) * 4:] = ['#'] * (corridor_len + 2)
        for (amph, w, _) in self.states:
            table[w] = amph
        return table

    def has_reached(amph, pos, table):
        x,y = decode(pos)
        rx = next(rx for (rc, rx) in State.amph_rooms if rc == amph)
        if rx == x and y == 3:
            return True
        if rx == x and y == 2 and table[encode(rx,3)] == amph:
            return True
        return False

    def prepare(self):
        return State(tuple(map(lambda s: (s[0], s[1], \
            State.final_state if State.has_reached(s[0], s[1], self._gen_table()) else 0), self.states)), 0)
    
    def __repr__ (self):
        display = ''.join(self._gen_table())
        return '\n'.join((display[a*(corridor_len + 2): (a+1)*(corridor_len + 2)] 
            for a in range(0, 5)))

    # return a list of new states and their distance from original
    # the uneven shape makes this function a pain in the @$$ 
    amph_rooms = (('A', 3), ('B', 5), ('C', 7), ('D', 9))

    def heuristic(self):
        table = self._gen_table()
        cost = 0
        for amph, pos, state in self.states:
            if State.has_reached(amph, pos, table):
                continue 
            x,y = decode(pos)
            rx = next(rx for (rc, rx) in self.amph_rooms if rc == amph)
            dy = abs(1-y) if x != rx else 0
            dyy = 1 if table[encode(rx, 3)] == amph else 1.5
            cost += self.costs[amph] * (abs(x-rx) + dy + dyy)
                
            ###
        return cost


    def next_moves(self):
        table = self._gen_table()
        available_moves = []

        def room_unaccessible(c, x):
            return False if (c,x) in self.amph_rooms else True

        def finalroom_accessible(amph, pos):
            nonlocal table

            x,y = decode(pos)
            rx = next(rx for (rc, rx) in self.amph_rooms if rc == amph)
            # Already in the right place
            if (x,y) == (rx, 3):
                return True
            if (x,y) == (rx, 2):
                return table[encode(rx, 3)] in ('.', amph)
            # Room is viable
            if table[encode(rx, 2)] != '.' or (table[encode(rx, 3)] not in ('.', amph)):
                return False
            # Corridor is viable
            ls = list(range(min(x,rx), max(x, rx) + 1))
            if y == 1:
                ls.remove(x)
            if (x != rx) and any(table[encode(t,1)] != '.' for t in ls):
                return False
            if y == 3:
                return table[encode(x,2)] == '.'
            return True
    
        def valid_moves(amph, pos):
            x,y = decode(pos)
            tentatives = [(nx, ny) for (nx, ny) in ((x+1,y), (x-1,y), (x,y-1), (x, y+1)) \
                            if table[encode(nx, ny)] == '.']
            if (y == 1 and (x, y+1) in tentatives and
                    room_unaccessible(amph, x)):
                tentatives.remove((x, y+1))
            return tentatives
            
        """
        # ignoring the rules like a chad wont work..
        for i,(amph, pos, state) in enumerate(self.states):
            x,y = decode(pos)
            for (nx, ny) in valid_moves(amph, pos):
                ns = list(self.states)
                ns[i] = (amph, encode(nx, ny), state)
                available_moves.append(State(tuple(ns), self.cost + self.costs[amph]))
        """

        for x in (3,5,7,9): 
            if table[encode(x, 1)] != '.': 
                nsl = list(self.states)
                k = next(k for k in range(len(self.states))
                        if self.states[k][1] == encode(x, 1))
                amph, pos, state = self.states[k]
                for (nx, ny) in valid_moves(amph, pos):
                    nsl[k] = (amph, encode(nx, ny), state)
                    available_moves.append(State(tuple(nsl), self.cost + self.costs[amph]))
                return available_moves

        k = next((k for k in range(len(self.states))
                if self.states[k][2] == State.restarted_state), None)

        # if an amph is restarted it's the only one able to move
        if k == None:
            j = next((j for j in range(len(self.states))
                    if self.states[j][2] == State.moving_state), None)

            for i,(amph, pos, state) in enumerate(self.states):
                if state == State.final_state or \
                    (state == State.stopped_state and not finalroom_accessible(amph, pos)):
                    continue

                nsl = list(self.states)
                if j != None and i != j:
                    nsl[j] = (nsl[j][0], nsl[j][1], State.stopped_state)
                    # for all but one case, the now moving amph will be stopped

                for (nx, ny) in valid_moves(amph, pos):
                    if state == State.moving_state or state == 0:
                        ns = State.moving_state
                    else:
                        ns = State.restarted_state
                    for (nx, ny) in valid_moves(amph, pos):
                        nsl[i] = (amph, encode(nx, ny), ns)
                        available_moves.append(State(tuple(nsl), self.cost + self.costs[amph]))
        else:
            # moving the restarted one
            amph, pos, state = self.states[k]
            for (nx, ny) in valid_moves(amph, pos):
                nsl = list(self.states)
                nsl[k] = amph, encode(nx, ny), \
                    State.final_state if State.has_reached(amph, encode(nx, ny), table) else State.restarted_state
                available_moves.append(State(tuple(nsl), self.cost + self.costs[amph]))

        return available_moves

    def __eq__ (self, other):
        # return self.hash == other.hash
        return hash(''.join(self._gen_table())) == hash(''.join(other._gen_table()))
        # +  ''.join(map(lambda x: str(x[2]), self.states)))

    def __hash__ (self):
        return self.hash

    def __lt__ (self, other):
        self.cost < other.cost


s0 = State(
    (
        ('D', encode(3, 2), 0), ('B', encode(3, 3), 0),
        ('D', encode(5, 2), 0), ('C', encode(5, 3), 0),
        ('B', encode(7, 2), 0), ('A', encode(7, 3), 0),
        ('A', encode(9, 2), 0), ('C', encode(9, 3), 0),
    ),
    0,
).prepare()

sT = State(
    (
        ('A', encode(3, 2), 0), ('A', encode(3, 3), 0),
        ('B', encode(5, 2), 0), ('B', encode(5, 3), 0),
        ('C', encode(7, 2), 0), ('C', encode(7, 3), 0),
        ('D', encode(9, 2), 0), ('D', encode(9, 3), 0),
    ),
    None,
)

sF = State(
    (
        ('A', encode(5, 2), 0),
        ('B', encode(1, 1), 0),
        ('C', encode(3, 3), 0),
        ('B', encode(7, 3), 0),
        ('D', encode(9, 1), 0), ('D', encode(9, 3), 0),
    ),
    None,
)

from queue import PriorityQueue
from collections import defaultdict

def A_star(start, goal):
    infty = 1e20
    gScore = defaultdict(lambda : infty) # exact
    gScore[start] = 0
    
    source_node = { start: State([], None) }
    open_set = PriorityQueue()
    open_set.put((start.heuristic(), start))

    while not open_set.empty():
        _, node = open_set.get()
        # print(node, gScore[node], len(open_set.queue))
        if node == goal:
            path = [node, ]
            # functional refactory throught reduce?
            while (pnode := source_node[path[-1]]) != State([], None):
                path.append(pnode)
            return list(reversed(path)), gScore[node]

        gs = gScore[node]
        for neigh_node in node.next_moves():
            if (neigh_node.cost < gScore[neigh_node]):
                source_node[neigh_node] = node
                gScore[neigh_node] = neigh_node.cost
                open_set.put((neigh_node.cost + neigh_node.heuristic(), neigh_node))

    return None
