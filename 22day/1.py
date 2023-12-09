def successive_pairs(l):
    return ((l[i], l[i+1]) for i in range(len(l) - 1))
    
def segment_intersect(l1, l2):
    return (l1[0] <= l2[0] <= l1[1]) or \
        (l2[0] <= l1[0] <= l2[1])

def middle_point(p1, p2):
    return tuple((p1[i] + p2[i]) / 2 for i in range(3))

class HypRect:
    def __init__ (self, p1, p2, state = 1):
        self.p1 = p1
        self.p2 = p2
        self.state = state

    def __contains__ (self, p):
        a,b,c = p
        return (self.p1[0] <= a <= self.p2[0]) and \
            (self.p1[1] <= b <= self.p2[1]) and \
            (self.p1[2] <= c <= self.p2[2])

    def side(self, i):
        return (self.p1[i], self.p2[i])

    def intersects(self, t):
        return all(segment_intersect(self.side(i), t.side(i)) 
            for i in range(3))

    # hr, hpi
    def intersection(self, t):
        select_sign = {
            (1,1): -1,
            (1,-1): -1,
            (-1,1): 1,
            (-1,-1): 1
        }
        #select_sign = lambda s1, s2: -1 if s1 + s2 >= 0 else 1
        l0 = tuple(sorted((*self.side(0), *t.side(0))))
        l1 = tuple(sorted((*self.side(1), *t.side(1))))
        l2 = tuple(sorted((*self.side(2), *t.side(2))))

        return HypRect((l0[1], l1[1], l2[1]), (l0[2], l1[2], l2[2]), 
            select_sign[(self.state, t.state)])

    def volume(self):
        import functools
        return functools.reduce(lambda x,p: x * (p[1] - p[0] + 1),
            zip(self.p1, self.p2), 1) * self.state

    def __repr__ (self):
        return f"HypRect({self.p1}, {self.p2}, {'on' if self.state >= 1 else 'off'})"

def parse(fname): 
    instructions = [ ]
    with open(fname) as file:
        for line in file:
            line = line.split()
            l = tuple(tuple(int(d) for d in c[2:].split('..')) 
                for c in line[1].split(','))
            instructions.append(HypRect(*tuple(zip(*l)), 1 if line[0] == 'on' else -1))
    return instructions


# TODO 'beautify'
# Algorithm works well only on eudlicean space topology..
def tassellate(c : HypRect, d: HypRect):
    # assuming they interect
    tasselation = [ ]
    l0 = tuple(sorted((*c.side(0), *d.side(0))))
    l1 = tuple(sorted((*c.side(1), *d.side(1))))
    l2 = tuple(sorted((*c.side(2), *d.side(2))))

    
    for (x0,x1) in successive_pairs(l0):
        for (y0,y1) in successive_pairs(l1):
            for (z0,z1) in successive_pairs(l2):
                # if (middle_point((x0, y0, z0), (x1, y1, z1)) in d) and d.state:
                    # if d.state == False => discard
                    # if d.state == True => keep it
                    # tasselation.append(HypRect((x0, y0, z0), (x1, y1, z1)))
                print((x0, y0, z0), (x1, y1, z1))
                if ((m := middle_point((x0, y0, z0), (x1, y1, z1))) in c) and (not m in d):
                    a = 1 if (x0, y0, z0) in d else 0
                    b = int (not a)
                    tasselation.append(HypRect((x0, y0, z0), (x1, y1, z1), True))

    return tasselation

def solve1old(hyprect_instrs):
    hyprects = []
    newhyprects = []
    for hpi in hyprect_instrs:
        for hr in hyprects:
            if (hr.intersects(hpi)):
                newhyprects.extend(tassellate(hr, hpi))
            else:
                newhyprects.append(hr)
        if hpi.state:
            newhyprects.append(hpi)
        hyprects = newhyprects 
        newhyprects = []
        print(hyprects)
        
    return sum(map(lambda c: c.volume(), hyprects))

def solve1(hyprect_instrs):
    hyprects = []
    for hpi in hyprect_instrs:
        newhyprects = []
        if hpi.state == 1:
            newhyprects.append(hpi)
        for hr in hyprects:
            if hr.intersects(hpi):
                newhyprects.append(hr.intersection(hpi))
        hyprects.extend(newhyprects)
        # print(hyprects, sum(map(lambda c: c.volume(), hyprects)))

    return sum(map(lambda c: c.volume(), hyprects))


c = HypRect((2, 4, 10), (4, 6, 20), True)
d = HypRect((4, 6, 0), (8, 8, 9), True)
# print(c.intersect(d))

instructions = parse('input.txt')
s = solve1(instructions)


