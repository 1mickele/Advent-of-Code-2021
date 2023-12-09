# pos = [7, 10]
scores = [0, 0]
player = 0
rolls = 0

def rolldice():
    global rolls
    i = -1
    while True:
        i += 3
        rolls += 3
        yield i * 3
roll = rolldice()
wrap = lambda x: x - int((x - 0.5) / 10) * 10

def sol1():
    while max(scores) < 1000:
        def compute(i, x): 
            global pos
            pos[i] = wrap(pos[i] + x)
            print(pos)
            return i, pos[i]

        def update(i, x):
            global scores
            scores[i] += x # [(scores[0] + x), (scores[1] + y)]
            return scores[i]

        update(*compute(player, next(roll)))
        player = int(not player)

    print(rolls * min(scores))

wins = [0,0]
state = { ((7,0),(10,0)) : 1 }

def range_product(l, *ls):
    if len(ls) == 0:
        return [[x] for x in l]
    return [(x, *y) for x in l for y in list(range_product(*ls))]

import functools

ls = tuple(map(sum, range_product(range(1,4), range(1,4), range(1,4))))
frequencies = functools.reduce(lambda d,x: (d, d.update({x: ls.count(x)}))[0] , ls, {})

# (6,6), (1,1) -> (10,16), (2, 3)
def sol2():
    global state
    while len(state) != 0:
        newstate = { }
        for (p1,s1), (p2,s2) in state:
            l1 = tuple(map(lambda np: (np[0], np[1], s1 + np[1]), 
                ((frequencies[o], wrap(o + p1)) for o in frequencies.keys())))
            l2 = tuple(map(lambda np: (np[0], np[1], s2 + np[1]), 
                ((frequencies[o], wrap(o + p2)) for o in frequencies.keys())))

            for (f1, np1, ns1) in l1:
                if ns1 >= 21:
                    wins[0] += state[((p1, s1), (p2, s2))] * f1
                    continue
                for (f2, np2, ns2) in l2:
                    f = f1 * f2
                    if ns2 >= 21:
                        wins[1] += state[((p1, s1), (p2, s2))] * f
                        continue
                    newstate.setdefault(((np1, ns1), (np2, ns2)), 0)
                    newstate[((np1, ns1), (np2, ns2))] += \
                        state[((p1, s1), (p2, s2))] * f

        state = newstate

        # (o for o in range(3))
        # o -> wrap(o + p1) = p1 -> (p1, s1 + p1) 
        # o -> wrap (..)
        # l1,l2 

sol2()
