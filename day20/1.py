l = 0
def read_input(nfile):
    with open(nfile) as file:
        algorithm = file.readline().rstrip()
        image = { }
        file.readline() # discard line
        i = 0
        global l
        while((line := file.readline().rstrip())):
            l = len(line) + 1
            for j,x in enumerate(line):
                image[i, j] = x
            i += 1
        
        return algorithm, image

algorithm, image = read_input('input.txt')
background = '.'

def get(i,j):
    return image[i,j] if (i,j) in image else background

def addborder(i, l):
    pos = [i, i] # -1,-1
    directions = [(1, 0), (0,1), (-1, 0), (0, -1)]
    for d in directions:
        for _ in range(l):
            image[pos[0], pos[1]] = background
            pos = [pos[0] + d[0], pos[1] + d[1]]

def nextborder():
    i = -1
    global l
    def run():
        nonlocal i
        global l
        addborder(i,l)
        i -= 1
        l += 2
    return run
nextborder = nextborder()

def display():
    (x0,x1) = min(image.keys(), key = lambda x: x[0])[0], \
                max(image.keys(), key = lambda x: x[0])[0]
    (y0,y1) = min(image.keys(), key = lambda x: x[1])[1], \
                max(image.keys(), key = lambda x: x[1])[1]

    for x in range(x0, x1 + 1):
        for y in range(y0, y1 + 1):
            print(image[x,y], end='')
        print('')

import functools

def enhance():
    neighbours = [(i,j) for i in range(1,-2,-1) for j in range(1,-2,-1)]
    
    global image, background

    (x0,x1) = min(image.keys(), key = lambda x: x[0])[0], \
                max(image.keys(), key = lambda x: x[0])[0]
    (y0,y1) = min(image.keys(), key = lambda x: x[1])[1], \
                max(image.keys(), key = lambda x: x[1])[1]

    # nextborder()
    imagetemp = dict(image)
    
    for x in range(x0, x1 + 1):
        for y in range(y0, y1 + 1):
            val = functools.reduce(lambda a,b: a + b[0] * 2**b[1],
                zip(map(lambda x: 1 if x == '#' else 0, 
                    [get(x + i, y + j) for i,j in neighbours]), range(9)), 0)
            imagetemp[x,y] = algorithm[val]
    image = imagetemp
    background = algorithm[0 if background=='.' else -1]

for i in range(50):
    nextborder()
    enhance()

print(sum(map(lambda x: 1 if x == '#' else 0, image.values())))
# 19066
