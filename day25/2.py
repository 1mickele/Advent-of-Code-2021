

def read_file(name):
	file = open(name)
	return [line.strip() for line in file.readlines()]


def parse_cucumbers(lines):
	return [[c for c in line] for line in lines]


def pretty_print(cucumbers):
	for row in cucumbers:
		print(''.join(row))
	print()


def number_of_steps(cucumbers):
    num_steps = 1
    while move_step(cucumbers):
        num_steps += 1
        print("\n", num_steps)
        pretty_print(cucumbers)
    return num_steps


def move_step(cucumbers):
	moved_east = move_herd(cucumbers, '>', (1, 0))
	moved_south = move_herd(cucumbers, 'v', (0, 1))
	return moved_east or moved_south


def move_herd(cucumbers, herd, offset):
	moving = []
	num_rows = len(cucumbers)
	num_cols = len(cucumbers[0])
	for y in range(num_rows - 1, -1, -1):
		for x in range(num_cols - 1, -1, -1):
			if cucumbers[y][x] != herd:
				continue
			target = ((x + offset[0]) % num_cols, (y + offset[1]) % num_rows)
			if cucumbers[target[1]][target[0]] == '.':
				moving.append(((x, y), target))
	if len(moving) == 0:
		return False
	for source, target in moving:
		cucumbers[target[1]][target[0]] = herd
		cucumbers[source[1]][source[0]] = '.'
	return True



cucumbers = parse_cucumbers(read_file("input.txt"))
num_steps = number_of_steps(cucumbers)
print(f"Part 1: {num_steps}")
