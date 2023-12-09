def read_input(nfile):
    scanner_readings = []
    with open(nfile) as file:
        while(file.readline()):
            reading = [ ]
            while((line := file.readline().rstrip()) and line != '\n'):
                reading.append(tuple(int(x) for x in line.split(',')))
            scanner_readings.append({
                'distances' : [],
                'reading': reading})
    return scanner_readings

def range2(n):
    for i in range(n):
        for j in range(n-i-1):
            yield (i, j+i+1)

def euclidean_distance(u, v):
    return sum(map(lambda t: (t[1] - t[0])**2, zip(u, v)))

# for sorted input lists
def common_values(ll, rl, key = lambda x: x): 
    commons = []
    itl, itr = 0, 0
    while (itl < len(ll) and itr < len(rl)):
        a,b = key(ll[itl]), key(rl[itr])
        if (a < b):
            itl += 1
        elif (a > b):
            itr += 1
        else:
            commons.append((ll[itl], rl[itr]))
            itl += 1
            itr += 1
    return commons

def vector_difference(ll, lr):
    return tuple(a-b for a,b in zip(ll, lr))

def vector_sum(ll, lr):
    return tuple(a+b for a,b in zip(ll, lr))

def manhattan(ll, lr):
    return sum(abs(x-y) for x,y in zip(ll, lr))

scanners = []

import functools
def matrix_mult(T, xs):
    fun = lambda a,p : a + p[0] * p[1]
    return tuple(functools.reduce(fun, zip(T[i], xs), 0)
        for i in range(len(T)))

import math
M = math.ceil((2000 * math.sqrt(3)) ** 2)
permutations = (
    lambda i,j,k: (i,j,k),
    lambda i,j,k: (i,k,j),
    lambda i,j,k: (j,i,k),
    lambda i,j,k: (j,k,i),
    lambda i,j,k: (k,i,j),
    lambda i,j,k: (k,j,i),
)

permutation_signs = {
    permutations[0]: 1,
    permutations[1]: -1,
    permutations[2]: -1,
    permutations[3]: 1,
    permutations[4]: 1,
    permutations[5]: -1,
}

# make it accept a index
def compute_distances(i, scanner_readings):
    # for r in scanner_readings:
    r = scanner_readings[i]
    d = r['reading']
    r['distances'] = sorted([{
            'distance': ed,
            'pair': (d[i], d[j])
        }
        for (i,j) in range2(len(d)) 
        if (ed := euclidean_distance(d[i], d[j])) <= M],
        key = lambda e: e.get('distance'))

    return scanner_readings

def test(i, j, scanner_readings, k):
    xs = common_values(scanner_readings[i]['distances'], 
                       scanner_readings[j]['distances'],
                       key = lambda b: b['distance'])
    return (vector_difference(*xs[k][0]['pair']),
            vector_difference(*xs[k][1]['pair']))

def forall(ls, key):
    for x in ls:
        if not key(x):
            return False
    return True

def check_pair(i, j, scanner_readings):
    cv = common_values(scanner_readings[i]['distances'], 
                       scanner_readings[j]['distances'],
                       key = lambda b: b['distance'])
    if len(cv) < 65: return
    # print(len(cv))

    valid_permutations = list(permutations)
    while len(valid_permutations) > 1:
        valid_permutations[:] = [s for s in valid_permutations
                if forall(cv, key = lambda p: forall(
                        zip(vector_difference(*p[0]['pair']),
                            s(*vector_difference(*p[1]['pair']))),
                        key = lambda t: abs(t[0]) == abs(t[1])
                    )
                )]

    if len(valid_permutations) == 0:
        fuzzy_permutation = { }
        for sgm in permutations:
            for p in cv:
                if (forall(zip(vector_difference(*p[0]['pair']),
                            sgm(*vector_difference(*p[1]['pair']))),
                           key = lambda t: abs(t[0]) == abs(t[1]))):
                    fuzzy_permutation.setdefault(sgm, 0)
                    fuzzy_permutation[sgm] += 1
        permutation = max(fuzzy_permutation, key=lambda x: fuzzy_permutation[x])
    else:
        permutation = valid_permutations[0]

    signs = [(1 if x == y else -1) for x,y in zip(
        vector_difference(*cv[0][0]['pair']),
        permutation(*vector_difference(*cv[0][1]['pair']))
        )]

    # det T = 1 = prod(sign) * sign(permutation)
    if (permutation_signs[permutation] != 
        functools.reduce(lambda x,y: x*y, signs)):
        signs = [-1*s for s in signs]

    T = [[signs[i] * (permutation(0,1,2)[i] == j) for j in range(3)] 
            for i in range(3)]

    valid_centers = {}
    for p in cv:
        lambdas = p[0]['pair']
        mus = p[1]['pair']
        c1 = vector_difference(lambdas[0], matrix_mult(T, mus[0]))
        c2 = vector_difference(lambdas[0], matrix_mult(T, mus[1]))
        valid_centers.setdefault(c1, 0)
        valid_centers.setdefault(c2, 0)
        valid_centers[c1] += 1
        valid_centers[c2] += 1
    c = max(valid_centers, key=lambda p: valid_centers[p])
    
    # print(c, len(cv), valid_centers[c])
    if (valid_centers[c] < 65):
        return

    scanners.append(c)
    

    ## merge scanner_reading j with i
    for ps in (srj := scanner_readings[j]['reading']):
        (sri := scanner_readings[i]['reading'])[len(sri): len(sri)] = \
            [vector_sum(c, matrix_mult(T, mu)) for mu in srj]

    sri = scanner_readings[i]['reading']
    scanner_readings[i]['reading'] = list({x for x in sri})
    compute_distances(i, scanner_readings)
    del scanner_readings[j]

    return permutation, signs, T, c, cv

scanner_readings = read_input("input.txt")
for i in range(len(scanner_readings)):
    compute_distances(i, scanner_readings)

while (l := len(scanner_readings)) > 1:
    for i,j in range2(l):
        if check_pair(i, j, scanner_readings) != None:
            break

print(len(scanner_readings[0]['reading']))


distances = [manhattan(scanners[i], scanners[j]) 
    for i,j in range2(len(scanners))]
print(max(distances))

