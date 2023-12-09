def parse(filename):
    res = []
    with open(filename) as file:
        for line in file:
            res.append(list(x for x in line.strip()))
    return res

def display(state):
    print('\n'.join(''.join(row) for row in state))

def iterate(state):
    display(state)
    rows, columns = len(state), len(state[0])

    for x in range(1, 2**10):
        changed = False

        for i in range(rows):
            loopback = True
            skip = False
            if state[i][0] != '.':
                loopback = False
            for j in range(columns):
                k = (j + 1) % columns
                if (j == columns - 1) and not loopback:
                    continue 
                if state[i][j] == '>' and state[i][k] == '.' and not skip:
                    state[i][k], state[i][j] = state[i][j], '.'
                    skip = changed = True
                    continue
                skip = False

        for j in range(columns):
            loopback = True
            skip = False
            if state[0][j] != '.':
                loopback = False
            for i in range(rows):
                k = (i + 1) % rows
                if (i == rows - 1) and not loopback:
                    continue 
                if state[i][j] == 'v' and state[k][j] == '.' and not skip:
                    state[k][j], state[i][j] = state[i][j], '.'
                    skip = changed = True
                    continue
                skip = False

        print("\n", x)
        display(state)

        if not changed:
            print(x)
            return 

iterate(parse("input.txt"))
