import math
def to_yield(l):
    for x in l:
        yield x

class ALU:
    opcode_table = {
        'add': lambda x,y : x + y,
        'mul': lambda x,y : x * y,
        'div': lambda x,y : x // y + (math.copysign(1, x // y) == -1),
        'mod': lambda x,y : x % y,
        'eql': lambda x,y : int(x == y)
    }

    def __init__ (self, z = 0):
        self.x = 0
        self.y = 0
        self.z = z
        self.w = 0

    def run(self, inputl, instructionl):
        read_reg = lambda x: getattr(self, x)
        for instr in instructionl:
            match instr:
                case ('inp', _out):
                    setattr(self, _out, next(inputl))
                case (opcode, ('x' | 'y' | 'z' | 'w' as _out), (('x' | 'y' | 'z' | 'w' as _in) | int(_in))) \
                    if (type(_in) == str and (_in := read_reg(_in))) or (type(_in) == int):
                    x = self.opcode_table[opcode](read_reg(_out), _in)
                    setattr(self, _out, self.opcode_table[opcode](read_reg(_out), _in))
                case _:
                    print("fuck")
        z = self.z
        # self.x = self.y = self.z = self.w = 0
        return z

    def __repr__ (self):
        return f"x={self.x}, y={self.y}, z={self.z}, w={self.w}"

def parse(filename):
    def isnumber(x):
        try:
            int(x)
            return True
        except:
            return False

    with open(filename) as file:
        return tuple(
            tuple(x if not isnumber(x) else int(x)
            for x in line.strip().split(' '))
        for line in file)

from itertools import islice
def select_block(ins, i):
    matches = (idx for idx, val in enumerate(ins) if val[0] == 'inp')
    return ins[next(islice(matches, i, i+1)): next(matches, len(ins))] 

def index_block(ins, i):
    matches = (idx for idx, val in enumerate(ins) if val[0] == 'inp')
    return next(islice(matches, i, i+1), len(ins))

def analyze(ins):
    res = []
    for idx, val in enumerate(ins):
        if val[0] == 'inp':
            l = (ins[idx + 4], ins[idx + 5], ins[idx + 15])
            #print(len(res), l)
            res.append(l)
    return res

# run backward
def test4(d):
    res = { 0: [] }
    ll = parse("input.txt")
    for i in range(13, 13-d, -1):
        tres = dict()
        # only positive values will come out from the whole computation
        for z in range(0, 2000):
            for w in range(1,10):
                alu = ALU()
                alu.z = z
                t = alu.run(
                    iter(to_yield([w])),
                    select_block(ll, i)
                    )
                if t in res:
                    tres[z] = [w] + res[t]

        print(len(tres))
        res = tres
    return res

def match_updown(ls):
    stack = []
    res = []
    for i,x in enumerate(ls):
        if x > 0:
            stack.append(i)
        else:
            res.append((stack.pop(), i))

    return res

def solve1(ins):
    """
    basically, the input program can be seen as 14 routines, each having
    only one result z and depending only on the external input w and the
    z computed by the previous routine.

    each routine will either be ascending / descending:
    * ascending routine will be of the form 
        f_n(z,w) = 26 * z + w_n + a_n, if register x == 1
                 = z if register x == 0
    * descending
        g_n(z,w) = z // 26 * 26 + b_n, if register x == 0
                 = z // 26              otherwise

    Now, each ascending routine will never have register x equal to 0
    so that in order to reach 0 again, all ascending rotine multiplying by 26
    the value z must have a corresponding descending routine hitting the condition
    register x = 0.

    after paring with match_updown() and the math, any pair i,j 
    must satisfy w_i + k == w_j where k = a_n + c_n; a comes from the
    condition register x == 0 of j, i.e. z % 26 + c_n = w_i + a_n + c_n ==? w_j

    even though this is a clumsy explanation, the key fact is that the 
    value a_n will only be seen by the matching descending routine, in all
    the previous one it will be killed by % 26.
    """

    t = analyze(ins)
    m = match_updown(map(lambda l: 1 if l[0][2] == 1 else -1, t))
    ws = [0]*14 

    for i,j in m:
        k = t[i][2][2] + t[j][1][2]
        # w_i + k == w_j 
        ws[i] = 9 - (k > 0) * k
        ws[j] = ws[i] + k

    return ''.join(map(str, ws))

def solve2(ins):
    t = analyze(ins)
    m = match_updown(map(lambda l: 1 if l[0][2] == 1 else -1, t))
    ws = [0]*14 

    for i,j in m:
        k = t[i][2][2] + t[j][1][2]
        # w_i + k == w_j 
        if k > 0:
            ws[i] = 1
        else:
            ws[i] = 1 - k
        ws[j] = ws[i] + k

    return ''.join(map(str, ws))
        

"""
in the end, no simulation of the program is required at all
"""

r = solve1(parse("input.txt"))
s = solve2(parse("input.txt"))
